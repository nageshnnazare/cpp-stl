# Problem catalog: shape → container

Open this when you recognize the *story* ("it's like LRU",
"it's a stream median") and want the container, the reason,
and the file. Decode a *new* story with
[`14_read_a_problem.md`](14_read_a_problem.md).

Names in parentheses are the usual LeetCode / contest
costumes of the same diagram.

---

## A. Sequence, index, buffer

| Shape | Container | Why not | Code |
| --- | --- | --- | --- |
| Append events, later `events[i]` | `vector` | `list` makes `[i]` O(i) | `01_vector_timeline` |
| Compile-time N (RGB, 256 bins, dice) | `array<T,N>` | `vector` heap + extra word | `05_array_histogram` |
| Accept `vector` or C array in one fn | `span` | `const vector&` forces copies | `14_span_api` |
| Text, parse, concat | `string` | `vector<char>` lacks `find` / SSO | `06_string_split_window` |
| Parameter that must not copy chars | `string_view` | dying owner dangling | `containers/string_view.md` |
| Prefix sums, difference array | `vector` + `partial_sum` | tree has no O(1) range-sum | `01`, `27_algo_numeric` |
| Undo / redo (one end) | `vector` or `stack` | `list` no benefit | `11_stack_*` |
| Browser back **and** forward | two stacks, or `deque`+index | `list` unless you splice tabs | — |
| Rotate / cycle a buffer | `vector` + `rotate`, or `deque` | — | `21_algo_mutate` |
| Insert at front in a loop | `deque` | `vector::insert(begin)` is O(n²) | `02_deque_*` |

## B. Membership, frequency, grouping

| Shape | Container | Why not | Code |
| --- | --- | --- | --- |
| Have I seen this value? | `unordered_set` | `set` if you also need order | `09_unordered_set_seen` |
| Frequency / Two Sum / complement | `unordered_map` | `map` extra log, no benefit | `10_unordered_map_freq` |
| Longest consecutive sequence | `unordered_set` + walk runs | `sort` is O(n log n) extra | `09` |
| Group anagrams | sort letters → hash of groups | `map` if you need sorted keys | `31_recipe_two_sum_anagrams` |
| Subarray sum equals k | prefix + `unordered_map` counts | nested O(n²) | `31` |
| Unique a vector **once** | `sort` + `unique` + `erase` | `set` allocates n nodes | `21_algo_mutate` |
| Distinct in a stream, then alpha dump | `unordered_set` then `vector`+`sort` | `set` if you need online sorted | `07_set_online` |
| Dense 0..n-1 → value | `vector<V>` | `map<int,V>` is a tree of ints | README pocket card |
| Byte / lowercase histogram | `array<int,256>` or `array<int,26>` | hash for 26 keys is slower | `05`, `33` |

## C. Ordered keys, predecessor, sweep

| Shape | Container | Why not | Code |
| --- | --- | --- | --- |
| Online min / max / successor | `set` | heap cannot successor or erase 7 | `07_set_online` |
| Time → value, query as-of t | `map<int,V>` or hash of sorted vectors | `unordered_map` has no upper_bound | `08_map_timeline`, `42` |
| Sweep: max concurrent intervals | `map<time,delta>` | hash scrambles time | `35_recipe_intervals` |
| Merge overlapping intervals | `vector` + `sort` | hash; `list` | `35` |
| Coordinate compress | `vector` + `sort` + `unique` + `lower_bound` | `map` if you must insert online | `containers/map.md` |
| Autocomplete / prefix range | `map.lower_bound(p)` walk, or trie | `unordered_map` cannot prefix-walk | `47_recipe_autocomplete` |
| Range of keys [L,R] | `map` `lower_bound`..`upper_bound` | hash | `08` |
| Small unused id pool | `set` of free ids | heap + lazy, or `priority_queue` **fails** on return-to-pool | exercises #2 |
| Sorted dictionary, many finds | `set<string, less<>>` transparent | temp `string` per find | `17_hash_transparent` |

