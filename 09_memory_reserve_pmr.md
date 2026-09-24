# 09. Memory: `reserve`, capacity, allocators, PMR

Big-O counts element operations. Wall-clock also counts
**how many times you go to the heap**. This chapter is that
second meter.

```
  vector  size=3 cap=4
  [ a b c _ ]     next push_back: no alloc

  size=4 cap=4
  [ a b c d ]     next push_back: alloc ~8, move 4, free old
```

## `reserve` vs `resize` vs `shrink_to_fit`

| Call | size | capacity | elements |
| --- | --- | --- | --- |
| `reserve(n)` | unchanged | ≥ n | none constructed |
| `resize(n)` | n | ≥ n | new slots default / fill constructed |
| `clear()` | 0 | **kept** | destroyed |
| `shrink_to_fit()` | unchanged | request size (non-binding) | none |
| `vector<T>().swap(v)` | 0 | 0 | old buffer released |

```
  reserve(1e6) then 1e6 push_back     ~1 allocation
  1e6 push_back with no reserve       ~20 allocations (doubling) plus moves
```

If you know `n`, `reserve`. If you need `n` default `T`s you
will index, `resize`. Indexing `[i]` after only `reserve` is
UB (`i >= size()`).

`string` is the same story plus SSO: tiny strings never hit
the heap. `map` / `unordered_map` allocate **per node**;
`reserve` on unordered sets the bucket count, not the nodes.

## The swap-to-release trick

```cpp
std::vector<int> v(1'000'000);
v.clear();                 // still holds ~4 MB
std::vector<int>().swap(v); // v is empty, capacity 0
// or  v = {};  often releases too (implementation)
```

## Allocators, in one picture

Every container is `vector<T, Alloc = std::allocator<T>>`.
The allocator is **where `new` goes**. You almost never write
one. You *do* switch to **PMR** when a function should allocate
from a stack buffer or an arena and then throw the whole
region away.

```
  default allocator          PMR monotonic arena
  each node:  operator new   bump a pointer in a buffer
  each delete: operator delete   (destroy objects, do not free one-by-one)
  map of 1e5 nodes:          map of 1e5 nodes:
    1e5 heap ops               1 arena, then reset
```

```cpp
#include <memory_resource>
char buf[64 * 1024];
std::pmr::monotonic_buffer_resource arena(buf, sizeof buf);
std::pmr::vector<int> v{&arena};          // note: pmr::vector, not vector
std::pmr::unordered_map<int, int> m{&arena};
```

`pmr::vector<int>` is **not** the same type as `vector<int>`.
You cannot pass it to a function that wants `vector<int>&`.
Use PMR at the **outer** boundary of a request / a parse, not
as a drop-in for every vector in the program.

When the buffer overflows, `monotonic_buffer_resource` allocates
upstream (the heap) in chunks. It never recycles individual
nodes — **destroy the arena**, do not `erase` hoping for reuse.

**Why not a custom `allocator<T>`?** The PMR type-erased
resource is the C++17 answer. Writing a typed allocator that
is still construct-compatible with `std::vector` is a weekend
project with sharp edges (rebind, propagate_on_move, …).

## Exception safety (the realloc rule)

`push_back` of a `T` with `noexcept` move: if the move of an
element throws, it must not — so the vector can offer the
**strong** guarantee (state unchanged on throw). If move can
throw, the library **copies** instead so the old buffer stays
valid. That is why [`08`](08_types_in_containers.md) cares
about `noexcept`.

Destructors of containers do not throw. A `T` destructor that
throws during `clear` is undefined territory — never do that.

Practice: [`examples/61_reserve_pmr.cpp`](examples/61_reserve_pmr.cpp).
Measure it: [`12_debug_measure.md`](12_debug_measure.md).
