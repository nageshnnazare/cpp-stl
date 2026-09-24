# Exercises

Do these on paper first, then in a file. Answers are at the
bottom. A star means "open the chapter if you stall."
Identify-the-container drills are the ones that make this
repo a rulebook; do those even if you skip the rest.

Decode method: [`14_read_a_problem.md`](14_read_a_problem.md)
(ASK / ARRIVE / EXTRA — do not invent the missing verbs).

## Containers

**1.** You append events and later need `events[i]` in a
simulation. Which container? Why not `list`?

**2.** You need the smallest unused id, and ids are returned
to a pool. `priority_queue` or `set`? What operation forces
the answer?

**3.** LRU cache: name the two containers and which one
stores iterators to the other. What invalidation rule makes
that legal?

**4.** `m["x"]` on an empty `map<string,int>`. What is
`m.size()` after the expression is evaluated? How do you
test membership without inserting?

**5.** A hash map of `string` keys, millions of `find` on
string literals. Which chapter's trick avoids a temporary
`string`?

## Iterators and algorithms

**6.** Why does `std::sort(lst.begin(), lst.end())` fail to
compile on a `std::list`? What do you call instead?

**7.** You `std::lower_bound(s.begin(), s.end(), x)` on a
`set`. It compiles. What is the complexity, and what should
you have written?

**8.** `copy_if` into an empty `vector` without
`back_inserter`. What goes wrong?

**9.** After `remove_if`, `v.size()` is unchanged. Write the
one extra call. C++20 one-liner?

**10.** You need the 10 smallest of 10⁷ values already in a
`vector`, and you will print them sorted. `nth_element`,
`partial_sort`, or `sort`? Why?

## Design

**11.** Cancel a job by id, pop all jobs with `time <= now`.
Why is a `priority_queue` of `(time, id)` not enough unless
you add a third structure?

**12.** `T`'s move constructor is not `noexcept`. What does
`vector<T>` do on realloc, and why?

## Identify (the rulebook drills)

For each, write: (a) ASK, (b) ARRIVE, (c) EXTRA (or “none”),
(d) the container(s), (e) what you refuse because it was
*not* in the sentence. One sentence each.

**13.** Count votes, then print the winner's name. n = 1e6
candidates live in a stream of ballots (names, not dense
ids).

**14.** Max of every window of length k on an array.

**15.** Online: insert a number, delete a number, query
current min and max.

**16.** A function `sum(xs)` must accept `vector<int>`,
`array<int,N>`, and a C array.

**17.** Player id → score, plus "add 5 to this id", plus
"print top 10 ids". Scores get corrected.

**18.** Shortest path in a maze (4-direction, every step
costs 1).

**19.** Shortest path in a graph with positive edge weights.

**20.** Return two indices whose values sum to `target`.

**21.** Implement a stack that also returns the current
minimum in O(1).

**22.** `insert`, `erase`, `getRandom` all average O(1),
uniform over the live set.

**23.** Autocomplete: dictionary of words with frequencies;
query a prefix, return top 3 hits.

**24.** Cache with capacity and a TTL on each key; get
refreshes recency but not TTL; expired keys must not count
toward capacity.

**25.** `for (auto& kv : m) if (kv.second == 0) m.erase(kv.first);`
on an `unordered_map`. What is wrong, and what are the two
legal fixes?

---

## Answers

1. `vector`. You need O(1) index and append. `list` makes
   `events[i]` O(i) and cold in cache. \*
   [`containers/vector.md`](containers/vector.md)

2. `set` (or a min-heap **plus** a way to delete an id that
   comes back). `priority_queue` cannot erase an arbitrary
   id; a returned id that still sits in the heap is stale.
   \* [`02_decision_guide.md`](02_decision_guide.md)

3. `list` of `{key,val}` (MRU..LRU) and
   `unordered_map<key, list::iterator>`. `list::erase`
   invalidates **only** the erased iterator. \*
   [`recipes/lru_cache.md`](recipes/lru_cache.md)

4. Size is 1: `[]` default-inserted `0`. Use `find` /
   `contains` / `at`. \* [`07_pitfalls.md`](07_pitfalls.md)

5. Transparent hash + eq, or `set<string, less<>>` for the
   ordered cousin, and `find(string_view)`. \*
   [`05_hash_compare_lookup.md`](05_hash_compare_lookup.md)

6. `sort` needs random-access. `lst.sort()` is merge-sort on
   nodes. \* [`01_iterators.md`](01_iterators.md)

7. O(n) iterator hops. `s.lower_bound(x)` is O(log n) tree
   hops. \* [`algorithms/iterator_requirements.md`](algorithms/iterator_requirements.md)

