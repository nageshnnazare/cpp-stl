# 02. Decision guide: which container, which algorithm

Use this page when you are staring at a problem and do not want
to open twelve chapters. Then go to the chapter and write the
code.

**How to decode English into a container** (what the
sentence actually contains — ASK / ARRIVE / EXTRA; blanks
are n/a):
[`14_read_a_problem.md`](14_read_a_problem.md).
**Shape catalog:** [`problem_catalog.md`](problem_catalog.md).
**Numbered laws:** [`rules.md`](rules.md).
**One page:** [`cheat_sheet.md`](cheat_sheet.md).

**Thread-safety in one sentence:** no STL container is a
concurrent data structure. Many threads may call **const**
methods on the same object. **Any** insert / erase / `map[]` /
realloc needs a mutex you provide — or a copy you do not
share. Details in the table and in
[`11_threads_exceptions.md`](11_threads_exceptions.md).

## Thirty-second container picker

```
  Need the k-th by index a lot? ------------------> vector
       Need a compile-time length?  --------------> array
       Need a view into someone else's buffer? ---> span

  Need cheap push/pop at BOTH ends? --------------> deque
       Also need random index? still -------------> deque
       Only one end, and you want cache? ---------> vector
       BFS queue / undo stack? -------------------> queue / stack
                                                    (deque underneath)

  Need "give me min/max, then delete it"?
       Only the extreme, stream in? --------------> priority_queue
       Either extreme, or delete arbitrary? ------> multiset
       Running median? ---------------------------> two heaps or two multisets
                                                    (see recipes/median_stream)

  Need key -> value
       Lookup only, no order? -------------------> unordered_map
       Walk keys in order / closest key? --------> map
       Key is just "have I seen this"? ----------> unordered_set
       Unique + sorted iterate? -----------------> set

  Need to cut a node out in O(1) and you
  already hold a handle to it? ------------------> list (+ map of iterators)
       That sentence did not apply? -------------> not list
```

## If the picker is not enough (the sentence is incomplete)

Problems do not hand you create/update/delete. Fill only
what is written. Full method: [`14`](14_read_a_problem.md).

```
  1 ASK     what do they return / support?     (always there)
  2 ARRIVE  array / stream / mixed class API   (always there)
  3 EXTRA   delete, by-id, order, window, O(1)? (often *none*)
```

Silence means **do not pay** for that verb. `n/a` delete →
no erase-by-key structure. `n/a` order → `unordered_*`, not
`map`.

```
  1 noun  2 how named  3 create  4 read  5 update  6 delete  7 hot
  optional leftovers.  Blank = n/a = do not design for it.
```

| English | Operation | Try |
| --- | --- | --- |
| "the i-th" / append log | index + `push_back` | `vector` |
| "have I seen" / frequency | membership | `unordered_set` / `unordered_map` |
| "next key after x" / sweep | ordered + predecessor | `set` / `map` |
| "always the max", stream | extreme only | `priority_queue` |
| "max AND delete 7" | extreme + erase by key | `multiset` |
| "BFS" / "last K seconds" | FIFO / expire left | `queue` / `deque` |
| "next greater" | LIFO monotonic | `stack` of indices |
| "O(1) get/put + evict oldest unused" | hash + splice | `list` + `unordered_map` |
| "getRandom O(1) + delete" | index + swap-pop | `vector` + index map |
| "cancel by id AND next due" | two axes | hash of handles + ordered time |

## Cheap vs expensive (operation matrix)

Read this as: **if this verb is hot, the cell must be cheap.**
A = amortized. I = you already hold an iterator.

| Verb | `vector` | `deque` | `list` | `set`/`map` | `unordered_*` | heap (`pq`) |
| --- | --- | --- | --- | --- | --- | --- |
| Index `i` | **O(1)** | **O(1)** | O(n) | n/a | n/a | n/a |
| Scan all | **O(n) hot** | O(n) | O(n) cold | O(n) cold | O(n) random | cannot |
| Insert end | **A O(1)** | **A O(1)** | O(1) | O(log n) | A O(1) | O(log n) (as push) |
| Insert front | O(n) | **A O(1)** | O(1) | n/a | n/a | n/a |
| Insert middle, no handle | O(n) | O(n) | O(n) find | O(log n) by key | A O(1) by key | cannot |
| Find by key | O(n) | O(n) | O(n) | **O(log n)** | **A O(1)** | cannot |
| Predecessor / closest | sort+LB | sort+LB | n/a | **O(log n)** | cannot | cannot |
| Min, then delete min | `sort`/heap | — | — | **O(log n)** | n/a | **O(log n)** |
| Delete arbitrary key | O(n) | O(n) | O(n) find | **O(log n)** | **A O(1)** | **cannot** |
| Delete held iterator | O(n) shift | O(n) | **O(1)** | **O(1) I** | **A O(1)** | cannot |
| Ordered walk | after sort | after sort | no | **yes** | no | no |
| Random element | **O(1)** | O(1) | O(n) | O(n) | O(n) | cannot |