## D. Heaps, top-k, streaming order statistics

| Shape | Container | Why not | Code |
| --- | --- | --- | --- |
| Always the max (or min), stream | `priority_queue` | `multiset` if you need erase / both ends | `13_priority_queue_topk` |
| Top-k of a stream, k ≪ n | min-heap of size k | full `sort` | `13`, `32` |
| Top-k frequent | count hash + heap **or** freq buckets | `sort` all uniques if U huge | `32_recipe_topk` |
| k smallest of a **buffer**, sorted | `partial_sort` | `nth_element` if unsorted k-th only | `22_algo_sort_select` |
| Median / k-th of a **buffer** | `nth_element` | `sort` | `22` |
| Running median of a **stream** | two heaps | two `multiset`s if deletes happen | `34_recipe_median` |
| Dijkstra (positive weights) | min-heap of `{dist,node}` | FIFO `queue` is BFS | `36_recipe_graph` |
| Merge k sorted lists | min-heap of k heads | k-way linear without heap is worse | `43_recipe_news_feed` (same merge) |
| Reorganize string / cooldown | max-heap of counts (+ queue for cool) | greedy one heap | `39_recipe_scheduler` |
| Need min **and** max **and** erase 7 | `multiset` | a heap has no erase(7) | `48_recipe_stock_tracker` |
| Lazy-delete heap | heap + `unordered_set` dead ids | skip `top()` while dead | `02_decision_guide` pattern |

## E. Stacks, queues, monotonic, windows

| Shape | Container | Why not | Code |
| --- | --- | --- | --- |
| Balanced parentheses / path simplify | `stack` | `queue` | `11` |
| Next greater / daily temperatures | monotonic `stack` of **indices** | O(n²) nested j | `11`, `37` |
| Sliding window maximum | decreasing `deque` of indices | heap + lazy, or `multiset` | `02`, `33` |
| Longest substring without repeat | two pointers + `array<int,256>` last | `list` | `33`, `06` |
| BFS / level order | `queue` | `stack` is DFS | `12` |
| Explicit DFS / backtracking path | `stack` or recursion | `queue` | `11` |
| Hit counter last 300s | `queue` of timestamps, or 300 buckets | `vector` pop_front is O(n) | `45_recipe_hit_counter` |
| Min in O(1) plus stack ops | two stacks (vals + mins) | scan | `44_recipe_min_stack` |
| Pop the max even if buried | `list` + `map` of iterators | two-stack min-stack cannot | `57_recipe_max_stack` |
| Freq-stack (pop most frequent) | freq hash + stack per frequency | one stack loses freq | `49_recipe_freq_stack` |

## F. Graphs

| Shape | Container | Why not | Code |
| --- | --- | --- | --- |
| Adj list, dense 0..n-1 | `vector<vector<int>>` | `map<int, vector>` | `12`, `36` |
| Weighted adj | `vector<vector<pair<int,int>>>` | — | `36` |
| Named nodes, sparse ids | `unordered_map<K, vector<K>>` | `vector` needs compress first | — |
| Unweighted shortest | `queue` + dist `vector` | heap | `12` |
| Positive weighted shortest | lazy Dijkstra heap | `queue` | `36` |
| 0-1 BFS | `deque` (0-weight front, 1-weight back) | heap | `containers/deque.md` |
| Topological order | `queue` + indegree `vector` (Kahn) | — | — |
| MST | heap (Prim) or `sort` edges + DSU | — | — |

## G. Strings, search, bits

| Shape | Container | Why not | Code |
| --- | --- | --- | --- |
| Inverted index AND query | hash term → sorted id `vector`; `set_intersection` | `set` per term also works, slower | `38_recipe_inverted_index` |
| Trie / nested directories | `array<26>` children or nested `map` | flat hash of full paths for `ls` | `47`, `59` |
| Packed flags, sieve of Eratosthenes N compile-time | `bitset<N>` | `vector<bool>` | `19_bitset_flags` |
| Runtime-n bits | `vector<char>` / `vector<uint64_t>` | `vector<bool>` | `07_pitfalls` |
| Hamming / popcount | `bitset` `count` of XOR | loop of bits | `19` |

