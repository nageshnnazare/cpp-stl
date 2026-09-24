# 06. C++20 ranges: the same costs, less `begin`/`end`

A range is a **pair of iterators (or iterator + sentinel) with
a name**. `std::ranges::sort(v)` is `std::sort(v.begin(), v.end())`
plus extra checks. Views are **lazy iterator adapters** that do
not own elements.

```
  C++17                         C++20
  sort(v.begin(), v.end())      ranges::sort(v)
  find(v.begin(), v.end(), x)   ranges::find(v, x)
  copy(a, b, back_inserter(c))  ranges::copy(a, back_inserter(c))

  views (lazy):
  v | views::filter(even) | views::transform(square) | views::take(5)
        no extra vector unless you materialize
```

Header: `<algorithm>` for `std::ranges::*` algorithms,
`<ranges>` for views.

## Algorithms on a range

```
  std::ranges::sort(v);
  std::ranges::sort(v, std::ranges::greater{});
  std::ranges::sort(people, {}, &Person::age);   // projection
```

A **projection** is a callable applied before compare / predicate.
`sort(people, {}, &Person::age)` is "sort by age" without a
lambda that returns `a.age < b.age`. The `{}` is the comparator
(`ranges::less`).

`ranges::find(v, x)` returns a **dangling-aware** iterator:
if you pass a temporary container, the return type is
`dangling` and you cannot dereference it. That is a compile-time
footgun the iterator-pair API did not have.

```
  auto it = std::ranges::find(std::vector{1,2,3}, 2);  // dangling
  // *it does not compile
```

## Views are not containers

```
  auto odds = v | std::views::filter([](int x){ return x % 2; });
  // odds does not hold ints. it holds (a copy of) the filter
  // and iterators into v.

  v.push_back(1);     // may realloc  => odds is garbage
```

```
  vector  [ 1 2 3 4 5 ]
              |  filter odd
              v
  view walks: 1, 3, 5     computed when you iterate, not before
```

Materialize when you need a container:

```cpp
std::vector<int> got;
std::ranges::copy(odds, std::back_inserter(got));
// or, C++23: auto got = std::ranges::to<std::vector>(odds);
```

**Why a view, not `copy_if` into a vector?** When the next step
is `find` / `accumulate` / a range-for and you would throw the
vector away. **Why `copy_if`?** When you must store, shuffle, or
return ownership.

## Common views

| View | Does | Extra space |
| --- | --- | --- |
| `views::all` | wrap a container | O(1) |
| `views::filter` | skip if pred false | O(1) |
| `views::transform` | map 1:1 | O(1) |
| `views::take` / `drop` | prefix / suffix skip | O(1) |
| `views::take_while` / `drop_while` | pred | O(1) |
| `views::reverse` | backward | needs bidirectional |
| `views::keys` / `values` | `map` pairs | O(1) |
| `views::enumerate` (C++23) | (i, elem) | O(1) |
| `views::iota` | infinite or bounded ints | O(1) |
| `views::split` / `lazy_split` | pieces | O(1) |

Iterator category **can drop**. `filter` on a `vector` is
bidirectional, not random-access: you cannot `sort` a filter
view (there is no `operator+`). Pipe into a `vector` first.

```
  vector          contiguous
    | filter
    v
  filter_view     bidirectional (you skip, so i+n is not O(1))
    | transform
    v
  still not sort-able
```

## Sentinels

`end` does not have to be the same type as `begin`. A sentinel
answers "are we done?" without being incrementable.

```
  iota(0) | take_while(x < 10)     end is "pred failed", not a counted iterator
```

You rarely name the sentinel. You *do* stop writing algorithms
that assume `last - first` unless the range is sized /
random-access (`ranges::size`, `ranges::distance`).

## What this guide still writes as iterator pairs

Complexity and invalidation are easier to see with `begin`/`end`.
Use `ranges::` at **call sites** (`sort`, `find`, `any_of`). Use
views when they replace a temporary vector you were going to
throw away. If a view would outlive the container, you wanted a
container.

Practice: [`examples/18_ranges.cpp`](examples/18_ranges.cpp).
Iterator category recap: [`01_iterators.md`](01_iterators.md).
Algorithm × category matrix: [`algorithms/iterator_requirements.md`](algorithms/iterator_requirements.md).
