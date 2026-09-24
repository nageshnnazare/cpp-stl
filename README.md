# C++ STL: the rulebook

This directory is a from-scratch workshop **and** a lookup
bible. You learn each container by **the problem it exists to
solve**, then you keep the decision pages open when a new
problem shows up.

**To pick a container given a problem:** underline
**ASK / ARRIVE / EXTRA** in the sentence
([method](14_read_a_problem.md)). Missing verbs are *not*
requirements — do not pay for delete, order, or “by id”
unless they are written. Then the
[picker](02_decision_guide.md). Familiar story →
[problem catalog](problem_catalog.md). Quote
[rules R1–R80](rules.md). One page:
[cheat sheet](cheat_sheet.md).

**To learn it:** [beginner first hour](00_beginner.md), then
[tracks](tracks.md) (weekend / interview / expert).

Every chapter has:

- an ASCII picture of the memory layout
- Create / Read / Update / Delete (CRUD) on that type
- a **why this, not that** argument against the next-best container
- a compilable example under [`examples/`](examples/)
- related interview / contest problems that are the same idea in costume

Target: **C++20**. Compile anything with:

```bash
cd cpp-stl
make
./build/01_vector_timeline
./build/80_beginner_hello
```

## How to walk this

```
  00b beginner hour        never used vector? start here
  00  mental model         iterators are the language algorithms speak
           |
           v
  14  read a problem       ASK / ARRIVE / EXTRA → container   ← rulebook
  02  decision guide       picker, erase, threads
  rules / catalog / sheet  quote, look up a shape, print
           |
           v
  01–07 pick + mechanics     adapters, emplace, hash, ranges, traps
           |
           v
  08–16    T, memory, threads, debug, C++23, expert, pair/optional
           |
           +---> containers/*     one type, many jobs
           |
           +---> algorithms/*     same loop, written once
           |
           v
  recipes/*  then capstone.md + exercises.md (identify 13–25)
```

| Order | File | What you can do after |
| --- | --- | --- |
| — | [`tracks.md`](tracks.md) | Pick weekend / interview / expert |
| — | [`cheat_sheet.md`](cheat_sheet.md) | One-page picker |
| — | [`rules.md`](rules.md) | Quote R1–R80 |
| — | [`problem_catalog.md`](problem_catalog.md) | Shape → file |
| 0b | [`00_beginner.md`](00_beginner.md) | Compile a `vector` and a count map |
| 0 | [`00_mental_model.md`](00_mental_model.md) | Draw data, name the cost |
| 1 | [`01_iterators.md`](01_iterators.md) | Know why `sort` rejects `list` |
| 2 | [`02_decision_guide.md`](02_decision_guide.md) | Pick a container; thread-safety; legal erase-while-walk |
| 3 | [`03_iterator_adapters.md`](03_iterator_adapters.md) | `copy` into a growing container |
| 4 | [`04_insert_emplace_extract.md`](04_insert_emplace_extract.md) | try_emplace, rename a key |
| 5 | [`05_hash_compare_lookup.md`](05_hash_compare_lookup.md) | Custom hash, transparent find |
| 6 | [`06_ranges.md`](06_ranges.md) | Pipelines without extra vectors |
| 7 | [`07_pitfalls.md`](07_pitfalls.md) | Name the footgun before it fires |
| 8 | [`08_types_in_containers.md`](08_types_in_containers.md) | Make `T` sortable, hashable, `noexcept` movable |
| 9 | [`09_memory_reserve_pmr.md`](09_memory_reserve_pmr.md) | `reserve`, arenas |
| 10 | [`10_predicates.md`](10_predicates.md) | `greater`, `not_fn`, projections |
| 11 | [`11_threads_exceptions.md`](11_threads_exceptions.md) | What you may share; what `push_back` guarantees |
| 12 | [`12_debug_measure.md`](12_debug_measure.md) | ASan, debug STL, time `reserve` |
| 13 | [`13_parallel_and_cpp23.md`](13_parallel_and_cpp23.md) | `par`, `flat_map` as sorted vector |
| 14 | [`14_read_a_problem.md`](14_read_a_problem.md) | Decode ASK / ARRIVE / EXTRA; blanks are n/a |
| 15 | [`15_expert.md`](15_expert.md) | Guarantees vs folklore; when to leave `std::` |
| 16 | [`16_supporting_types.md`](16_supporting_types.md) | `pair`, `optional`, `variant`, smart pointers |
| 17 | [`containers/`](containers/) | Use each type on a real job |
| 18 | [`algorithms/`](algorithms/) | Replace hand-rolled loops |
| 19 | [`recipes/`](recipes/) | Combine types without fighting them |
| 20 | [`capstone.md`](capstone.md) | One program, four containers |
| 21 | [`exercises.md`](exercises.md) | 25 questions including identify-the-container |
| — | [`glossary.md`](glossary.md) | The words, fixed |

