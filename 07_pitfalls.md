# 07. Traps the compiler will not save you from

These are legal programs with the wrong cost, the wrong
lifetime, or a container that is not the container you think.

## `vector<bool>` is not a `vector`

It packs bits. `v[i]` is a **proxy**, not a `bool&`. You cannot
take its address. A range of `vector<bool>` is not a contiguous
range of `bool`. Use `vector<char>`, `vector<uint8_t>`, or
[`bitset`](containers/bitset.md).

## `remove` does not remove

```
  remove / unique / remove_if   compact keepers, return new logical end
  size() is unchanged until     v.erase(that, v.end())
```

C++20 members: `std::erase(v, value)`, `std::erase_if(v, pred)`
do both steps. Prefer them on `vector` / `string` / `deque`.
On `list`, the **member** `remove` relinks nodes (no compact).
On `map` / `set` / `unordered_*`, `erase(k)` is the real delete;
there is `std::erase_if` too (C++20).

## `unique` is adjacent-only

`[1,2,1]` after `unique` is still `[1,2,1]`. Sort first, or use
a `seen` set if you must keep first-occurrence order.

## `operator[]` on `map` / `unordered_map` inserts

`if (m[k] == 0)` is "insert 0 if missing, then test". Membership
is `find` / `contains`. Counting is the one place `[]` is the
right Create-or-Update.

## `accumulate` type is the type of `0`

```cpp
std::accumulate(v.begin(), v.end(), 0);     // int, UB on overflow
std::accumulate(v.begin(), v.end(), 0LL);   // long long
```

`reduce` may reorder. `string` fold: `accumulate` with
`std::string{}` as init, not `""` as `const char*` (wrong type,
won't concatenate).

## Iterator death

```
  auto it = v.begin();
  v.push_back(x);          // maybe realloc  => it dangling
  for (auto x : v) v.push_back(x);   // infinite / UB
```

`erase(it)` on `vector` invalidates `it` and everything after;
the return value is the next live iterator:

```cpp
for (auto it = v.begin(); it != v.end(); )
    if (pred(*it)) it = v.erase(it);
    else ++it;
```

On `unordered_map`, that loop is O(n) average and correct
(`erase(it)` returns the next). On `vector` it is O(n²). Use
erase-remove.

Holding a **reference** `auto& x = v[0]; v.push_back(...);`
is the same dangling as an iterator.

`string_view` / `span` into a string that then `+=` enough to
leave SSO or realloc: dangling. Views do not keep the owner
alive.

## `std::lower_bound` on a `set`

Compiles. O(n) hops. Member `s.lower_bound(x)` is O(log n).
See [`01_iterators.md`](01_iterators.md).

## `priority_queue` has no iterators and no erase

You cannot walk it or delete 7. Need those: `multiset`.
Need only the extreme: the heap. See
[`containers/priority_queue.md`](containers/priority_queue.md).

## `sort` comparator `<=`

Undefined. Use `<`. Equal keys: `!comp(a,b) && !comp(b,a)`.
See [`05_hash_compare_lookup.md`](05_hash_compare_lookup.md).

## Hashing floats / pointers as keys

`-0.0` and `0.0` compare equal in `==` but may hash differently
depending on the implementation. Pointers as keys identify
**addresses**, not objects (a moved-to object has a new address).

## `end()` is not the last element

`*v.end()` is UB. Last is `v.back()` or `*prev(end())` (check
non-empty). Algorithms return `end()` for "not found".

## Adapters hide a real container

`stack` / `queue` / `priority_queue` are not ranges. If you
needed `for (x : s)`, you wanted a `vector` and `back()`.

## SSO and small-object surprises

A `string` of a few chars lives inside the object. Taking
`data()` / a `string_view`, then `push_back` until it spills
to the heap, invalidates the view. Treat short strings the
same as long ones when you store views.

## `deque` `data()` does not exist

Random-access ≠ contiguous. You cannot pass `&d[0]` as a
buffer of `d.size()` elements to C. Copy to a `vector` or
use `span` on a real array.

## Related reading

Invalidation table: [README](README.md#iterator-invalidation-the-silent-bug).
Erase-remove: [`algorithms/modifying.md`](algorithms/modifying.md).
Views dangling: [`06_ranges.md`](06_ranges.md).
The same facts as numbered laws: [`rules.md`](rules.md) R9–R18, R40–R47.
Guarantees: [`15_expert.md`](15_expert.md).
