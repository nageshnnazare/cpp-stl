# 14. How to read a problem and pick a container

This is the rulebook chapter. A problem statement is **not**
a CRUD form. It almost never names create / update / delete
/ “what is hot.” You are not supposed to invent those seven
answers. You underline what **is** written, and you treat
every missing verb as **do not pay for that operation**.

The 30-second picker in
[`02_decision_guide.md`](02_decision_guide.md) is the
pocket version. The lookup table of *shapes* is
[`problem_catalog.md`](problem_catalog.md). Laws:
[`rules.md`](rules.md).

---

## Three questions that *are* in every problem

Every statement has (1) an **ask**, (2) an **arrival**, and
(3) either extra verbs or silence. That is enough to pick.

```
  1. ASK      What must I return or support?
              (the output, the query API — this is always there)

  2. ARRIVE   How does the data show up?
              given array / stream / mixed query list / "implement a class"

  3. EXTRA    Which other verbs are actually written?
              delete, cancel, update-by-id, expire, getRandom, "last K"…
              If the sentence does not contain them, they are NOT required.
```

Then pick so **ASK is cheap**. Pay for EXTRA only. Arrival
decides *online* (insert as you go) vs *offline* (build,
then query).

```
  "Given nums and target, return two indices that add to target."

  ASK     two indices whose values sum to target
  ARRIVE  whole array, one query
  EXTRA   none  (no delete, no update, no "by id", no order)

  → hot op is "have I seen target-a[i]?" → unordered_map
    missing delete is an *answer*: you do not need a set or a list
```

```
  "Design a stack: push, pop, top, getMin, all O(1)."

  ASK     those four methods
  ARRIVE  mixed calls (a class)
  EXTRA   getMin   (a normal stack has no min)

  → two stacks.  No "how named", no erase-by-key in the spec.
```

```
  "Return the k most frequent words."

  ASK     k words, high count first
  ARRIVE  a list of words (offline)
  EXTRA   none

  → count with unordered_map, then select (heap or partial_sort).
    Do not build a leaderboard set. Nobody asked to update a score later.
```

**Silence is the design.** If they never say cancel, do not
add an id-locator. If they never say “the i-th”, do not pick
`vector` for indexing. If they never say “in order”, do not
pick `map`.

---

## When a slot is blank (optional seven-slot sheet)

The seven names below are a **checklist for leftovers**, not
a form the interviewer fills. Fill only what the statement
supports. Write `n/a` or a **default** for the rest — that
blank *is* the answer.

```
  1. NOUN     one record          default: the element of the input
  2. ID       how you name it     default: position in the input, or
                                  "the current extreme" if ASK is min/max
  3. CREATE   how it enters       default: the input is given up front
  4. READ     how you get it      THIS IS THE ASK. Always fill this.
  5. UPDATE   change in place     default: n/a  (overwrite only if they say so)
  6. DELETE   how it leaves       default: n/a
  7. HOT      what runs n times   default: whatever implements ASK
                                  on the arrival pattern
```

| Statement says nothing about | You do **not** | Default container instinct |
| --- | --- | --- |
| delete / remove / expire / cancel / evict | pay for `erase` by key | vector, hash, or heap is enough |
| update / increment / correct / reset this id | a second index for “find then move” | one structure |
| “the i-th” / random access | `vector` for indexing | whatever ASK needs |
| “by id” / “by key” | a hash | scan or position |
| sorted / alphabetical / closest / predecessor | `map`/`set` | `unordered_*` or sort once at the end |
| both ends / queue / window | `deque` | `vector` or `stack` |
| stream / after each insert | an online structure | sort the whole array once |
| O(1) / O(log n) | that complexity | O(n) scan is legal if n is the input size and they did not constrain it |

**Overfitting** is picking LRU because you memorized LRU,
when the problem never mentioned eviction. Match EXTRA, not
your favorite recipe.

**Underfitting** is ignoring a verb that *is* written
(“reset the player” → heap is illegal).

If two ASK/EXTRA verbs need two layouts (lookup by id **and**
walk by score), that is two **axes** — two containers —
and only then must destroy unhook both. If delete is `n/a`,
there is nothing to unhook.

```
  axes = distinct cheap lookups the *statement* promised
         (not ones you imagined)

  1 stated lookup   →  1 container
  2 stated lookups  →  2 containers
  delete n/a        →  no destroy checklist
```

---

## English → operation (memorize this vocabulary)