You are done with this map when you can (1) pick a container from
a CRUD+cost sentence, (2) name the iterator an algorithm needs,
(3) combine two indexes without leaving a ghost on destroy, and
(4) say whether `T`'s move is `noexcept` and why `vector` cares.
The exercises check 1–4. The capstone is (3) in one file.
Identify drills 13–25 are (1) under time pressure.

---

## Container map

```
                         +------------------+
                         |   Do I need      |
                         |   order by key?  |
                         +--------+---------+
                                  |
              no                  |                   yes
              |                   |                    |
              v                   |                    v
     +--------+--------+          |          +---------+----------+
     | Need O(1) index |          |          | Need sorted        |
     | or a stack/q?   |          |          | iteration?         |
     +--------+--------+          |          +---------+----------+
              |                   |                    |
     yes      |      hash lookup  |           yes      |    no, just
              |      by key       |                    |    average O(1)
              v                   v                    v
     SEQUENCE / ADAPTER     UNORDERED_*          set / map
     vector deque list      unordered_map        (tree, O(log n))
     stack queue pq         unordered_set
```

| Family | Types | Order | Typical find | Typical insert |
| --- | --- | --- | --- | --- |
| Sequence | `array` `vector` `deque` `list` `forward_list` `string` | insertion / index | O(n) | see table below |
| Ordered | `set` `multiset` `map` `multimap` | sorted by key | O(log n) | O(log n) |
| Hashed | `unordered_set` `unordered_multiset` `unordered_map` `unordered_multimap` | none | avg O(1) | avg O(1) |
| Adapter | `stack` `queue` `priority_queue` | restricted | n/a | see adapters |
| View | `span` `string_view` | borrowed | O(1) index / `find` | no ownership |
| Bits | `bitset<N>` | bit index | O(1) test | N compile-time, no heap |

Space is **O(n)** plus allocator slack unless noted. Trees and hashes
pay extra per node (pointers / bucket array). `array` and `span` have
no heap (span owns nothing).

---

## CRUD and complexity: sequence containers

Legend: **A** = amortized. **I** = at a known iterator. **n** = size.
`string` is listed because you will use it as a sequence every day.

### `std::vector<T>` — growable contiguous array

```
  data --> [ a | b | c | d | _ | _ | _ ]
            0   1   2   3         capacity
            <----- size ---->
```

| CRUD | Methods | Time | Space extra |
| --- | --- | --- | --- |
| C | `vector()`, `vector(n)`, `vector(n, v)`, `{...}`, `assign`, `insert`, `push_back`, `emplace_back` | construct O(n); `push_back` **A O(1)**, worst O(n) on realloc | capacity ≥ size |
| R | `[]`, `at`, `front`, `back`, `data`, `begin`/`end`, `size` | O(1) | — |
| U | `v[i]=`, `assign`, `swap`, `std::fill` | O(1) per element | — |
| D | `pop_back`, `erase`, `clear` | `pop_back` O(1); `erase` middle O(n) | capacity kept on `clear` |

Search unsorted: O(n). Binary search if sorted: O(log n).
Iterators / references **die on realloc**; `erase` invalidates at and
after the hole.

