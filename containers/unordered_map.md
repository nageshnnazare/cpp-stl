# `std::unordered_map` — the default key/value table

Hash map. Average O(1) `find` / `insert`. No key order.
This is the container behind Two Sum, graphs with sparse ids,
memoization, and frequency counts.

```
  word frequency

  "the"  --hash-->  bucket 3  --> ["the", 4]
  "a"    --hash-->  bucket 0  --> ["a", 2] --> ["an", 1]
```

Header: `<unordered_map>`. Same `operator[]` trap as `map`:
`m[k]` inserts a default `T`. Prefer `find` / `try_emplace` /
`insert_or_assign`.

## CRUD

| | Methods | Avg | Notes |
| --- | --- | --- | --- |
| C | `m[k]=`, `insert`, `emplace`, `try_emplace` | O(1) | `reserve(n)` first |
| R | `find`, `at`, `contains` | O(1) | `at` throws |
| U | `it->second =`, `insert_or_assign` | O(1) | key is const |
| D | `erase` | O(1) | |

`bucket_count`, `load_factor`, `max_load_factor`, `rehash` exist
when you are tuning. For learning: `reserve` and a good hash.

## Job 1 — frequency table

```cpp
std::unordered_map<std::string, int> freq;
for (auto& w : words) ++freq[w];     // [] default 0, then increment
```

Here `[]` is the right tool: "insert 0 if new, then add one" **is**
the Create-or-Update you want.

**Why not `map`?** You do not need alphabetical order to count.
**Why not `multiset`?** Counting copies as separate nodes is
heavier than one node + an `int`.

## Job 2 — Two Sum / complement index

```
  nums:  [ 2, 7, 11, 15 ]   target 9

  i=0  seen: { 2 -> 0 }
  i=1  need 2, found at 0    => (0, 1)
```

```cpp
std::unordered_map<int, int> last;  // value -> index
for (int i = 0; i < n; ++i) {
    if (auto it = last.find(target - nums[i]); it != last.end())
        return {it->second, i};
    last[nums[i]] = i;
}
```

**Why not two loops?** O(n²). **Why not `sort` + two pointers?**
That loses original indices unless you store pairs; it is a valid
alternative when you do not need indices (3Sum family).

## Job 3 — adjacency list with non-int nodes

```
  "A" -> "B", "C"
  "B" -> "D"

  unordered_map<string, vector<string>> g;
```

Dense `0..n-1` nodes: `vector<vector<int>>`. Strings or sparse
ints: hash map.

## Job 4 — memoization

```
  fib(n):  if (auto it = memo.find(n); it != memo.end()) return it->second;
           return memo[n] = fib(n-1) + fib(n-2);
```

For `int` keys in `0..n`, a `vector<int> memo(n+1, -1)` is faster.

## Why this, not that

| Alternative | Prefer unordered_map | Prefer the other |
| --- | --- | --- |
| `map` | no order, hotter lookups | predecessor, ordered walk |
| `vector<T>` | sparse / non-int keys | dense index |
| `unordered_multimap` | — | really several values per key; often `map<K,vector<V>>` is clearer |

Hashing `vector<int>` or `pair` as a key: you must provide a
hasher. A common pattern for anagrams is to use a **sorted
string** or `array<int,26>` as the key (array needs a hasher too;
sorted string uses `std::hash<string>`).

## Related problems

- Two Sum, 4Sum II, Group Anagrams, Subarray Sum Equals K
- Clone graph, course schedule (graph as map)
- LRU **value** side (`unordered_map` + `list`)
- Word pattern / isomorphic strings (two maps, or map + set)
- Time-based store if you hash the key and keep a **sorted
  vector** of (time, value) as the mapped type — two containers

Run [`examples/10_unordered_map_freq.cpp`](../examples/10_unordered_map_freq.cpp).