Gray cells ("cannot") mean **compose or change type**, not
"be careful." Heap + dead-set, or `multiset`, or `vector` +
index map: [`14`](14_read_a_problem.md) worked decodes 7–8, 10.

## Thread safety × erase-while-walk × how to get the job done

Legend for **threads**:

- **CR** = concurrent **const** reads of the *same* object are
  defined (C++11). `find`, `at`, `size`, `[]` on `vector` /
  `array`, iteration that does not write.
- **no W** = any non-const method vs another thread is a data
  race unless **you** lock. That includes `map::operator[]`
  (it may insert).
- **elem** = concurrent writes to **different** elements of
  `vector` / `deque` / `string` / `array` are OK **if** no
  realloc / insert / erase happens. Node containers: you may
  mutate `it->second` on different nodes if no insert/erase.
  Still lock if you are unsure.

Legend for **erase in a loop**: the range-for

```cpp
for (auto& x : c) c.erase(x);   // always UB — the loop's iterators die
```

is illegal on every container. The **legal** pattern depends
on what erase invalidates. Adapters (`stack` `queue`
`priority_queue`) have **no iterators** — you cannot walk them
at all.

| Container | Threads | Why a naive delete-in-loop blows up | How you actually delete / update while walking |
| --- | --- | --- | --- |
| `vector` `string` | CR; no W; **elem** if no realloc | `erase(it)` shifts the tail; **`it` and every iterator after it die**. Range-for still holds the old `it`. Also O(n) per erase → O(n²) | **Prefer** `erase(remove_if(...), end)` or C++20 `erase_if`. If you must loop: `it = v.erase(it)` else `++it`. **Update in place:** `v[i]=` is fine; do not `push_back` (realloc kills the walk) |
| `deque` | CR; no W; **elem** if no insert/erase | Middle `erase` invalidates **all** iterators (ends are fragile — treat as all). Same range-for bug | Same `it = erase(it)` dance. Ends-only: `pop_front` / `pop_back` outside a middle walk. **Do not** hold `deque::iterator` across `push_front` |
| `list` | CR; no W; mutate other nodes OK | Only the **erased** iterator dies. Range-for still UB because *its* `it` is the erased one | `it = lst.erase(it)` — O(1), other iterators (e.g. stored in a hash) stay. That is LRU. **Splice** to move without delete |
| `forward_list` | same as list, weaker API | `erase` does not exist; you erase **after** a position | `erase_after(prev)`. Walk with a trailing `before` iterator |
| `array` | CR; no W; **elem** | Cannot change length. "Delete" = mark a slot or `fill` a sentinel | Overwrite `a[i]`. No `erase` |
| `span` `string_view` | CR on the view; owner has its own rules | View cannot erase. If the **owner** reallocates, the view is dangling | Delete on the owner; rebuild the view. Never walk a view while the owner grows |
| `set` `map` `multiset` `multimap` | CR; no W; `it->second =` on **map** OK | Only erased iterator dies. Range-for UB on *that* `it`. **Cannot** assign `*it` on `set` (key is const) | `it = m.erase(it)`. Update a map **value**: `it->second =`. Update a **key**: `extract` + change `nh.key()` + `insert` ([`04`](04_insert_emplace_extract.md)). Insert during walk: new iterators do not invalidate others — but do not assume the new node is "the next `++`" |
| `unordered_set` `unordered_map` (+ multi) | CR; no W; value assign OK | `erase(it)` invalidates **only** `it` (C++11 `erase` returns the next). **`insert` may rehash → ALL iterators die** | Delete: `it = m.erase(it)`. **Never insert while iterating** unless you `reserve` enough that rehash cannot happen — still easy to get wrong; **collect keys, then insert**. Update: `it->second =` |
| `stack` `queue` | CR of `top`/`front`/`empty` only if no other writer | **No iterators.** There is no loop over elements | Delete = `pop` the exposed end only. To drop a middle value, you wanted a `deque` / `list` / `vector` |
| `priority_queue` | same | No iterators, no `erase(value)` | `pop` the extreme. To delete 7: `multiset`, or lazy-delete (push tombstones, skip on pop) |
| `bitset` | CR; no W; **elem** on different bits | No iterators in C++20. `reset(i)` is the delete | `reset(i)` / `flip`. Fine in a `for (i=0;i<N;++i)` index loop |
| `pmr::*` | **same as the container** it wraps | PMR does not add locks | Put the mutex around the `pmr::vector`, not around the arena, unless many containers share the resource |

### Why "just `erase` in the for-loop" is undefined