**Choose vector** when you want cache-friendly random access and
almost all growth is at the back. **Not list**: a `list` node loses
to a `vector` scan until n is huge and you splice constantly.

### `std::deque<T>` — chunked array, cheap ends

```
  map -->  [ chunk0 ] [ chunk1 ] [ chunk2 ]
              |           |           |
           [ _ _ a b]  [ c d e f]  [ g _ _ _ ]
                ^                         ^
              front                     back
```

| CRUD | Methods | Time | Space extra |
| --- | --- | --- | --- |
| C | same family as vector + `push_front` / `emplace_front` | ends **A O(1)**; middle O(n) | chunk map + slack in end chunks |
| R | `[]`, `at`, `front`, `back` | O(1) (slower constant than vector) | — |
| U | `d[i]=` | O(1) | — |
| D | `pop_front`, `pop_back`, `erase`, `clear` | ends O(1); middle O(n) | — |

Pointers to elements stay valid on end insert/erase (not middle).
Iterators are **not** contiguous (`data()` does not exist).

**Choose deque** for a double-ended queue, a sliding window, or a
BFS that grows both ways. **Not vector** if you `insert(begin)` in a
loop. **Not list** if you still want `d[i]`.

### `std::list<T>` — doubly linked list

```
  head <-> [p|a|n] <-> [p|b|n] <-> [p|c|n] <-> tail
```

| CRUD | Methods | Time | Space extra |
| --- | --- | --- | --- |
| C | `push_front/back`, `insert(it)`, `emplace`, `splice` | O(1) **I**; construct O(n) | 2 pointers + alloc per node |
| R | `front`, `back`; walk `begin`..`end` | O(1) ends; O(n) to the k-th | — |
| U | `*it =` | O(1) **I** | — |
| D | `pop_front/back`, `erase(it)`, `remove`, `clear` | O(1) **I**; `remove` by value O(n) | — |

`size()` is O(1) since C++11. No `operator[]`. `splice` moves nodes
in O(1) / O(k) without copying `T`.

**Choose list** only when you **hold iterators** and splice/erase
them without invalidating others (LRU, playlist). **Not** for
"I insert in the middle a lot" if the iterator is not already there:
finding the spot is O(n) and then you lost to `vector`.

### `std::forward_list<T>` — singly linked list

```
  head -> [a|n] -> [b|n] -> [c|n] -> null
```

| CRUD | Methods | Time | Space extra |
| --- | --- | --- | --- |
| C | `insert_after`, `emplace_after`, `push_front` | O(1) after a known it | 1 pointer per node |
| R | `front`; walk only forward | O(1) front; no `size()` | — |
| U | `*it =` | O(1) | — |
| D | `erase_after`, `pop_front`, `remove` | O(1) after a known it | — |

**Choose forward_list** when you are memory-tight and only walk
forward (intrusive-style chains, simple separate chaining). Almost
always `vector` is faster.

### `std::array<T, N>` — fixed buffer, no heap

```
  stack (or static):  [ 0 | 1 | 2 | 3 ]     N is a compile-time constant
```

| CRUD | Methods | Time | Space extra |
| --- | --- | --- | --- |
| C | `array<T,N>{}`, `= {..}`. No `push_back` | O(N) value-init | 0 heap |
| R | `[]`, `at`, `front`, `back`, `data` | O(1) | — |
| U | `a[i]=`, `fill`, `swap` | O(1) / O(N) | — |
| D | cannot shrink; `fill` to a sentinel | — | — |

**Choose array** for a compile-time size (RGB, 256-bin histogram,
small lookup). **Not vector** if N is known and you want no alloc.

### `std::string` — `vector<char>` with text operations

Same asymptotic story as `vector<char>`, plus SSO (small-string
optimization): short strings live **inside the object**.

| Extra vs vector | Methods | Time |
| --- | --- | --- |
| C | `+`, `append`, `insert`, `push_back` | A O(1) at end; O(n) middle |
| R | `[]`, `substr`, `find`, `compare`, `starts_with` | `find` O(n·m) typical |
| U | `replace`, `[]=` | O(n) if it shifts |
| D | `erase`, `pop_back`, `clear` | same as vector |

