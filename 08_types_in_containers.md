# 08. Types that are legal inside a container

The container is a template. It generates `push_back`, `sort`,
`find` **for your `T`**. If `T` cannot be moved the way
`push_back` needs, the error is in `stl_vector.h` and the bug
is in `T`. This chapter is the contract `T` must keep.

```
  vector<Widget>        needs: move or copy into a new slot
  set<Widget>           needs: strict <  (see 05)
  unordered_set<Widget> needs: hash + ==
  sort(v.begin(), v.end())  needs: swappable, movable, <
```

## Rule of Zero first

If every member is a container / `string` / smart pointer, you
write **nothing**. The compiler's copy, move, and destructor
are correct. The moment you write one of the five, you write
all five (or `= delete` the ones you do not want).

```
  Widget
    string name;          // already owns
    vector<int> ids;
  => default move steals name and ids.  vector<Widget> realloc is cheap.
```

## Realloc cares about `noexcept` move

```
  vector grows:
    old  [ W0 | W1 | W2 ]
    new  [  ? |  ? |  ? |  ? |  ? |  ? ]

  if move ctor is noexcept:  steal W0, W1, W2 into new, free old     fast
  if move ctor can throw:    **copy** W0, W1, W2, then free old
                             (a throw must leave the vector unchanged)
```

A `vector<string>` realloc is a handful of pointer moves.
A `vector<Widget>` whose move is not `noexcept` copies every
Widget. Mark moves `noexcept` when they truly cannot throw
(they only steal members that do not throw).

`swap` of two `T` should be `noexcept` too: `sort` uses it.

## What each operation asks of `T`

| You call | `T` must |
| --- | --- |
| `push_back(t)` / `insert` | CopyInsertable or MoveInsertable |
| `emplace_back(args...)` | construct from those args in the slot |
| `v[i] =` | Copy/MoveAssignable |
| `erase` | destructor; for `vector`, MoveAssign the tail left |
| `std::sort` | MoveAssignable, swappable, strict `<` |
| `set` / `map` key | Copy/MoveConstructible, strict `<`, **not mutated** through `*it` |
| `unordered_*` key | hash + `==`, same "hash agrees with ==" rule as [`05`](05_hash_compare_lookup.md) |

You do **not** need `<` for a `vector` you never sort. You do
not need a hash for a `set`. Add the piece the container
actually calls.

## `const` iterators

```
  vector<int> v{1,2,3};
  auto it  = v.begin();     // iterator: *it is int&
  auto cit = v.cbegin();    // const_iterator: *cit is const int&
  const vector<int>& cv = v;
  auto x = cv.begin();      // already const_iterator
```

A `const` container only hands out `const_iterator`. Algorithms
that write (`sort`, `copy` into) will not compile. That is the
type system enforcing Read-only.

`set::iterator` is already a const key (mutating would break
the tree). `map::iterator` has `const` key, mutable `second`.

## `initializer_list` is a one-shot copy

```
  vector<string> v{"a", "b", "c"};   // copies from a temporary array
```

The list itself is `const T[]`. You cannot move out of it.
`vector<unique_ptr<T>>` cannot be initialized from
`{make_unique<T>(), ...}` in the obvious way — the
`initializer_list` wants to copy `unique_ptr`. Emplace in a
loop instead.

## `swap` and ADL

Containers swap in O(1) (pointers, not elements), except
`array` (O(N) element swaps). For your `T`, prefer a friend
`swap` that steals, so `std::sort` and `vector::swap` of
elements stay cheap.

## A minimal STL-friendly `T`

```cpp
struct Item {
    int id;
    std::string name;
    friend bool operator<(const Item& a, const Item& b) {
        return a.id < b.id;
    }
    friend bool operator==(const Item& a, const Item& b) {
        return a.id == b.id;
    }
};
```

Add a hash only if it goes in `unordered_*`:

```cpp
template<>
struct std::hash<Item> {
    std::size_t operator()(const Item& x) const {
        return std::hash<int>{}(x.id);
    }
};
```

If equality is on `id` only, hash **must** use `id` only.
Hashing `name` too would violate "equal ⇒ same hash".

Practice: [`examples/60_stl_friendly_type.cpp`](examples/60_stl_friendly_type.cpp).
Memory of the *container*: [`09_memory_reserve_pmr.md`](09_memory_reserve_pmr.md).