```
  range-for is:

      auto __end = c.end();
      for (auto __it = c.begin(); __it != __end; ++__it) {
          use *__it;
          c.erase(__it);     // __it is dead; ++__it is UB
                             // on vector, __end may be dead too
      }
```

Erase **changes the range**. The hidden `__it` / `__end` were
snapshotted for a range that no longer exists. Even
`for (auto it = c.begin(); it != c.end(); ++it) c.erase(it)`
increments a dead iterator.

### Patterns that *are* defined (memorize these)

**1. Erase-remove (sequences you can compact) — default for `vector`**

```cpp
v.erase(std::remove_if(v.begin(), v.end(), pred), v.end());
// C++20: std::erase_if(v, pred);
```

One pass, O(n), no iterator death in your loop because you
are not looping with `erase`.

**2. Iterator erase (node containers, or rare vector cases)**

```cpp
for (auto it = c.begin(); it != c.end(); ) {
    if (pred(*it)) it = c.erase(it);   // erase returns the next LIVE it
    else ++it;
}
```

Works on `vector` (slowly), `list`, `set`, `map`, `unordered_*`.
This is the one legal "delete while iterating".

**3. Collect, then mutate (when insert *and* erase happen)**

```cpp
std::vector<Key> kill, add;
for (auto& [k, v] : m) { /* read only */ if (pred) kill.push_back(k); }
for (auto& k : kill) m.erase(k);
for (auto& k : add)  m.insert(k);
```

The walk is **read-only**. Hash tables will not rehash under
your feet. Use this the moment the loop would insert.

**4. Index loop on `vector` (delete from the back, or swap-pop)**

```cpp
for (int i = (int)v.size() - 1; i >= 0; --i)
    if (pred(v[i])) v.erase(v.begin() + i);   // still O(n²)

// O(1) delete when order does not matter:
v[i] = std::move(v.back()); v.pop_back();     // randomized_set
```

**5. Copy, then walk the copy**

```cpp
auto snap = m;                 // expensive, but the walk is stable
for (auto& [k, v] : snap) m.erase(k);
```

Useful across threads: snapshot under a lock, process the
copy without the lock, write results back under the lock.

### How to get *what you wanted* when the container says no

| You wanted | Container said no | Do this |
| --- | --- | --- |
| Delete 7 from a heap | `priority_queue` has no `erase` | `multiset`, or lazy skip (`unordered_set` of dead ids, pop while top is dead) |
| Walk a stack | no iterators | use `vector` + `back()` as top |
| Delete while `for (x : unordered_map)` | rehash / invalid `it` | pattern 2 or 3 above |
| Insert while walking a hash map | rehash kills all `it` | `reserve(n)` then still prefer collect-then-insert |
| Change a `set` key | `*it` is const | `extract` + `nh.key()=` + `insert` |
| Concurrent writers | STL will not lock | `mutex` / `shared_mutex`, or one container per thread + merge, or a concurrent hashmap (TBB — not STL) |
| Concurrent read during write | data race | lock, or RCU/copy-on-write of a `shared_ptr<const Map>` |
| `map[]` in a const getter | `[]` is a write | `find` / `at` / `contains` |
| Filter a `vector` in a loop of `erase` | O(n²) + invalidation | erase-remove |
| Stable handles to elements across insert | `vector` iterators die on realloc | `list` / `map` iterators, or indices into a vector you **never** erase in the middle (`reserve` first), or slot maps |
| Random element + O(1) delete | `unordered_set` cannot index | `vector` + index map ([`randomized_set`](recipes/randomized_set.md)) |
| Min and max and delete arbitrary | heap cannot | `multiset` |
| Thread-safe queue | `queue` is not | `deque` + `mutex` + `condition_variable`, or a lock-free queue library |

## "I was about to use X" — common swaps

| Instinct | Usually better | Why |
| --- | --- | --- |
| `list` because "middle inserts" | `vector` | You still *find* the middle in O(n). Then `vector` insert is O(n) moves of cheap T, `list` is O(n) cache misses + alloc |
| `map<int,V>` for dense 0..n ids | `vector<V>` | Index **is** the key |
| `map` for Two Sum / frequency | `unordered_map` | No order needed; avg O(1) |
| `unordered_map` then sort keys | `map`, or `vector<pair>` + `sort` | If you sort anyway, a vector of pairs is faster to build |
| `set` to unique a vector once | `sort` + `unique` + `erase` | One allocation story, contiguous |
| `priority_queue` and then "remove 7" | `multiset` | Heap cannot find 7 |
| `queue` of pairs `(dist, node)` for Dijkstra | `priority_queue` | You need smallest dist, not FIFO |
| `stack` of indices for "next greater" | still `stack` | Correct — see monotonic stack |
| `vector` + `insert(begin)` in a loop | `deque` or `reverse` then `push_back` | Front insert on vector is O(n²) |
| `for (auto& x : v) v.erase(...)` | `erase_if` / `it = erase(it)` | Range-for iterators die; see table above |
| `m[k]` to test membership | `contains` / `find` | `[]` **creates** `k` |
| Share a `unordered_map` cache across threads with `[]` | `find` + mutex, or `shared_mutex` | `[]` is a write; two readers can race-insert |