---

## CRUD and complexity: ordered associative

Implemented as a **balanced binary tree** (libstdc++/libc++: red-black).
Keys are sorted. Iterator walk is in-order.

```
            [ 8 ]
           /     \
        [ 3 ]   [ 10 ]
        /   \       \
     [ 1 ] [ 6 ]   [ 14 ]
```

### `std::set<K>` / `std::multiset<K>`

| CRUD | Methods | Time | Notes |
| --- | --- | --- | --- |
| C | `insert`, `emplace`, `insert(hint, ...)` | O(log n); hint can be A O(1) | `set` rejects duplicates |
| R | `find`, `count`, `contains`, `lower_bound`, `upper_bound`, `equal_range`, `begin` (min), `--end` (max) | O(log n) | walk is O(n) sorted |
| U | erase + insert; `*it` is `const K` | O(log n) | cannot mutate key in place |
| D | `erase(key)`, `erase(it)`, `clear` | O(log n) / O(1) **I** / O(n) | — |

`count` on `set` is 0 or 1. On `multiset` it is O(log n + hits).

### `std::map<K,V>` / `std::multimap<K,V>`

Same tree costs. `map[k]` **default-constructs V** if `k` is new
(that is a **Create**, not a Read). Prefer `find` / `try_emplace`
when you must not insert.

| CRUD | Methods | Time |
| --- | --- | --- |
| C | `insert`, `emplace`, `try_emplace`, `insert_or_assign`, `m[k]=` | O(log n) |
| R | `find`, `contains`, `at` (throws), `lower_bound` | O(log n) |
| U | `m[k]=`, `it->second =` | O(log n) to find; O(1) after |
| D | `erase` | O(log n) / O(1) **I** |

**Choose set/map** when you need **sorted keys**, range queries
(`lower_bound`..`upper_bound`), or a predecessor/successor. **Not
unordered_*** if you iterate in key order or need closest-key.

---

## CRUD and complexity: unordered (hash tables)

```
  buckets:  0    1    2    3    4    5
            |    |    |    |    |    |
           []   [7]  []  [3][19] []  [11]
                      hash(x) % B
```

Average O(1) assumes a decent hash and load factor. **Worst case
O(n)** if everything lands in one bucket (or you attack a weak hash).

### `std::unordered_set<K>` / `unordered_multiset<K>`
### `std::unordered_map<K,V>` / `unordered_multimap<K,V>`

| CRUD | Methods | Avg time | Worst | Space extra |
| --- | --- | --- | --- | --- |
| C | `insert`, `emplace`, `try_emplace`, `m[k]=` | O(1) | O(n) | buckets + nodes |
| R | `find`, `contains`, `count`, `at` | O(1) | O(n) | — |
| U | `it->second =` (map) | O(1) | — | keys are const |
| D | `erase(key)`, `erase(it)` | O(1) | O(n) | — |

`rehash` / growth is O(n). `reserve(n)` before a known bulk insert.
No `lower_bound`. Iteration order is **not** insertion order and
**not** sorted.

**Choose unordered_map** for "id → value" and Two-Sum-style
lookups. **Not map** unless you need order or a custom key that is
painful to hash.

---

## CRUD and complexity: adapters

Adapters are **not** containers. They hide a real one and expose a
narrow API. You cannot iterate a `priority_queue`.

| Adapter | Default inner | CRUD surface | Time | Space |
| --- | --- | --- | --- | --- |
| `stack<T>` | `deque<T>` | C: `push`/`emplace`  R: `top`  U: assign into `top()`  D: `pop` | all O(1) | inner |
| `queue<T>` | `deque<T>` | C: `push`/`emplace`  R: `front`/`back`  D: `pop` | all O(1) | inner |
| `priority_queue<T>` | `vector<T>` + heap | C: `push`/`emplace`  R: `top`  D: `pop` | push/pop O(log n), top O(1) | inner |

