# 10. Predicates and function objects

An algorithm's "if" is a **callable** you pass in. The STL
ships the common ones in `<functional>` so you do not write
`return a < b;` for the hundredth time.

```
  sort(v.begin(), v.end(), greater<int>{});
  //                    ^^^^^^^^^^^^^^^^
  //                    a function object:  operator()(a,b) => a > b

  any_of(v.begin(), v.end(), [](int x){ return x < 0; });
  //                         ^^^^^^^^^^^^^^^^^^^^^^^^^^
  //                         a lambda: also a function object
```

## The named ones

| Object | `operator()` | Typical use |
| --- | --- | --- |
| `less<T>` / `less<>` | `a < b` | default `set`, `sort`, `priority_queue` |
| `greater<T>` | `a > b` | min-heap, descending sort |
| `equal_to<>` | `a == b` | `unordered_*` default eq |
| `plus<>` / `multiplies<>` | `a+b` / `a*b` | `accumulate`, `inner_product` |
| `identity` (C++20) | `a` | default projection |
| `logical_not<>` / `not_fn(f)` | `!f(x)` | invert a predicate |
| `compare_three_way` (C++20) | `<=>` | ranges, not the old algos |

`less<>` (void) is **transparent**: it accepts mixed types.
That is the heterogeneous `set<string, less<>>` trick in
[`05_hash_compare_lookup.md`](05_hash_compare_lookup.md).

## Lambdas beat `std::bind` for new code

```cpp
// C++11 bind soup
std::bind(std::less<int>{}, std::placeholders::_1, 10);

// lambda
[](int x){ return x < 10; };
```

`bind` still appears in old code. Prefer a lambda. Capture
`[&]` only for the duration of the algorithm call — do not
store a lambda that captured a local by reference in a
`std::function` that outlives the local.

## `std::function` is a type-erased **owner**

```
  function<bool(int)> p = [](int x){ return x > 0; };
  // heap-allocates if the lambda is larger than SSO
```

Use it when you must **store** a predicate of unknown type
(plugin, config). Do not use it as the type of a `sort`
comparator in a hot loop — the call is indirect. A template
parameter (`sort`'s `Compare`) inlines.

## Predicates must be pure enough

`remove_if` / `sort` / `set` may call your functor **any
number of times**, including on the same element twice.
Do not `cout` inside a comparator and call it a log. Do not
mutate the element (that breaks `sort`'s ordering). Counting
calls is OK; depending on the count for correctness is not.

`for_each` is the algorithm whose functor is *allowed* to have
side effects — that is its job.

## Projections (C++20)

```cpp
std::ranges::sort(people, std::less{}, &Person::age);
std::ranges::find(people, 21, &Person::age);
```

The projection runs **before** compare / equality. It is the
replacement for `bind(&Person::age, _1)`. See
[`06_ranges.md`](06_ranges.md).

Practice: [`examples/62_predicates.cpp`](examples/62_predicates.cpp).
`sort` comparator rules: [`algorithms/sorting.md`](algorithms/sorting.md).
