# 15. Expert invariants: what the standard actually promises

This page is for the reader who can pick a container in thirty
seconds and now needs the **guarantees** — so a review comment
or a sanitizer report is about the code, not folklore.

Beginners: skip until
[`tracks.md`](tracks.md) says so. Laws:
[`rules.md`](rules.md). Threads/exceptions intro:
[`11_threads_exceptions.md`](11_threads_exceptions.md).

---

## Complexity: guaranteed vs "typical"

The standard states **complexity requirements**. Libraries may
be faster; they may not be asymptotically slower (except where
the standard says "amortized" or "average").

| Claim people make | What is actually true |
| --- | --- |
| `unordered_map::find` is O(1) | **Average** O(1). Worst O(n). Required: expected constant if the hash is good |
| `vector::push_back` is O(1) | **Amortized** O(1). A single call is O(n) on realloc. Capacity growth is geometric (typical 2×; libc++ often 2×) |
| `list::size` is O(1) | Yes **since C++11**. Pre-C++11 it could be O(n) |
| `deque::push_front` is O(1) | Amortized O(1); an end chunk may allocate |
| `map::insert` is O(log n) | Comparisons O(log n). Pointer hops O(log n). Not cache-O(log n) |
| `std::sort` is O(n log n) | C++11: O(n log n) **comparisons**. Introsort typical (quick + heap fallback) |
| `stable_sort` extra memory | Attempts O(n) extra; may fall back to slower in-place |
| `std::lower_bound` is O(log n) | O(log n) **comparisons**. Iterator advances are O(n) on bidirectional |

Do not write "O(1) hash" in a public SLA without stating
average, and without a hash that is not attacker-controlled.

---

## Iterator and reference stability (the contract)

Folklore is "node containers keep pointers valid." The
standard is narrower. Use this table in reviews.

| Container | Insert | Erase |
| --- | --- | --- |
| `vector` | Realloc: **all** it/ref/ptr die. No realloc: refs/ptrs stay; iterators at/after insert die | At and after die |
| `string` | Same family as vector (SSO: growth may leave the small buffer — treat like realloc) | Same |
| `deque` | **References** to elements stay valid on **end** insert/erase. **Iterators** are invalidated on insert (all) and on middle erase (all). End erase: only iterators to the erased elements (treat as fragile in portable code) | See left |
| `list` / `forward_list` | Other iterators **and references** stay | Only the erased |
| `set` / `map` (+ multi) | Other iterators and references stay | Only the erased |
| `unordered_*` | Rehash: **all iterators die**. **Pointers and references to elements stay valid** across insert/rehash (standard). Do **not** store unordered **iterators** across a potential rehash | `erase(it)`: only that iterator. C++11 returns the next |

**Portable LRU:** store `list::iterator`, not
`unordered_map::iterator`, across inserts into the hash.

**Portable "pointer to vector element":** `reserve` first and
never `insert`/`erase` in the middle, or store an **index**.

---

## Exception safety (what `push_back` really guarantees)

| Operation | Typical guarantee | Condition |
| --- | --- | --- |
| `size`, `empty`, `begin`, swap of containers | No-throw | — |
| `vector::push_back` / `emplace_back` | **Strong** (state unchanged if throw) | If `T` move is `noexcept`; otherwise may copy, still tries strong |
| `vector` realloc with throwing move | Implementation must not use that move if it would break strong; **copies** instead | **R57** |
| `map::insert` | Strong for a single-node insert if `T` ctor throws | Tree unchanged |
| `erase` | No-throw if `T` destructor does not throw | **Never throw from a destructor** |
| `sort` | Basic: throw ⇒ valid but unspecified order | Comparator must not throw if you need more |

If `T`'s destructor throws, every container is undefined
territory. `noexcept` on destructors is not optional taste.

---

## Allocators and PMR (what you can skip, what you cannot)

Default: `std::allocator<T>` → global `new`/`delete`.

`std::pmr::vector<T>` is still a vector. Same invalidation,
same threads, same erase rules. The polymorphic resource
sits behind a pointer:

```
  pmr::vector  -->  memory_resource::allocate
                       monotonic_buffer_resource  (arena, no per-free)
                       unsynchronized_pool_resource
                       new_delete_resource
```

**Expert traps:**

- A `pmr::vector` and a `std::vector` are **different types**.
  You cannot pass one where the other is required without a
  copy.
- `monotonic_buffer_resource` never recycles individual
  objects. Fine for build-and-throw-away. Terrible as a
  long-lived cache of many insert/erase cycles.
- Sharing one resource across threads: the resource's
  synchronization + the container mutex. PMR is not a lock.
- Do not let a `pmr::string` outlive its arena.

Full custom allocators (`allocator_traits`, fancy pointers)
are real and almost never the first fix. Measure, then PMR
arena, then a custom allocator.

---

## Implementation notes (libstdc++ / libc++ / MSVC)

Useful, **not** portable contracts. Do not write code that
requires them without an `#ifdef` and a test.

