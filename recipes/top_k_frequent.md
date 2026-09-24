# Top-k frequent — count, then select

**Need:** k most frequent elements. Two phases, two shapes.

```
  words:  apple apple banana apple pear pear

  phase 1  unordered_map
           apple->3  banana->1  pear->2

  phase 2  pick k
           either a min-heap of size k
           or bucket sort on frequency (freq ≤ n)
```

## Phase 1 is not a debate

`unordered_map<T,int>`. `map` if you must later print remaining
keys sorted. `multiset` would store duplicate objects, not
counts.

## Phase 2: three honest options

```
  n = number of unique keys,  k = how many you want
  F = max frequency  (F ≤ original length N)
```

| Method | Time after counting | Extra | When |
| --- | --- | --- | --- |
| `sort` pairs by freq | O(n log n) | vector of pairs | n small, k ≈ n |
| min-heap size k | O(n log k) | heap | k << n, or stream of uniques |
| `nth_element` on pairs | avg O(n) | vector | k-th cutoff, then maybe sort the k |
| **bucket `vector<vector<T>>` indexed by freq** | O(N + n) | O(N) buckets | frequencies are ints in `1..N` |

```
  bucket sort (LeetCode 347 favorite):

  freq  1        2         3
        banana   pear      apple

  walk freq = N .. 1 collecting until k items
```

**Why a min-heap not a max-heap of everything?** You only keep
k. A max-heap of n is O(n) build then k pops — also fine
(`make_heap` on all pairs). Size-k min-heap is the streaming
version.

**Why not `multiset` of pairs?** Works (O(n log n) or log-k if
you cap size). Heavier nodes.

## Sketch (heap)

```cpp
std::unordered_map<int, int> cnt;
for (int x : nums) ++cnt[x];

using P = std::pair<int, int>; // freq, value
std::priority_queue<P, std::vector<P>, std::greater<P>> h;
for (auto [val, c] : cnt) {
    h.push({c, val});
    if ((int)h.size() > k) h.pop();
}
```

Full: [`examples/32_recipe_topk.cpp`](../examples/32_recipe_topk.cpp).

## Related problems

- Top K Frequent Elements (347), Top K Frequent Words (692)
  (692: heap with a comparator on freq then lexicographic)
- K Closest Points to Origin (973) — no count map, just heap
  or `nth_element` on distance
- Sort characters by frequency (451) — bucket or heap
- Reorganize string (767) — max-heap of counts
- Ugly number II — min-heap of candidates (different recipe)

If k == n, skip selection: dump the map into a vector and
`sort`. The heap is not "always more professional."
