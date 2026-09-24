# 12. Debug, sanitize, measure

Wrong iterators are undefined behavior. They do not always
crash. This chapter is how you **make them crash** in
development and how you **stop guessing** at `reserve`.

## Sanitizers (the default build for learning)

```bash
c++ -std=c++20 -O1 -g -fsanitize=address,undefined \
    examples/01_vector_timeline.cpp -o /tmp/v
```

AddressSanitizer catches use-after-free (dangling iterator
after realloc) and out-of-bounds `&v[v.size()]`.
UndefinedBehaviorSanitizer catches signed overflow
(`accumulate` with `0` on a big `vector<int>`) and misaligned
loads.

Run with `ASAN_OPTIONS=detect_leaks=1` if you care about
leaks. Containers should not leak; your `new` without
`unique_ptr` will.

## Debug STL (iterator checking)

libstdc++:

```bash
c++ -std=c++20 -D_GLIBCXX_DEBUG examples/foo.cpp
```

`vector::operator[]` becomes range-checked. Invalidated
iterators are trapped on use. Slow. Learning / test only.

libc++ (varies by version):

```bash
c++ -std=c++20 -D_LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_DEBUG ...
```

Do not ship these macros. They change ABI in some modes
(`_GLIBCXX_DEBUG` is a different `vector` layout).

## Measure before "optimizing" into a `list`

```cpp
#include <chrono>
auto t0 = std::chrono::steady_clock::now();
// ... work ...
auto t1 = std::chrono::steady_clock::now();
std::cerr << std::chrono::duration<double, std::milli>(t1 - t0).count() << " ms\n";
```

Compare `push_back` of 10⁷ ints **with and without**
`reserve`. That single experiment is worth more than a debate
about `deque`. Warm up once, then time several runs; do not
publish the first.

`vector` of pointers to fat objects is not "like a list".
It is a vector of pointers: extra hops, extra allocs. Measure.

## What the debugger shows

A `vector` is three pointers (`begin`, `end`, `capacity_end`)
in libstdc++. A `map` node is left/right/parent plus the
`pair<const K,V>`. Stepping into `std::sort` is noise; step
into **your comparator** if `sort` mis-orders.

`_GLIBCXX_DEBUG` + ASan together is a valid combo for
homework.

## A checklist when "the STL crashed"

1. Iterator / reference held across `push_back` / `rehash` /
   `insert` in the middle?
2. `end()` dereferenced?
3. `string_view` / `span` into a dead `string`?
4. `T` constructor threw, and you assumed strong guarantee
   with a throwing move?
5. Data race (see [`11`](11_threads_exceptions.md))?
6. `vector<bool>` proxy?
7. `sort` with `<=`?

Practice: [`examples/63_measure_reserve.cpp`](examples/63_measure_reserve.cpp)
prints the cost of forgetting `reserve`. Do not treat the
absolute milliseconds as portable — treat the **ratio**.