```
  stack (LIFO)          queue (FIFO)           priority_queue (heap)
  +-------+             front --> [a b c]             9
  |   c   |  top                 back --^           /   \
  |   b   |                                       7     8
  |   a   |                                      / \
  +-------+                                    1   3
```

**Choose stack** for matching / DFS / undo. **queue** for BFS.
**priority_queue** for "always the extreme" (Dijkstra, top-k, merge-k).
If you need the **min and** random erase, use `multiset` instead of
a heap.

`priority_queue` is a max-heap by default. Min-heap:

```cpp
std::priority_queue<int, std::vector<int>, std::greater<int>> min_h;
```

---

## `std::span<T>` (C++20) — non-owning view

```
  vector / array / C array     span is a (pointer, length)
  [ a b c d e ]   <--------    [ptr ------> +  len=5]
```

| CRUD | Methods | Time |
| --- | --- | --- |
| C | `span(c)`, `span(p, n)`, `subspan` | O(1) |
| R | `[]`, `front`, `back`, `data` | O(1) |
| U | `s[i]=` if `T` is not const | O(1) |
| D | cannot; you do not own the memory | — |

Use `span` as a **function parameter** so one function accepts
`vector`, `array`, and C arrays. It is not a place to store data.

### `std::string_view` (C++17) — span for characters

| CRUD | Methods | Time |
| --- | --- | --- |
| C | `string_view(s)`, `{p,n}`, `substr` | O(1); **no copy** of chars |
| R | `[]`, `find`, `starts_with`, `compare` | `find` typically O(n·m) |
| U | cannot change characters | — |
| D | `remove_prefix` shrinks the view only | O(1) |

Dies if the owner reallocates or is destroyed. Store `string` in
long-lived objects. See [`containers/string_view.md`](containers/string_view.md).

### `std::bitset<N>` — packed flags, N in the type

| CRUD | Methods | Time |
| --- | --- | --- |
| C | `bitset<N>{}`, `{0b...}`, from integer / string | O(N/word) |
| R | `test`, `[]`, `count`, `any`/`none`/`all`, `to_ulong` | bit O(1); `count` O(N/word) |
| U | `set`/`reset`/`flip`, `&=` `\|=` `^=` `<<=` | O(N/word) for whole-set |
| D | `reset`; cannot change N | — |

**Choose bitset** for compile-time N flags or a sieve. **Not**
`vector<bool>`. Runtime N: `vector<char>`. See
[`containers/bitset.md`](containers/bitset.md).

---

## Iterator invalidation (the silent bug)

```
  vector push_back that reallocates
  old:  [ a b c _ ] <--- it
  new:  [ a b c d _ _ _ _ ]     it is garbage
```

| Action | `vector` | `deque` | `list` / `set` / `map` | `unordered_*` |
| --- | --- | --- | --- | --- |
| insert at end | refs OK unless realloc | refs OK; iterators **no** | others OK | may rehash: all iterators die |
| insert middle | at and after die | all iterators die | others OK | may rehash |
| erase | at and after die | all iterators die (except ends, impl-defined details — treat as fragile) | only erased | only erased (avg) |

`list`/`set`/`map`: erase invalidates **only** the erased iterator.
That is why LRU is `list` + `unordered_map` of iterators.

---

## Algorithms: what they cost

Algorithms take **iterator pairs**, not containers. Complexity is
in terms of `N = last - first` (or walks, for non-random iterators).

`F` = cost of the functor you pass. Tables assume `F` is O(1).

### Query / non-modifying — `<algorithm>`

| Algorithm | Typical use | Time | Extra space |
| --- | --- | --- | --- |
| `for_each` | side effects on each | O(n) | O(1) |
| `count` / `count_if` | how many match | O(n) | O(1) |
| `find` / `find_if` / `find_if_not` | first match | O(n) | O(1) |
| `find_end` / `find_first_of` / `search` | subsequence | O(n·m) | O(1) |
| `adjacent_find` | first equal pair | O(n) | O(1) |
| `mismatch` | first difference | O(n) | O(1) |
| `equal` | ranges equal? | O(n) | O(1) |
| `all_of` / `any_of` / `none_of` | predicate over all | O(n) | O(1) |
| `search_n` | n equal values | O(n) | O(1) |