## H. Design problems (O(1) / O(log n) on several verbs)

| Shape | Containers | Axes | Code |
| --- | --- | --- | --- |
| LRU cache | `list` + `unordered_map` of iterators | key, recency | `03`, `30` |
| LFU cache | hash + `list` per frequency | key, freq, LRU-within-freq | `46` |
| TTL + LRU | list + pos hash + expiry `set` | key, recency, time | `52` |
| Leaderboard | hash id→score + `set{-score,id}` | id, rank | `40` |
| Randomized set | `vector` + index map | value, dense index | `41` |
| Time-based KV | hash of append-only sorted vectors | key, time (binary search) | `42` |
| News feed | follow graph + per-user logs + merge heap | graph, time | `43` |
| Movie rental | price hash + available `set` + rented `set` | copy, cheapest available, cheapest rented | `50` |
| MK average | `queue` + 3 `multiset`s | FIFO window, trimmed order stats | `51` |
| Order book | bid/ask `map<price, list>` + id locator | price, FIFO, cancel-by-id | `53` |
| Dinner plates | `vector<stack>` + `set` of holes | leftmost non-full, popAt(i) | `54` |
| First unique | `list` + pos hash + dead set | arrival, uniqueness | `55` |
| Delay queue | `map<time, list>` + id locator | time, cancel | `56` |
| Stock tracker | `map<time,price>` + `multiset` prices | latest time, min/max, correct a time | `48` |
| Many-to-many tags | two hashes of `set`s | item→tags, tag→items | `58` |
| In-memory FS | nested `map` trie | path, alpha `ls` | `59` |

**Destroy that forgets an axis is the bug.** Start from
[`recipes/three_plus.md`](recipes/three_plus.md).

## I. Algorithms that *replace* a container choice

Sometimes the right answer is "sorted `vector` + algorithm",
not a tree.

| Shape | Do this | Instead of |
| --- | --- | --- |
| Build once, many membership queries | `sort` + `binary_search` | `set` if no online inserts |
| A ∩ B of two static lists | `set_intersection` | putting both in a `set` |
| Select k-th | `nth_element` | `sort` or a heap of n |
| Materialize a filter | `copy_if` + `back_inserter` | hand-grown loop **or** a view you store |
| Permute all orderings | `next_permutation` from sorted | recursion if you just need the generator |

## J. Concurrency (STL will not do this for you)

| Shape | Do this |
| --- | --- |
| Many readers, rare writer | `shared_mutex` around the container, or `shared_ptr<const Map>` copy-on-write |
| Many writers counting | thread-local `unordered_map` + merge |
| Producer/consumer queue | `deque` + `mutex` + `condition_variable` |
| Truly concurrent hashmap | TBB / `concurrent_hash_map` / your platform — **not** `std::unordered_map` |

No STL container is thread-safe for mixed read/write.
[`11_threads_exceptions.md`](11_threads_exceptions.md).

---

## Quick "I heard X, I open Y"

| I heard | I open |
| --- | --- |
| Two Sum, anagrams, subarray sum k | `recipes/two_sum_and_anagrams.md` |
| LRU / LFU | `recipes/lru_cache.md`, `lfu_cache.md` |
| Top-k, heap vs buckets | `recipes/top_k_frequent.md` |
| Window, substring, window max | `recipes/sliding_window.md` |
| Median stream | `recipes/median_stream.md` |
| Intervals, meeting rooms | `recipes/merge_intervals.md` |
| BFS, Dijkstra | `recipes/graph_bfs_dijkstra.md` |
| Next greater, daily temps | `recipes/monotonic_stack_queue.md` |
| Leaderboard, heap-can't-erase | `recipes/leaderboard.md` |
| getRandom | `recipes/randomized_set.md` |
| Cancel + due time | `recipes/delayed_queue.md` |
| Several O(log n) lookups | `recipes/three_plus.md` |