The problem statement almost never says `unordered_map`.
It says a sentence whose **verb** is an STL operation.
**Only rows whose words appear in the statement apply.**

| The problem says | The operation | First container to consider |
| --- | --- | --- |
| "the i-th", "k-th event", "random access" | index | `vector` (`array` if N compile-time) |
| "append-only log", "undo last" | push/pop back | `vector` (or `stack`) |
| "push both ends", "queue", "BFS" | FIFO / deque ends | `queue` / `deque` |
| "have I seen this?" | membership | `unordered_set` |
| "how many times does k appear?" | frequency | `unordered_map` |
| "id → record in O(1)" | key lookup, no order | `unordered_map` |
| "print keys in order", "next key after x" | ordered walk / predecessor | `map` / `set` |
| "smallest unused id", "delete arbitrary then min" | min + erase anywhere | `set` / `multiset` |
| "always the current max, stream in" | extreme only | `priority_queue` |
| "current max AND remove value 7" | extreme + erase by key | `multiset` (heap cannot) |
| "top k of n", k ≪ n | select | heap of size k, or `partial_sort` |
| "median of a stream" | two extremes | two heaps or two `multiset`s |
| "next greater to the right" | monotonic LIFO | `stack` of **indices** |
| "max in a sliding window" | decreasing deque | `deque` of indices |
| "shortest path, equal weights" | FIFO | `queue` + `vector` dist |
| "shortest path, positive weights" | smallest dist first | `priority_queue` |
| "last K seconds / last K items" | expire the left | `queue` / `deque` |
| "O(1) get and put, evict oldest unused" | hash + splice | `unordered_map` + `list` (LRU) |
| "O(1) insert, delete, getRandom" | index + swap-pop | `vector` + `unordered_map` |
| "change this player's score, then top-k" | id lookup + ordered rank | `unordered_map` + `set` |
| "cancel job by id, pop due jobs" | id handle + time order | hash of iterators + `map<time, list>` |
| "prefix of a word", autocomplete | ordered keys or trie | `map` / nested `map` |
| "range of keys [L, R]" | ordered range | `map.lower_bound` .. `upper_bound` |
| "merge overlapping intervals" | sort by start | `vector` + `sort` |
| "how many overlap at once" | sweep line | `map<time, delta>` |
| "subarray sum equals k" | prefix + complement | `vector` + `unordered_map` |
| "group anagrams" | canonical key | sort letters, `unordered_map` |
| "union / intersection of two sorted lists" | linear merge | `set_union` on sorted `vector`s |
| "dense ids 0..n-1" | index **is** the key | `vector`, not `map<int,V>` |
| "compile-time N flags" | packed bits | `bitset<N>` |
| "function should accept vector or array" | non-owning slice | `span` |
| "do not copy the string, it is a parameter" | non-owning text | `string_view` (owner lives) |

If two rows both apply and they disagree (`O(1) by id` AND
`walk in score order`), that is **two rows, two containers**.

---

## The default, then the exception

```
  Start at vector.

  Leave vector only when a HOT operation is the wrong shape:

    front insert in a loop          → deque
    membership / id lookup          → unordered_set / unordered_map
    ordered keys / predecessor      → set / map
    only the extreme, no erase-by-id → priority_queue
    extreme AND erase-by-id         → multiset
    O(1) cut of a node you hold     → list (+ hash of iterators)
    compile-time N, no heap         → array / bitset
    you do not own the buffer       → span / string_view
```

"I insert in the middle a lot" is **not** a reason to leave
`vector` unless you **already hold an iterator** to that
middle. Finding the middle is O(n) in a `list` too; then
`list` loses on cache.

---

## Count the axes (the design move)

```
  Problem: "update score by player id, query top K"

  Axis A:  id → score          hot: Update, Read by id     unordered_map
  Axis B:  score order         hot: Read top K             set{-score,id}

  Destroy(id) must:  ranked.erase({-old,id}); scores.erase(id);
```

```
  Problem: "O(1) LRU cache"

  Axis A:  key → node          hot: get                     unordered_map
  Axis B:  recency order       hot: move-to-front, evict    list

  list::erase does not kill other iterators  ← why this pair is legal
```

```
  Problem: "schedule(id, time), cancel(id), pop due"

  Axis A:  id → node           cancel                       unordered_map
  Axis B:  time order          pop due                      map<time, list>

  A heap of (time,id) has no cancel unless you add axis A
  as a dead-set (lazy) — that is still two axes
```