8. `copy` *writes* through the output iterator; empty
   `begin()` is `end()`, so you write off the end (UB).
   `back_inserter` calls `push_back`. \*
   [`03_iterator_adapters.md`](03_iterator_adapters.md)

9. `v.erase(it, v.end())` with `it` the return of
   `remove_if`. C++20: `std::erase_if(v, pred)`. \*
   [`algorithms/modifying.md`](algorithms/modifying.md)

10. `partial_sort` for 10 sorted. `nth_element` then
    `sort` on the prefix also works. Full `sort` is
    Θ(n log n) for 10⁷ when you only needed n log k. \*
    [`algorithms/sorting.md`](algorithms/sorting.md)

11. A heap cannot cancel by id. You need an id → node (hash)
    plus a time-ordered structure (`map<time, list>`), or a
    heap plus a dead-set (lazy). \*
    [`recipes/delayed_queue.md`](recipes/delayed_queue.md)

12. It **copies** elements to the new buffer so a throwing
    move cannot destroy the old one (strong guarantee).
    Mark real steal-moves `noexcept`. \*
    [`08_types_in_containers.md`](08_types_in_containers.md)
    [`09_memory_reserve_pmr.md`](09_memory_reserve_pmr.md)

13. ASK: winner's name. ARRIVE: stream of name ballots. EXTRA:
    none. `unordered_map<string,int>`. Refuse `map` (no ordered
    walk asked) and refuse a leaderboard `set` (no "reset id"). \*
    [`14_read_a_problem.md`](14_read_a_problem.md) decode 12

14. ASK: max of each window of length k. ARRIVE: array.
    EXTRA: window of k. `deque` of indices. Refuse a heap
    unless you accept lazy delete — EXTRA did not say
    "delete by id." \*
    [`recipes/sliding_window.md`](recipes/sliding_window.md)

15. ASK: min and max. ARRIVE: online. EXTRA: insert **and**
    delete a number. `multiset`. Refuse `priority_queue`
    because delete **is written**. \*
    [`rules.md`](rules.md) R33

16. ASK: one `sum` for several buffer types. ARRIVE: caller
    owns the data. EXTRA: none (no store). `span<const int>`.
    Refuse `const vector&` — that forces copies the sentence
    did not ask for. \*
    [`containers/span.md`](containers/span.md)

17. ASK: add 5 to an id, print top 10. ARRIVE: mixed.
    EXTRA: scores get **corrected** (by id). Hash +
    `set{-score,id}`. Refuse a heap — EXTRA said correct. \*
    [`recipes/leaderboard.md`](recipes/leaderboard.md)

18. ASK: shortest path. ARRIVE: maze. EXTRA: every step
    costs 1 (unweighted). `queue`. Refuse Dijkstra — EXTRA
    did not mention varying weights. \* [`recipes/graph_bfs_dijkstra.md`](recipes/graph_bfs_dijkstra.md)

19. ASK: shortest path. EXTRA: **positive weights** (written).
    Min-heap. Refuse FIFO `queue`. \* [`examples/36_recipe_graph.cpp`](examples/36_recipe_graph.cpp)

20. ASK: two indices. ARRIVE: array. EXTRA: none. Hash map.
    Refuse nested loops; refuse `map` (no order asked);
    refuse LRU (no eviction asked). \* [`examples/81_decode_two_sum.cpp`](examples/81_decode_two_sum.cpp)

21. ASK: push/pop/top **and** getMin O(1). EXTRA: getMin
    (a stack does not have it). Two stacks. Refuse `multiset`
    unless EXTRA also said erase-from-middle. \*
    [`recipes/min_stack.md`](recipes/min_stack.md)

22. ASK: insert, erase, getRandom O(1). EXTRA: all three
    are written → two axes. `vector` + index map. Refuse
    `unordered_set` — getRandom **is** in the sentence. \*
    [`recipes/randomized_set.md`](recipes/randomized_set.md)

23. ASK: top 3 for a **prefix**. EXTRA: prefix (order of
    keys). `map` or trie. Refuse `unordered_map` — prefix
    walk **is** written. \* [`recipes/autocomplete.md`](recipes/autocomplete.md)

24. ASK: get/put with capacity. EXTRA: TTL **and** LRU
    (both written) → three indexes. List + hash + expiry
    set. Refuse LRU-only — TTL **is** in the sentence. \* [`recipes/ttl_cache.md`](recipes/ttl_cache.md)

25. Range-for holds a dead iterator after `erase`; insert
    is not even the issue — **erase** of the current node
    is enough for UB. Fixes: (1) `it = m.erase(it)` loop;
    (2) collect keys, then erase (safer if you also insert).
    \* [`02_decision_guide.md`](02_decision_guide.md) patterns 2–3