### Copy / mutate

| Algorithm | Typical use | Time | Extra space |
| --- | --- | --- | --- |
| `copy` / `copy_if` / `copy_n` / `copy_backward` | range → range | O(n) | O(1) |
| `move` / `move_backward` | steal elements | O(n) | O(1) |
| `swap` / `swap_ranges` / `iter_swap` | exchange | O(n) / O(1) | O(1) |
| `transform` | map 1:1 or 2:1 | O(n) | O(1) |
| `replace` / `replace_if` / `replace_copy` | overwrite matches | O(n) | O(1) |
| `fill` / `fill_n` / `generate` / `generate_n` | write values | O(n) | O(1) |
| `remove` / `remove_if` | **logical** delete (erase-remove) | O(n) | O(1) |
| `unique` | collapse adjacent dups | O(n) | O(1) |
| `reverse` / `reverse_copy` | flip | O(n) | O(1) |
| `rotate` / `rotate_copy` | left-cycle | O(n) | O(1) |
| `shift_left` / `shift_right` (C++20) | slide | O(n) | O(1) |
| `sample` (C++17) | pick k at random | O(n) | O(1) |
| `shuffle` | random perm | O(n) | O(1) |

`remove` does **not** shrink the container. Always:

```cpp
v.erase(std::remove_if(v.begin(), v.end(), pred), v.end());
```

### Sort, partition, select

| Algorithm | Typical use | Time | Extra space | Stable? |
| --- | --- | --- | --- | --- |
| `sort` | full order | O(n log n) avg | O(log n) | **no** |
| `stable_sort` | keep equal-key order | O(n log n) | O(n) | yes |
| `partial_sort` | smallest k in front, sorted | O(n log k) | O(1) | no |
| `nth_element` | k-th, unordered sides | avg O(n) | O(1) | no |
| `is_sorted` / `is_sorted_until` | check | O(n) | O(1) | — |
| `partition` / `stable_partition` | pred true in front | O(n) / O(n) | O(1) / O(n) | no / yes |
| `partition_point` | first false (already partitioned) | O(log n) | O(1) | — |

**Choose `nth_element`** for median / k-th. **`partial_sort`** if
you also need those k **sorted**. **`sort`** if you need everyone
sorted. **`priority_queue` of size k** if the stream is unbounded.

### Binary search (range **must** be partitioned / sorted)

| Algorithm | Typical use | Time |
| --- | --- | --- |
| `lower_bound` | first ≥ x | O(log n) |
| `upper_bound` | first > x | O(log n) |
| `equal_range` | [lo, hi) of x | O(log n) |
| `binary_search` | exists? | O(log n) |

On `set`/`map`, **use the member** `lower_bound`, not `std::lower_bound`.
The member is O(log n) tree hops. `std::lower_bound` on a bidirectional
iterator is still O(n) hops (it only does O(log n) *comparisons*).

### Heap (on a random-access range)

| Algorithm | Typical use | Time |
| --- | --- | --- |
| `make_heap` | build | O(n) |
| `push_heap` / `pop_heap` | after push_back / before pop_back | O(log n) |
| `sort_heap` | heap → sorted | O(n log n) |
| `is_heap` / `is_heap_until` | check | O(n) |

`priority_queue` is these four plus a `vector`.

### Set operations (both ranges **sorted**)

| Algorithm | Typical use | Time | Output size |
| --- | --- | --- | --- |
| `merge` | stable combine | O(n+m) | n+m |
| `inplace_merge` | merge two halves | O(n) | O(n) extra typical |
| `set_union` | A ∪ B | O(n+m) | ≤ n+m |
| `set_intersection` | A ∩ B | O(n+m) | ≤ min(n,m) |
| `set_difference` | A − B | O(n+m) | ≤ n |
| `set_symmetric_difference` | A Δ B | O(n+m) | ≤ n+m |
| `includes` | B ⊆ A? | O(n+m) | — |

Prefer these on **sorted vectors** over `set` if you build once and
query many times: better constants, contiguous.