**Rule:** one promised O(1) or O(log n) lookup = one
container. A second ordered view that Destroy forgets is
not a design. It is a bug that shows up next week.

---

## Worked decodes

Each decode starts from **ASK / ARRIVE / EXTRA** (what the
sentence actually contains). Seven-slot rows use `n/a` when
the statement is silent — that is deliberate.

### 0. Sparse statements (most problems look like this)

**A.** *Given an integer array `nums`, return true if any
value appears at least twice.*

```
  ASK     boolean: a duplicate exists
  ARRIVE  whole array
  EXTRA   none
```

`unordered_set`, insert while scanning, return on a hit.
No delete, no update, no index in the output, no order.
`set` works and is slower. `sort` + adjacent equal also
works (offline, extra log). You do **not** need a frequency
map: ASK is yes/no, not counts.

**B.** *Given `intervals`, merge all overlapping intervals.*

```
  ASK     a list of merged [start,end]
  ARRIVE  whole list
  EXTRA   none
```

Sort a `vector` of pairs, scan once. Delete of an input
interval is `n/a` — you emit a **new** vector. No `map`
unless a later sentence says “online insert” or “max
concurrent.”

**C.** *Implement `next()` and `hasNext()` for a BST
iterator.*

```
  ASK     next in-order value, O(1) average
  ARRIVE  a tree (already built)
  EXTRA   none (no insert/delete of nodes during iteration)
```

`stack` of nodes (controlled recursion). If they added
“insert a node while iterating,” EXTRA would change the
design. They did not.

---

### 1. Two Sum

*Given an array of ints and a target, return two indices
that add to target.*

```
  ASK     two indices
  ARRIVE  whole array, one query
  EXTRA   none
```

| Slot | Answer |
| --- | --- |
| Noun | a value at an index |
| Id | the index (ASK returns indices — that *is* in the sentence) |
| C | remember values as we scan |
| R | "have I seen `target - a[i]`?" |
| U | n/a (last index overwrite is an implementation detail) |
| D | **n/a** — the statement never removes anything |
| Hot | R, once per element |

**Container:** `unordered_map<int,int>` value → index.
**Not `map`:** no ordered walk. **Not two nested loops:**
that is O(n²). **Not `[]` for the complement:** `[]` would
insert. Use `find`.
Code: [`examples/10_unordered_map_freq.cpp`](examples/10_unordered_map_freq.cpp).

### 2. Valid parentheses

*Given a string of `()[]{}`, is it balanced?*

```
  ASK     true/false
  ARRIVE  one string
  EXTRA   none  (LIFO is inferred from "matching", not from a CRUD form)
```| Q | Answer |
| --- | --- |
| Noun | an unmatched opener |
| Id | "the most recent unmatched" — LIFO |
| C | push opener |
| R | top opener |
| U | none |
| D | pop when a closer matches |
| Hot | C/D per character |

**Container:** `stack<char>` (or `vector` + `back()`).
**Not `queue`:** FIFO would match the oldest opener.
**Not `list`:** no benefit.
Code: [`examples/11_stack_parens_next_greater.cpp`](examples/11_stack_parens_next_greater.cpp).

### 3. Binary tree level order / unweighted shortest path

*Visit nodes by distance from the source.*

| Q | Answer |
| --- | --- |
| Noun | a node id |
| Id | the id (dense 0..n-1 → `vector` of adj) |
| C | push a newly discovered node |
| R | pop the **oldest** undiscovered (FIFO) |
| D | pop front |
| Hot | C/D once per node/edge |

**Container:** `queue<int>` + `vector<int> dist`.
**Not `priority_queue`:** all edges weight 1, FIFO already
visits in increasing distance. **Not `list`:** you only
touch ends.
Code: [`examples/12_queue_bfs.cpp`](examples/12_queue_bfs.cpp).

### 4. Sliding window maximum

*Max of every `a[i..i+k)`.*

| Q | Answer |
| --- | --- |
| Noun | a candidate index in the window |
| Id | the index (need expiry: `i - k`) |
| C | push i at the back |
| R | front is the max |
| U | pop back while `a[back] <= a[i]` (dominated) |
| D | pop front if expired; pop back if dominated |
| Hot | amortized O(1) per i |

**Container:** `deque<int>` of indices, decreasing
`a[dq[i]]`. **Not a heap** unless you accept lazy delete
(O(n log n)). **Not `multiset`** unless you want simpler
O(n log k) code.
Code: [`examples/02_deque_sliding_window.cpp`](examples/02_deque_sliding_window.cpp).