## Thirty-second algorithm picker

```
  Scan for a match / count / all-any-none? -----> find* / count* / all_of...
  Write a new range 1-to-1? --------------------> transform / copy / copy_if
  Drop elements? -------------------------------> remove_if + erase  (not erase in a loop)
  Adjacent dups after sort? --------------------> unique + erase
  Full order? ----------------------------------> sort
       Must keep equal-key order? --------------> stable_sort
       Only the k smallest, sorted? ------------> partial_sort
       Only the k-th (median)? -----------------> nth_element
  Already sorted, find x? ----------------------> lower_bound / equal_range
  Already sorted, A ∩ B / A ∪ B? ---------------> set_* / merge
  Always want the max, streaming? --------------> heap ops or priority_queue
  Rearrange so pred is true in front? ----------> partition
       Then binary-search the split? -----------> partition_point
  Prefix sums / running xor? ------------------> partial_sum / accumulate
```

Algorithms are not thread-safe on a range another thread is
mutating. `std::execution::par` parallelizes **one** call
across threads; it still requires exclusive ownership of the
range. See [`13_parallel_and_cpp23.md`](13_parallel_and_cpp23.md).

## Worked choices (the rest of the repo in miniature)

**Undo for a drawing app.** Need: push a stroke, pop last stroke,
sometimes peek. **`vector` or `stack`**. Not `list` (no benefit).
Not `deque` unless you also undo from the *oldest*.

**Browser history with back and forward.** Two stacks, or a
`deque` plus an index. **Not** a `list` unless you also splice
tabs.

**Word frequency, then print top 10.** Count with
`unordered_map<string,int>`. Select top 10 with a min-heap of
size 10 (`priority_queue`) or `partial_sort` on a vector of
pairs. **Not** `map<string,int>` unless you want alphabetical
walk. **Not** `sort` the whole map if N is 10⁷ and k is 10.

**"Is this interval covered?"** Sort endpoints (`vector` +
`sort`) or a `map<int,int>` of sweep-line deltas. **Not**
`unordered_map`: order of keys *is* the algorithm.

**LRU cache.** O(1) get and put. Need: hash lookup + move-to-front
without walking. **`unordered_map<key, list::iterator>` +
`list<pair<key,val>>`**. This is the canonical "two containers"
design. See [`recipes/lru_cache.md`](recipes/lru_cache.md).
Erase of one list node does **not** invalidate the other
iterators sitting in the hash.

**Sliding window maximum.** Need the max in a moving `[i-k+1, i]`
in amortized O(1) per step. **`deque` of decreasing indices**.
A heap of windows is O(n log n) and needs lazy delete. A `multiset`
is correct O(n log k) and simpler. Pick deque when the constant
matters; multiset when you want less code.

**Game leaderboard (online top-K).** Hash id → score for O(1)
lookup, plus a `set` of `{-score, id}` so `begin()` is the
winner. **Not** a heap if you must reset / correct a score
(heaps cannot erase by id). See
[`recipes/leaderboard.md`](recipes/leaderboard.md).

**"getRandom in O(1)" plus insert/delete.** `vector` +
`unordered_map` to the index; delete is swap-with-last. See
[`recipes/randomized_set.md`](recipes/randomized_set.md).

**Need cancel-by-id AND "best by another key"** (rentals,
orders, TTL, delay jobs). That is **three indexes**, not two.
See [`recipes/three_plus.md`](recipes/three_plus.md). Destroy
must update **every** index.

## Space you actually pay

```
  vector<int> n=1e6     ~ 4 MB + capacity slack
  list<int>   n=1e6     ~ 4 MB + 2 pointers + alloc header  PER node
                        often 24-32 B/node  => 24-32 MB and cold

  map<int,int> n=1e6    3 pointers + color + 2 ints per node
                        tens of MB, log n hops that miss cache

  unordered_map         node + bucket array (load factor ~1)
```

If the problem fits in a `vector`, the "smarter" node container
is usually a memory and latency regression.

## What to read next

You have the picker. Decode a new problem with
[`14_read_a_problem.md`](14_read_a_problem.md), look up a
familiar shape in [`problem_catalog.md`](problem_catalog.md),
quote [`rules.md`](rules.md). Open the container that won,
write the example, then break it on purpose (omit `reserve`,
use `map` where `unordered_map` belongs, `erase` inside a
range-for) and look at the cost — or the sanitizer. Every
`examples/*.cpp` file repeats the thread / erase rule in its
header comment.
