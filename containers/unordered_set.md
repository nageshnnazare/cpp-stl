# `std::unordered_set` — "have I seen this?" in average O(1)

A hash set. No order. The API is `set` without `lower_bound`.

```
  hash(x) % bucket_count

  0   1   2   3   4
  |   |   |   |   |
 []  [17] [] [3] [10]-->[24]     collision chain
```

Header: `<unordered_set>`. Iterator: **forward**.
Average insert/find/erase O(1); worst O(n). Call `reserve(n)`
before a known flood of inserts.

`unordered_multiset` allows duplicates (rarely what you want;
prefer `unordered_map<K,int>` counts).

## CRUD

| | Methods | Avg | Worst |
| --- | --- | --- | --- |
| C | `insert`, `emplace` | O(1) | O(n) |
| R | `find`, `contains`, `count` | O(1) | O(n) |
| U | erase + insert (keys are const) | O(1) | O(n) |
| D | `erase(k)`, `erase(it)` | O(1) | O(n) |

`count` is 0 or 1 on the unique set. Iteration order is junk —
never sort by walking an unordered_set; copy to a vector.

Custom keys need `operator==` and a hash. For `pair` / `tuple`
you write a hasher (or use Boost). Forgetting hash is why people
fall back to `set`.

## Job 1 — seen-set on a stream / a graph

```
  BFS / DFS:   unordered_set<int> seen;
  if (!seen.insert(v).second) continue;   // already there
```

`insert` returns `{iterator, bool}`. The bool **is** the test;
do not `count` then `insert` (two hashes).

**Why not `vector<bool> seen(n)`?** Use the vector when ids are
`0..n-1`. The hash set is for sparse ids, strings, pairs.

**Why not `set`?** If you never need order, hashing wins on
lookups once n is more than a few thousand (measure; small n
+ bad hash can lose).

## Job 2 — two containers: values vs complements

Contains-duplicate-II, longest consecutive sequence:

```
  consecutive: put all in a set
  for x in set:
      if x-1 not in set:          // x is a run start
          walk x, x+1, x+2, ...
```

```
  { 100, 4, 200, 1, 3, 2 }

  1 is a start (0 missing)   walk 1-2-3-4   length 4
  100, 200 are starts of 1
```

`unordered_set` makes `x-1 in set` O(1) average. A sorted
`vector` can do the same walk after `sort` + `unique` without a
hash — often faster. Both are valid; the hash version matches
the textbook "O(n)" argument.

## Why this, not that

| Alternative | Use unordered_set | Use the other |
| --- | --- | --- |
| `set` | membership only | min / successor / sorted walk |
| `unordered_map<K,bool>` | — | you need a payload |
| bitset / `vector<char>` | keys are huge or sparse | keys are `0..n` |

## Related problems

- Contains Duplicate I/II/III (III needs a sliding `set`)
- Longest Consecutive Sequence (128)
- Happy number (seen cycle)
- Walking a maze / word search (`set` of `pair<r,c>` or a
  flattened `visited` vector)
- Intersection of two arrays (`set` or sort + two pointers)

Run [`examples/09_unordered_set_seen.cpp`](../examples/09_unordered_set_seen.cpp).