### 5. LRU cache

*O(1) `get`/`put`, evict least recently used at capacity.*

```
  ASK     get and put in O(1)
  ARRIVE  mixed calls, bounded size
  EXTRA   evict LRU  ← this sentence is why list appears.
          Without "evict least recently used", a hash map alone is the answer.
```| Q | Answer |
| --- | --- |
| Noun | a (key, value) node |
| Id | the key **and** recency position |
| C | insert at MRU (front) |
| R | find by key in O(1), then the node |
| U | change value; splice node to front |
| D | erase LRU (back); erase by key |
| Hot | R+U every get; D on overflow |

**Two axes → two containers:** `list` of nodes +
`unordered_map<key, list::iterator>`.
**Not `vector`:** middle erase moves everyone, stored
indices die. **Not hash alone:** cannot rank recency.
**Not `list` alone:** find-by-key is O(n).
The pair is legal because `list::erase` / `splice`
invalidates only the moved/erased iterator.
Code: [`examples/03_list_lru.cpp`](examples/03_list_lru.cpp).

### 6. Running median

*Stream of ints; after each insert, the median.*

```
  ASK     median after each insert
  ARRIVE  stream (online)
  EXTRA   none — nobody said delete-a-value.
          Two heaps.  If a later constraint says "also erase x",
          EXTRA changed and you switch to two multisets.
```| Q | Answer |
| --- | --- |
| Noun | a number in the lower or upper half |
| Id | "the max of the low half", "the min of the high half" |
| C | push into one half, rebalance sizes |
| R | tops of the two heaps |
| D | pop a top when rebalancing |
| Hot | C each insert |

**Containers:** max-heap `lo` + min-heap `hi`.
**If you must later delete an arbitrary value:** heaps
cannot — switch both bags to `multiset`.
Code: [`examples/34_recipe_median.cpp`](examples/34_recipe_median.cpp).

### 7. Leaderboard with score updates

*Add to a player's score; query sum of top K; reset a player.*

```
  ASK     add-score, top-K, reset
  ARRIVE  mixed calls
  EXTRA   reset / correct by id  ← heap is illegal because this is written
```| Q | Answer |
| --- | --- |
| Noun | a (id, score) |
| Id | id, **and** rank by score |
| C | first time we see id |
| R | top K in score order; score of id |
| U | score += delta  (this **moves** rank) |
| D | reset = erase from both views |
| Hot | U and R(top K) |

**Two axes:** `unordered_map<id,score>` +
`set{-score, id}`. Update = erase old pair, insert new.
**Not a heap:** reset/correct cannot erase by id.
**Not `map<id,score>`:** top K would scan everyone.
Code: [`examples/40_recipe_leaderboard.cpp`](examples/40_recipe_leaderboard.cpp).

### 8. Insert / delete / getRandom in O(1)

*Set of ints; uniform random element.*

| Q | Answer |
| --- | --- |
| Noun | a value |
| Id | the value, **and** a dense index |
| C | append to a vector |
| R | `v[rand() % n]`; membership by key |
| U | none |
| D | swap with last, pop_back, fix the moved index |
| Hot | all three O(1) |

**Two axes:** `vector<int>` + `unordered_map<int,int>`
value → index. **Not `unordered_set`:** cannot index a
random slot. **Not `vector` alone:** delete is O(n) unless
you swap-pop, and swap-pop needs the index.
Code: [`examples/41_recipe_randomized_set.cpp`](examples/41_recipe_randomized_set.cpp).

### 9. Merge intervals

*Given `[start,end]` pairs, merge overlaps.*

| Q | Answer |
| --- | --- |
| Noun | an interval |
| Id | starts, in order |
| C | all intervals given up front |
| R | walk in start order, extend or emit |
| U | extend `out.back().end` |
| D | none (write a new vector) |
| Hot | one sort, one scan |

**Container:** `vector<pair<int,int>>` + `sort`.
**Not `unordered_map`:** order **is** the algorithm.
**Not `list`:** you sort once; contiguous wins.
If the query is "max concurrent", that is a **sweep**:
`map<time,delta>` — different problem, still ordered keys.
Code: [`examples/35_recipe_intervals.cpp`](examples/35_recipe_intervals.cpp).

### 10. Delay queue with cancel

*Schedule(id, time), cancel(id), pop all due.*

