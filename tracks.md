# Tracks: beginner, interview, expert

Same repo, three routes. Do not read every file in order
unless you are writing a compiler.

The **rulebook** on every track:
[`14_read_a_problem.md`](14_read_a_problem.md),
[`rules.md`](rules.md),
[`cheat_sheet.md`](cheat_sheet.md),
[`problem_catalog.md`](problem_catalog.md).

---

## Track A — weekend beginner (6–8 hours)

Goal: grow a `vector`, count with `unordered_map`, `sort`,
not `erase` in a range-for, default to `vector`.

```
  1. 00_beginner.md                         + example 80
  2. 00_mental_model.md                     pictures
  3. cheat_sheet.md                         keep this open
  4. containers/vector.md                   + example 01
  5. containers/unordered_map.md            + example 10
  6. 02_decision_guide.md                   picker + erase patterns
  7. 07_pitfalls.md                         map[], remove, vector<bool>
  8. recipes/two_sum_and_anagrams.md        + example 31
  9. 14_read_a_problem.md                   ASK / ARRIVE / EXTRA; decode 0–3
 10. exercises.md  1, 4, 8, 9
```

Done when you can say why Two Sum is a hash map and why
`m[k]` is the wrong membership test.

---

## Track B — interview / contest (about two weeks)

Goal: decode a problem into containers in two minutes;
write LRU, top-k, window max, BFS, monotonic stack without
looking up the *shape*.

After Track A:

```
  Day 1    01_iterators.md, 03_iterator_adapters.md, example 00, 15
  Day 2    vector, deque, stack, queue, pq     examples 01–02, 11–13
  Day 3    set, map, unordered_*               examples 07–10
  Day 4    list + LRU                          03, 30, recipes/lru_cache
  Day 5    algorithms: modify, sort, binary    20–23; 07_pitfalls unique
  Day 6    14_read_a_problem.md worked 1–12
           problem_catalog.md A–E
  Day 7    recipes: sliding_window, top_k, median, intervals, graph
  Day 8    monotonic, two_sum, inverted_index, scheduler
  Day 9    leaderboard, randomized_set, min_stack, hit_counter
  Day 10   three_plus.md + one of movie / delay / first_unique
  Day 11   04_insert_emplace, 05_hash, 08_types
  Day 12   capstone.md + example 70; exercises.md all
```

Done when destroy-must-update-every-index is automatic, and
you refuse `priority_queue` the moment the problem says
"reset this id."

---

## Track C — production expert (ongoing)

Goal: quote the standard's invalidation and exception
rules; know when to leave `std::`.

After Track B:

```
  09_memory_reserve_pmr.md + examples 61, 63
  11_threads_exceptions.md
  12_debug_measure.md
  13_parallel_and_cpp23.md
  15_expert.md                         guarantees vs folklore
  16_supporting_types.md               pair, optional, variant
  06_ranges.md                         views do not own
  recipes/three_plus.md                rest of 50–59
  rules.md R57–R80
```

Done when the review checklist in `15_expert.md` is muscle
memory.

---

## "I have 20 minutes before a screen-share"

```
  cheat_sheet.md
  14_read_a_problem.md   English table + one worked decode
  02_decision_guide.md   erase patterns 1–3
```

## "I got a sanitizer hit"

```
  07_pitfalls.md
  02_decision_guide.md   invalidation / erase
  15_expert.md           stability table
```

## "Which file is the bible's front door?"

[`README.md`](README.md). This page is how to *walk* it.