| Topic | Typical |
| --- | --- |
| `map`/`set` | Red-black tree |
| `unordered_*` | Separate chaining; node per element (so element refs often survive rehash — still do not store iterators) |
| `deque` chunk | Often ~512 bytes of `T`s (libstdc++: `512 / sizeof(T)` or 1) |
| `string` SSO | libstdc++ ~15 bytes on 64-bit; libc++ 22/23 on 64-bit — **not** a portable capacity |
| `vector` growth | Factor 2 common |
| Hash of integers | Seeded / perturbed in modern libraries; still write a real mixer for pairs |
| `execution::par` | libstdc++ often needs TBB; Apple libc++ often **seq fallback** |

Swiss tables (absl `flat_hash_map`) are **not**
`std::unordered_map`: they move elements on rehash, so
**pointers to elements die**. Faster. Different contract.
`std::unordered_map` keeps element pointers valid across
rehash; that is why it is node-based and slower. If you
need pointer stability, stay on `std::` or say so.

C++23 `flat_map` / `flat_set`: sorted vector. Insert O(n).
Lookup cache-hot O(log n). Pointers die on insert. Use when
build-once / find-often.

C++26 watch list (not required here): `inplace_vector`
(fixed cap, no heap), `hive`/`colony` (stable refs, O(1)
erase, no index).

---

## Strict weak ordering and hash — the UB people ship

`sort` / `set` comparator:

```
  comp(x,x) == false
  comp(a,b) implies !comp(b,a)
  transitive <  and  transitive equivalence
```

Violations: infinite loops, lost nodes, ASan silence,
"works on my machine." `<=`, comparing only one field of a
struct that `==` uses two, and `NaN` are the usual three.

Hash:

```
  a == b  ⇒  hash(a) == hash(b)     required
  hash(a) == hash(b) ⇏ a == b      collisions OK
```

If `==` is on `id` and hash is on `name`, `unordered_set`
will store "duplicates" that `==` says are the same, or miss
finds. [`05_hash_compare_lookup.md`](05_hash_compare_lookup.md).

---

## When to leave the STL

The STL is complete for **sequential** in-memory data with
the operations above. Leave it (compose, or another library)
when you need:

| Need | Not in `std::` | Direction |
| --- | --- | --- |
| Concurrent hashmap / MPMC queue | no | TBB, folly, junction, lock + `deque` |
| O(1) erase **and** stable index **and** dense | no | slot map / `hive` |
| Small-buffer vector (inline storage) | no until `inplace_vector` | `boost::container::small_vector`, LLVM SmallVector |
| Order-statistic tree (k-th in a set) | no | `__gnu_pbds`, policy data, Fenwick, treap |
| Persistent / immutable map | no | immer, structural sharing |
| GPU / SIMD containers | no | Thrust, Eigen, your SOA |
| Intrusive lists (node is inside T) | no | Boost.Intrusive, roll a pointer |
| Perfect hashing / frozen maps | no | frozen, gperf |

**Compose first.** LRU, leaderboard, delay queue, randomized
set — this repo's recipes — are still 100% STL. "Leave"
means the *operation does not exist*, not "I am bored of
`vector`."

---

## Debug vs release

| Tool | What it catches |
| --- | --- |
| `-D_GLIBCXX_DEBUG` (libstdc++) | Iterator misuse, `[]` bounds on debug containers |
| libc++ `_LIBCPP_DEBUG` / hardened mode | Similar, version-dependent |
| ASan / UBSan | Use-after-free, OOB, misaligned — **not** "wrong Big-O" |
| `-Wpedantic -Wall -Wextra` | Missing includes, narrowing |

Debug STL is ABI-incompatible with release STL. Do not mix
object files. Measure **release** with `reserve` on; debug
iterators can make `vector` look like `list`.

[`12_debug_measure.md`](12_debug_measure.md).

---

## Parallel algorithms, again, as an expert

`std::execution::par` on `sort`/`for_each`/`reduce`:

- The range is exclusively owned for the call.
- `F` is thread-safe under `par`; under `par_unseq` it must
  tolerate unsequenced / overlapping execution (no
  `std::mutex` in `F`).
- `reduce` reorders. Sum of `float` will differ.
- No backend ⇒ sequential. Measure or you shipped a lie.

[`13_parallel_and_cpp23.md`](13_parallel_and_cpp23.md).

---

## A review checklist (expert)

```
  [ ] Container matches the hot op (R1–R4)
  [ ] No map[] for membership (R18)
  [ ] No range-for erase (R40)
  [ ] No stored vector iterator across push_back (R9)
  [ ] No unordered iterator across insert (R26, R43)
  [ ] Destroy updates every index (R4, R71–R74)
  [ ] Comparator is strict; hash agrees with == (R22–R23)
  [ ] T move is noexcept if vector grows (R57)
  [ ] Views have owners (R65, R77)
  [ ] Shared container has a mutex story (R60)
  [ ] Complexity comments say amortized/average when required
```

If this list is muscle memory, you are done with the tutorial
as a *reference*. Keep [`cheat_sheet.md`](cheat_sheet.md) in
the repo search path and open
[`problem_catalog.md`](problem_catalog.md) when the story
is familiar.
