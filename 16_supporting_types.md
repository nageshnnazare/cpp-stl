# 16. Supporting types you will put *in* containers

The STL is not only `vector` and `map`. These types show up
as **elements**, **keys**, and **return values**. They are
not sequences. Picking them wrong is a different class of
bug than picking `list` over `vector`.

## `std::pair` and `std::tuple`

```cpp
std::pair<int, std::string> p{1, "ada"};
auto [id, name] = p;                    // C++17 structured binding

std::tuple<int, int, int> t{1, 2, 3};
auto [a, b, c] = t;
std::get<0>(t);
```

**As a `set` / `map` key:** order is **lexicographic**. First
field is the primary sort.

```
  set<pair<int,int>>     {score, id}     begin() = lowest score, then id
  set<pair<int,int>>     {-score, id}    begin() = highest score
```

Leaderboards use the second line (**R79**). If you need a
different order, write a comparator; do not swap fields in
only half the inserts.

**As an unordered key:** no standard `hash<pair>`. Mix both
fields ([`05`](05_hash_compare_lookup.md), example 17).

`std::tie(a,b) = p` assigns to existing variables.
`std::ignore` skips a field. Prefer structured bindings in
C++17.

`map` values *are* `pair<const K, V>`. `structured binding`
`auto& [k,v]` on a map gives `const` key.

## `std::optional<T>`

"Zero or one T", with an engaged flag. Header `<optional>`.

```cpp
std::optional<int> parse(const std::string& s);
if (auto x = parse(t)) use(*x);
int y = parse(t).value_or(-1);
```

**Not a container of many.** Do not `vector<T>` of size 0–1
to mean maybe. **Not a view:** it owns `T` (or is empty).

In a `vector<optional<T>>`, empty slots are still slots
(`size` counts them). That is a valid "sparse but dense
index" trick; a `map` is the other choice.

`optional` of a reference is not a thing — use
`optional<reference_wrapper<T>>` or a pointer.

## `std::variant<Ts...>` and `std::visit`

Tagged union. Header `<variant>`. One of the types, not a
map.

```cpp
using Cell = std::variant<int, std::string>;
Cell c = 3;
std::visit([](auto&& x) { std::cout << x; }, c);
```

In a `vector<variant<...>>` you get a mixed column. In a
`map<string, variant<...>>` you get a JSON-ish object.
Visit is the Read. There is no iterator over "the int
alternative only" — `visit` or `get_if`.

`std::monostate` is the empty alternative (a variant that
may hold nothing).

## `std::reference_wrapper<T>`

A copyable reference. `vector<T&>` does not compile.
`vector<reference_wrapper<T>>` does; `v[i].get()` is `T&`.

`std::ref(x)` / `std::cref(x)` make them. Algorithms that
would copy `T` copy the wrapper (still aliases `x`).

If `x` dies, the wrapper is a dangling reference — same
rule as `span`.

## Smart pointers as elements

```
  vector<unique_ptr<T>>     owns T, not copyable as a vector via initializer_list
  vector<shared_ptr<T>>     shared ownership, extra atomics
  vector<T*>                does **not** own; you still delete
```

**R59:** braced `vector<unique_ptr<T>>{ make_unique<T>(), ... }`
tries to copy. `emplace_back(make_unique<T>(...))` in a loop.

`set<unique_ptr<T>>` needs a comparator on the pointed-to
value (or on the pointer address — usually wrong). Prefer
`set<T>` if T is movable, or `map<Id, unique_ptr<T>>`.

## `std::function` vs a template parameter

`std::function<bool(int)>` is a **type-erased** callable:
you can store it, put it in a vector, reassign it. It can
allocate. A template `Pred pred` on `find_if` inlines.

In a container of callbacks (`vector<function<void()>>`),
`function` is the right noun. As the predicate of a hot
`find_if`, a lambda / function object is cheaper.

## `std::chrono` as keys

`map<steady_clock::time_point, Job>` is a valid delay
queue **if** you do not need cancel-by-id (then add a
hash of iterators). Time points are comparable; they are
not hashable by default in older standards — check your
library or use `time_since_epoch().count()` as the key.

## `std::bitset` vs these

`bitset<N>` is a **container-like** of bits, N in the type.
It is not `optional<bool>`. See
[`containers/bitset.md`](containers/bitset.md).

## Mini picker

| You need | Type |
| --- | --- |
| Two named fields, sort by first | `pair` |
| N fields, get by index | `tuple` |
| Maybe one value | `optional<T>` |
| One of several types | `variant` |
| Vector of aliases to existing T | `reference_wrapper<T>` |
| Vector of owned polymorphic T | `unique_ptr<Base>` |
| Store a lambda in a member | `function<...>` (or a template) |

Practice with pairs-as-keys: [`examples/17_hash_transparent.cpp`](examples/17_hash_transparent.cpp).
Leaderboard `{-score,id}`: [`examples/40_recipe_leaderboard.cpp`](examples/40_recipe_leaderboard.cpp).