### Min / max / permutation / numeric

| Algorithm | Header | Time | Extra space |
| --- | --- | --- | --- |
| `min` / `max` / `minmax` / `min_element` / `max_element` / `minmax_element` | `<algorithm>` | O(1) / O(n) | O(1) |
| `clamp` | `<algorithm>` | O(1) | O(1) |
| `next_permutation` / `prev_permutation` / `is_permutation` | `<algorithm>` | O(n) per step | O(1) |
| `iota` | `<numeric>` | O(n) | O(1) |
| `accumulate` / `reduce` | `<numeric>` | O(n) | O(1) |
| `inner_product` | `<numeric>` | O(n) | O(1) |
| `partial_sum` / `inclusive_scan` / `exclusive_scan` | `<numeric>` | O(n) | O(1) |
| `adjacent_difference` | `<numeric>` | O(n) | O(1) |
| `gcd` / `lcm` | `<numeric>` | O(log min(a,b)) | O(1) |

`reduce` may reorder; `accumulate` is left-to-right. Use `accumulate`
when `+` is not associative in practice (`float`, string concat).

---

## Complexity pocket card

```
                 find          insert end     insert middle    ordered walk
  vector         O(n)          A O(1)         O(n)             O(n) cache-hot
  deque          O(n)          A O(1) both    O(n)             O(n)
  list           O(n)          O(1)           O(1) if you *sit* there   O(n) cold
  set/map        O(log n)      O(log n)       O(log n)         O(n) sorted
  unordered_*    A O(1)        A O(1)         A O(1)           O(n) random
  priority_queue n/a           O(log n)       n/a              cannot
```

---

## File index

**Containers**

- [`containers/vector.md`](containers/vector.md) — timelines, prefix sums, undo
- [`containers/deque.md`](containers/deque.md) — sliding window, BFS
- [`containers/list.md`](containers/list.md) — LRU node list, splice
- [`containers/forward_list.md`](containers/forward_list.md) — forward chains
- [`containers/array.md`](containers/array.md) — fixed tables, histograms
- [`containers/string.md`](containers/string.md) — parsing, rolling windows
- [`containers/string_view.md`](containers/string_view.md) — non-owning text
- [`containers/bitset.md`](containers/bitset.md) — packed flags
- [`containers/set.md`](containers/set.md) — unique ordered, online min/max
- [`containers/map.md`](containers/map.md) — ranges, coordinate compress
- [`containers/unordered_set.md`](containers/unordered_set.md) — seen-set
- [`containers/unordered_map.md`](containers/unordered_map.md) — frequency, graph
- [`containers/stack.md`](containers/stack.md) — parens, monotonic stack
- [`containers/queue.md`](containers/queue.md) — BFS, level order
- [`containers/priority_queue.md`](containers/priority_queue.md) — top-k, Dijkstra
- [`containers/span.md`](containers/span.md) — non-owning APIs

**Mechanics (after the iterator intro)**

- [`00_beginner.md`](00_beginner.md)
- [`14_read_a_problem.md`](14_read_a_problem.md)
- [`problem_catalog.md`](problem_catalog.md)
- [`rules.md`](rules.md)
- [`cheat_sheet.md`](cheat_sheet.md)
- [`tracks.md`](tracks.md)
- [`03_iterator_adapters.md`](03_iterator_adapters.md)
- [`04_insert_emplace_extract.md`](04_insert_emplace_extract.md)
- [`05_hash_compare_lookup.md`](05_hash_compare_lookup.md)
- [`06_ranges.md`](06_ranges.md)
- [`07_pitfalls.md`](07_pitfalls.md)
- [`08_types_in_containers.md`](08_types_in_containers.md)
- [`09_memory_reserve_pmr.md`](09_memory_reserve_pmr.md)
- [`10_predicates.md`](10_predicates.md)
- [`11_threads_exceptions.md`](11_threads_exceptions.md)
- [`12_debug_measure.md`](12_debug_measure.md)
- [`13_parallel_and_cpp23.md`](13_parallel_and_cpp23.md)
- [`15_expert.md`](15_expert.md)
- [`16_supporting_types.md`](16_supporting_types.md)
- [`glossary.md`](glossary.md)
- [`exercises.md`](exercises.md)
- [`capstone.md`](capstone.md)

