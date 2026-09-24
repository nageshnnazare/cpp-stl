# Examples

Every `.cpp` here is a complete program: headers, `main`, printed
checks. The banner at the top of each file is the memory aid:

```
WHAT / WHY / THREAD / ERASE / REMEMBER
```

`THREAD` is the one-line rule for that container (const reads vs
mutex). `ERASE` is why `for (x : c) c.erase(...)` is undefined
and which legal pattern replaces it. The full table lives in
[`02_decision_guide.md`](../02_decision_guide.md).

From `cpp-stl/`:

```bash
make                 # build all into build/
make test            # run all
c++ -std=c++20 examples/01_vector_timeline.cpp -o /tmp/v && /tmp/v
```

| File | Chapter |
| --- | --- |
| `00_iterator_categories.cpp` | iterators |
| `01_vector_timeline.cpp` | vector |
| `02_deque_sliding_window.cpp` | deque |
| `03_list_lru.cpp` | list |
| `04_forward_list_chain.cpp` | forward_list |
| `05_array_histogram.cpp` | array |
| `06_string_split_window.cpp` | string |
| `07_set_online.cpp` | set |
| `08_map_timeline.cpp` | map |
| `09_unordered_set_seen.cpp` | unordered_set |
| `10_unordered_map_freq.cpp` | unordered_map |
| `11_stack_parens_next_greater.cpp` | stack |
| `12_queue_bfs.cpp` | queue |
| `13_priority_queue_topk.cpp` | priority_queue |
| `14_span_api.cpp` | span |
| `15_iterator_adapters.cpp` | reverse / inserter / move |
| `16_emplace_extract.cpp` | try_emplace, node handle, merge |
| `17_hash_transparent.cpp` | custom hash, transparent set |
| `18_ranges.cpp` | C++20 views + projections |
| `19_bitset_flags.cpp` | bitset flags / sieve |
| `20`–`28_algo_*.cpp` | algorithms |
| `30`–`39_recipe_*.cpp` | multi-container recipes |
| `40_recipe_leaderboard.cpp` | leaderboard |
| `41_recipe_randomized_set.cpp` | insert/delete/getRandom |
| `42_recipe_time_kv.cpp` | time-based KV |
| `43_recipe_news_feed.cpp` | news feed |
| `44_recipe_min_stack.cpp` | min stack |
| `45_recipe_hit_counter.cpp` | hit counter |
| `46_recipe_lfu.cpp` | LFU cache |
| `47_recipe_autocomplete.cpp` | autocomplete |
| `48_recipe_stock_tracker.cpp` | stock tracker |
| `49_recipe_freq_stack.cpp` | freq stack |
| `50_recipe_movie_rental.cpp` | movie rental (3 indexes) |
| `51_recipe_mk_average.cpp` | MK average (4 bags) |
| `52_recipe_ttl_cache.cpp` | TTL LRU |
| `53_recipe_order_book.cpp` | order book |
| `54_recipe_dinner_plates.cpp` | dinner plates |
| `55_recipe_first_unique.cpp` | first unique |
| `56_recipe_delayed_queue.cpp` | delay queue |
| `57_recipe_max_stack.cpp` | max stack |
| `58_recipe_many_to_many.cpp` | dual tag index |
| `59_recipe_file_system.cpp` | nested map FS |
| `60_stl_friendly_type.cpp` | `T` in set / unordered_set |
| `61_reserve_pmr.cpp` | reserve + PMR arena |
| `62_predicates.cpp` | greater, not_fn, projection |
| `63_measure_reserve.cpp` | time reserve vs not |
| `70_capstone_logs.cpp` | count + top-k + window |
| `80_beginner_hello.cpp` | first hour: vector + count map |
| `81_decode_two_sum.cpp` | ASK / ARRIVE / EXTRA on Two Sum |
