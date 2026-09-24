# 05. Order, hash, heterogeneous lookup

Containers that **find by key** need a total idea of "same" and,
for trees, "before". Getting this wrong compiles and then
duplicates keys, or hashes everything into one bucket.

## Strict weak ordering (`set` / `map` / `sort`)

`comp(a, b)` must be a **strict** `<`:

```
  irreflexive     comp(x, x) == false
  asymmetric      comp(a,b) => !comp(b,a)
  transitive      comp(a,b) && comp(b,c) => comp(a,c)
  equivalence     !comp(a,b) && !comp(b,a)  is an equivalence
                  and that must be transitive too
```

`<=` as a comparator is undefined behavior (`sort` / `set` may
infinite-loop or drop elements). Equality is derived: two keys
are equivalent iff neither is less.

```
  set with less<int>:     1 < 2 < 3
  set with greater<int>:  3 before 2 before 1 in a walk
```

Custom: a functor or `decltype` of a lambda (C++20 allows
stateless lambdas as template arguments more easily; storing a
stateful lambda needs the comparator passed to the constructor).

```
  // people sorted by last name, then first
  auto cmp = [](const Person& a, const Person& b) {
      if (a.last != b.last) return a.last < b.last;
      return a.first < b.first;
  };
  std::set<Person, decltype(cmp)> s(cmp);
```

**NaN in a `set<double>`:** `NaN < x` is false and `x < NaN` is
false, so NaN is equivalent to everyone — the tree invariant
dies. Do not put NaN in ordered containers.

## Hash + equality (`unordered_*`)

Two requirements, both yours if `T` is not a standard type:

```
  if a == b  then  hash(a) == hash(b)     (must)
  hash(a) == hash(b)  may still have a != b   (collision, OK)
```

If `==` says equal but hashes differ, `find` misses an element
that `==` would match. If `==` is "close enough" (floats) you
get a lottery.

```
  buckets
  0   1   2   3
      |       |
     [k]    [a]->[b]     collision chain; == walks the chain
```

Quality: mix the bits. A hash that returns `x.first` for
`pair<int,int>` puts every `(5, *)` in one bucket.

```cpp
struct PairHash {
    std::size_t operator()(const std::pair<int, int>& p) const {
        std::size_t h1 = std::hash<int>{}(p.first);
        std::size_t h2 = std::hash<int>{}(p.second);
        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));  // boost-style mix
    }
};
std::unordered_set<std::pair<int, int>, PairHash> seen;
```

`reserve(n)` **before** a known flood. `max_load_factor` defaults
near 1. Worst case O(n) is real for adversarial keys; C++14
and later libraries often hash integers in a way that resists
trivial attacks, but a custom hash is still your job.

There is **no** standard `hash<pair<T,U>>` or `hash<vector<T>>`.
That is why people sort a string and use `hash<string>` as an
anagram key instead.

## Heterogeneous lookup (C++14 transparent)

`set<string>::find("x")` constructs a **temporary `string`**
("x" is `const char*`). On a hot path that is an allocation
(unless SSO eats it). Transparent comparators compare a
`string_view` to the stored `string` without building one.

```
  set<string> ordinary
  find("foo")     ---- allocates / SSO string ---->  tree walk

  set<string, less<>>  (C++14, less<void> is transparent)
  find("foo"sv)   ---- compares view to node ---->  tree walk
```

```cpp
std::set<std::string, std::less<>> s;   // less<> is transparent
s.insert("alpha");
if (s.contains(std::string_view{"alpha"})) { /* no extra string */ }
auto it = s.find("al");                 // also works; const char*
```

`map::find`, `count`, `contains`, `equal_range`, `lower_bound`,
`upper_bound` all participate. `unordered_map` needs a
**transparent hash and eq** (`is_transparent` in both) to
`find(string_view)` without a `string`. libstdc++ / libc++
support this for `unordered_map<string, V>` with extra template
knobs; the portable pattern is a custom hasher:

```cpp
struct SvHash {
    using is_transparent = void;
    std::size_t operator()(std::string_view s) const {
        return std::hash<std::string_view>{}(s);
    }
    std::size_t operator()(const std::string& s) const {
        return std::hash<std::string>{}(s);
    }
};
struct SvEq {
    using is_transparent = void;
    bool operator()(std::string_view a, std::string_view b) const { return a == b; }
};
std::unordered_map<std::string, int, SvHash, SvEq> hits;
hits.find("path");   // no temporary string
```

**Why bother?** A cache keyed by URL, a symbol table, a
dictionary. If you `find` more than you `insert`, the temporary
`string` is the tax you stop paying.

## `key_eq` vs identity

`unordered_multimap` allows duplicate keys. `equal_range(k)` is
the bucket slice. Prefer `unordered_map<K, vector<V>>` when you
want "all V for K" as an explicit list (clearer CRUD, one hash).

Practice: [`examples/17_hash_transparent.cpp`](examples/17_hash_transparent.cpp).
Related: [`containers/set.md`](containers/set.md),
[`containers/unordered_map.md`](containers/unordered_map.md).