| Q | Answer |
| --- | --- |
| Noun | a job |
| Id | id **and** due time |
| C | insert into a time bucket |
| R | smallest time, then FIFO in the bucket |
| U | none (or reschedule = cancel+schedule) |
| D | cancel by id; pop a whole due bucket |
| Hot | C, D(cancel), R(due) |

**Two axes, three structures:** `map<time, list<Job>>` +
`unordered_map<id, list::iterator>`.
**Not `priority_queue` alone:** no cancel. Lazy dead-set
is the cheaper cousin if cancel is rare and pop can skip.
Code: [`examples/56_recipe_delayed_queue.cpp`](examples/56_recipe_delayed_queue.cpp).

### 11. First unique in a stream

*Add numbers; query the first number that appears once.*

| Q | Answer |
| --- | --- |
| Noun | a still-unique value, in arrival order |
| Id | the value **and** its place in line |
| C | append if first sighting |
| R | front of the unique line |
| U | none |
| D | on second sighting, cut that node in O(1); ignore later |
| Hot | C/D per add, R is O(1) |

**Three pieces:** `list` of uniques + hash to iterator +
`unordered_set` dead. **Not a queue alone:** cannot delete
the middle. **Not a hash alone:** no arrival order.
Code: [`examples/55_recipe_first_unique.cpp`](examples/55_recipe_first_unique.cpp).

### 12. Word frequency, then top 10

*Count tokens, print the 10 most common (ties: alpha).*

| Q | Answer |
| --- | --- |
| Noun | a (word, count) |
| Id | the word |
| C/U | ++count |
| R | top 10 after the file is read |
| D | none |
| Hot | C/U per token (huge); R once (tiny unique set, or huge) |

**Count:** `unordered_map<string,int>` — no order needed
while counting. **Select:** if unique words U is huge and
k=10, min-heap of size 10. If U is small, dump to
`vector<pair>` and `partial_sort`. **Not `map<string,int>`
for counting** unless you want alphabetical walk of *all*
words. **Not `sort` of 1e7 uniques for k=10.**
Code: [`examples/32_recipe_topk.cpp`](examples/32_recipe_topk.cpp),
[`examples/70_capstone_logs.cpp`](examples/70_capstone_logs.cpp).

---

## Algorithm decode (name the loop, not seven slots)

After the container is chosen, the hot *loop* often has a
name. The problem still will not say `partial_sort`. It
will say “the k smallest” or “already sorted, find x.”

| The loop is | Call |
| --- | --- |
| scan for a match / count / all-true | `find` / `count` / `all_of` |
| write a new range 1-to-1 | `transform` / `copy` / `copy_if` |
| drop some elements of a vector | `erase_if` / erase-remove |
| full order | `sort` |
| keep equal-key order | `stable_sort` |
| k smallest, sorted | `partial_sort` |
| the k-th / median of a buffer | `nth_element` |
| already sorted, find x / range | `lower_bound` / `equal_range` |
| already sorted, A ∩ B | `set_intersection` |
| pred true in front | `partition` |
| prefix sums | `partial_sum` |
| next permutation | `next_permutation` |

On `set`/`map`, **member** `lower_bound`, never
`std::lower_bound` on the iterators (O(n) hops).

---

## Check before you code

```
  [ ] ASK / ARRIVE / EXTRA written from the *sentence* (not from memory)
  [ ] Blank verbs are n/a — you did not invent cancel, getRandom, or order
  [ ] ASK is cheap on the chosen type
  [ ] Only EXTRA added extra containers (axes)
  [ ] If delete is n/a, no destroy checklist
  [ ] If delete *is* stated on several axes, destroy unhooks every axis
  [ ] Membership is find/contains, not map[]
  [ ] No erase inside range-for
  [ ] Heap not asked to erase 7 unless EXTRA said erase 7
  [ ] list not chosen unless EXTRA is O(1) cut of a held node
  [ ] Dense 0..n-1 is a vector, not map<int,V>
```

If a box fails, the container is wrong or you overfitted a
recipe. Fix the design; do not "just be careful."

---

## Practice

Decode on paper from ASK / ARRIVE / EXTRA, then open the
example. Catalog: [`problem_catalog.md`](problem_catalog.md).
Two Sum with blanks left blank:
[`examples/81_decode_two_sum.cpp`](examples/81_decode_two_sum.cpp).
Drills: [`exercises.md`](exercises.md) section "Identify."
