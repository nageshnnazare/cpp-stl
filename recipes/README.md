# Recipes: more than one container

A single container is a noun. A *design* is two or three nouns
that cover each other's missing CRUD.

```
  hash  (O(1) by key, no order)
    +
  linked list / heap / sorted vector / deque
    =
  a structure the STL does not ship under one name
```

| Recipe | Types | Problem shape |
| --- | --- | --- |
| [`lru_cache.md`](lru_cache.md) | `list` + `unordered_map` | O(1) get/put + recency |
| [`two_sum_and_anagrams.md`](two_sum_and_anagrams.md) | `unordered_map` + `vector` / `array` | complement, grouping |
| [`top_k_frequent.md`](top_k_frequent.md) | `unordered_map` + heap or `bucket` | count then select |
| [`sliding_window.md`](sliding_window.md) | `deque` / `unordered_map` / `array` | subarray/substring constraint |
| [`median_stream.md`](median_stream.md) | two heaps or two `multiset`s | running median |
| [`merge_intervals.md`](merge_intervals.md) | `vector` + `sort` / `map` sweep | overlaps |
| [`graph_bfs_dijkstra.md`](graph_bfs_dijkstra.md) | `vector` + `queue` / `priority_queue` | graphs |
| [`monotonic_stack_queue.md`](monotonic_stack_queue.md) | `stack` / `deque` + `vector` | next greater, window max |
| [`inverted_index.md`](inverted_index.md) | `unordered_map` + `set`/`vector` | search, posting lists |
| [`task_scheduler.md`](task_scheduler.md) | heap + `queue` | cooldown / rearrange |
| [`leaderboard.md`](leaderboard.md) | `unordered_map` + `set{-score,id}` | online top-K scores |
| [`randomized_set.md`](randomized_set.md) | `vector` + index map | insert/delete/getRandom |
| [`time_based_kv.md`](time_based_kv.md) | hash of sorted `vector`s | value as-of time |
| [`news_feed.md`](news_feed.md) | graph + logs + heap | Twitter-style feed |
| [`min_stack.md`](min_stack.md) | two stacks | O(1) min |
| [`hit_counter.md`](hit_counter.md) | `queue` of timestamps | last-N-seconds count |
| [`lfu_cache.md`](lfu_cache.md) | hash + freq `list`s | O(1) LFU |
| [`autocomplete.md`](autocomplete.md) | `map` or trie | prefix top-k |
| [`stock_tracker.md`](stock_tracker.md) | `map` + `multiset` | current / min / max price |
| [`freq_stack.md`](freq_stack.md) | freq map + stack per f | pop most frequent |
| [`three_plus.md`](three_plus.md) | **3+ indexes** | CRUD on several axes |
| [`movie_rental.md`](movie_rental.md) | price hash + 2 ordered `set`s | search / rent / drop / report |
| [`mk_average.md`](mk_average.md) | `queue` + 3 `multiset`s | windowed trimmed mean |
| [`ttl_cache.md`](ttl_cache.md) | LRU list + hash + expiry `set` | get/put with TTL |
| [`order_book.md`](order_book.md) | bid/ask maps + id → list it | match / cancel |
| [`dinner_plates.md`](dinner_plates.md) | `vector<stack>` + hole `set` | leftmost push, popAt |
| [`first_unique.md`](first_unique.md) | `list` + pos hash + dead `set` | first unique in a stream |
| [`delayed_queue.md`](delayed_queue.md) | time `map` of lists + id hash | schedule / cancel / due |
| [`max_stack.md`](max_stack.md) | `list` + `map` of iterators | popMax of a buried value |
| [`many_to_many.md`](many_to_many.md) | two hashes of `set`s | tags; destroy both sides |
| [`file_system.md`](file_system.md) | nested `map` trie | mkdir / ls / write / rm |

Two containers cover "hash + order". **Destroy that forgets
one index is the bug** in the 3+ group — start at
[`three_plus.md`](three_plus.md). The related problems at the
bottom of each file are the same diagram with different field
names.

Given a *new* design prompt, decode with
[`14_read_a_problem.md`](../14_read_a_problem.md) (count the
axes first). Catalog of shapes:
[`problem_catalog.md`](../problem_catalog.md) section H.
