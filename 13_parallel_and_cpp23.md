# 13. Parallel algorithms and C++23 — what "complete" still omits

C++20 is the floor of this guide. Two extensions show up in
real codebases. Neither changes the **cost contract**; they
change who runs the loop and which container is a sorted
vector in disguise.

## Execution policies (`<execution>`, C++17)

```
  std::sort(std::execution::seq,  v.begin(), v.end());  // same as std::sort
  std::sort(std::execution::par,  v.begin(), v.end());  // may use threads
  std::sort(std::execution::par_unseq, ...);            // + vectorized, overlapping
```

`for_each` / `transform` / `reduce` / `sort` / `count` have
overloads that take a policy **first**. The functor must be
safe to run on many threads (`par`) and must not assume
adjacent calls are sequential (`par_unseq` — no mutex in the
functor unless it is lock-free enough; usually: no mutex).

```
  par          split the range, threads, join
  par_unseq    plus SIMD; your F may be called as if unsequenced
               (do not lock a std::mutex in F)
```

**Apple libc++ often implements `par` as `seq`.** libstdc++
typically needs TBB (`-ltbb`) or a similar backend. If
`thread::hardware_concurrency()` is 8 and your `par` sort of
10 ints is not 8× faster, you do not have a real backend — you
have a sequential fallback. Measure
([`12_debug_measure.md`](12_debug_measure.md)).

`reduce` with `par` **reorders**. `accumulate` does not have a
parallel overload on purpose. Floating sums will differ.

Do not `par` a 100-element vector. The fork/join tax wins.

## C++23 pieces you will actually meet

| Feature | What it is | Instead of |
| --- | --- | --- |
| `std::ranges::to<vector>(view)` | materialize a view | `copy` + `back_inserter` |
| `std::flat_map` / `flat_set` | sorted `vector` of keys (and values) | `map` when N is small / lookup-heavy after build |
| `std::mdspan` | `span` with multiple extents | raw `T*` + `i*N+j` |
| `std::expected` | value or error, no exception | out-params / exceptions for *business* errors |
| `std::print` | formatted stdout | `cout <<` |
| `views::enumerate` / `zip` / `join_with` | more lazy adapters | index loops |

`flat_map` is the "I was about to keep a `vector<pair>` and
`lower_bound`" type **in the standard**. Insert is O(n).
Lookup is O(log n) binary search, cache-hot. Use it when you
build once (or rarely) and find often — the same critical
thinking as "sorted vector vs `map`" in
[`02_decision_guide.md`](02_decision_guide.md).

```
  map (tree)                 flat_map (two vectors or vector of pairs)
    [ 8 ]
   /     \                   keys:   [ 1 | 3 | 8 | 10 ]
 [3]     [10]                values: [ a | b | c |  d ]
 / \
[1] [?]
  node alloc, pointer chase     contiguous, insert shifts
```

This guide does not require a C++23 compiler. Read `flat_map`
as "named sorted vector". Implement it with `vector` +
`lower_bound` until your toolchain catches up
([`algorithms/binary_search.md`](algorithms/binary_search.md)).

## What this tutorial still is not

- How to write an allocator that is not PMR
- How `std::regex` works (not a container)
- Iostreams localization
- Coroutine generators as ranges (C++23 `generator`)
- GPU / execution offload

You are done with *this* map when you can pick a container
from a CRUD+cost sentence, name the iterator an algorithm
needs, and combine two indexes without leaving a ghost on
destroy. The method is [`14_read_a_problem.md`](14_read_a_problem.md).
[`exercises.md`](exercises.md) checks that (especially 13–25).
[`glossary.md`](glossary.md) is the dictionary.
Guarantees vs folklore: [`15_expert.md`](15_expert.md).

Capstone (several containers in one program):
[`examples/70_capstone_logs.cpp`](examples/70_capstone_logs.cpp).