**Algorithms**

- [`algorithms/iterator_requirements.md`](algorithms/iterator_requirements.md) — which iterator each algo needs
- [`algorithms/non_modifying.md`](algorithms/non_modifying.md)
- [`algorithms/modifying.md`](algorithms/modifying.md)
- [`algorithms/sorting.md`](algorithms/sorting.md)
- [`algorithms/binary_search.md`](algorithms/binary_search.md)
- [`algorithms/heap.md`](algorithms/heap.md)
- [`algorithms/partition.md`](algorithms/partition.md)
- [`algorithms/set_operations.md`](algorithms/set_operations.md)
- [`algorithms/numeric.md`](algorithms/numeric.md)
- [`algorithms/permutations.md`](algorithms/permutations.md)

**Multi-container recipes**

- [`recipes/README.md`](recipes/README.md)
- [`recipes/lru_cache.md`](recipes/lru_cache.md) — `list` + `unordered_map`
- [`recipes/two_sum_and_anagrams.md`](recipes/two_sum_and_anagrams.md)
- [`recipes/top_k_frequent.md`](recipes/top_k_frequent.md)
- [`recipes/sliding_window.md`](recipes/sliding_window.md)
- [`recipes/median_stream.md`](recipes/median_stream.md)
- [`recipes/merge_intervals.md`](recipes/merge_intervals.md)
- [`recipes/graph_bfs_dijkstra.md`](recipes/graph_bfs_dijkstra.md)
- [`recipes/monotonic_stack_queue.md`](recipes/monotonic_stack_queue.md)
- [`recipes/inverted_index.md`](recipes/inverted_index.md)
- [`recipes/task_scheduler.md`](recipes/task_scheduler.md)
- [`recipes/leaderboard.md`](recipes/leaderboard.md) — `unordered_map` + `set`
- [`recipes/randomized_set.md`](recipes/randomized_set.md) — `vector` + index map
- [`recipes/time_based_kv.md`](recipes/time_based_kv.md) — hash of sorted vectors
- [`recipes/news_feed.md`](recipes/news_feed.md) — graph + logs + heap
- [`recipes/min_stack.md`](recipes/min_stack.md) — two stacks
- [`recipes/hit_counter.md`](recipes/hit_counter.md) — timestamp queue
- [`recipes/lfu_cache.md`](recipes/lfu_cache.md) — hash + frequency lists
- [`recipes/autocomplete.md`](recipes/autocomplete.md) — `map` / trie
- [`recipes/stock_tracker.md`](recipes/stock_tracker.md) — `map` + `multiset`
- [`recipes/freq_stack.md`](recipes/freq_stack.md) — freq map + stacks
- [`recipes/three_plus.md`](recipes/three_plus.md) — 3+ indexes, CRUD on several axes
- [`recipes/movie_rental.md`](recipes/movie_rental.md)
- [`recipes/mk_average.md`](recipes/mk_average.md)
- [`recipes/ttl_cache.md`](recipes/ttl_cache.md)
- [`recipes/order_book.md`](recipes/order_book.md)
- [`recipes/dinner_plates.md`](recipes/dinner_plates.md)
- [`recipes/first_unique.md`](recipes/first_unique.md)
- [`recipes/delayed_queue.md`](recipes/delayed_queue.md)
- [`recipes/max_stack.md`](recipes/max_stack.md)
- [`recipes/many_to_many.md`](recipes/many_to_many.md)
- [`recipes/file_system.md`](recipes/file_system.md)

**Code** lives in [`examples/`](examples/). Each file is a complete
`main` you can step through. First hour:
[`examples/80_beginner_hello.cpp`](examples/80_beginner_hello.cpp).
Seven-questions decode:
[`examples/81_decode_two_sum.cpp`](examples/81_decode_two_sum.cpp).
