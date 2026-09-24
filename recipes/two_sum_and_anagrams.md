# Two Sum, grouping, prefix-sum maps

These problems are "I have a value, I need its **partner**
fast." The partner lives in an `unordered_map`. The sequence
itself stays a `vector`.

```
  Two Sum
  i walks the vector
  partner = target - nums[i]
  hash: value -> index of a previous i

  [ 2, 7, 11, 15 ]  target 9
         ^
         need 2, hash has 2@0
```

## Why `unordered_map`, not `map` or sort

| Approach | Time | Indices? | Notes |
| --- | --- | --- | --- |
| two loops | O(n²) | yes | |
| **hash value→index** | avg O(n) | yes | default |
| `map` | O(n log n) | yes | no need for order |
| sort + two pointers | O(n log n) | need extra index field | good for 3Sum (avoid dups) |

3Sum sorts first because the inner loop is two-pointers **and**
you must skip duplicates in order. Two Sum does not need that.

## Group anagrams — map from signature → list

```
  "eat","tea","tan","ate","nat","bat"

  signature = sort the letters   ("aet") -> ["eat","tea","ate"]
              or array<int,26> counts

  unordered_map<string, vector<string>> groups;
```

**Why sorted string as key?** `std::hash<string>` exists.
**Why `array<int,26>`?** No allocation per word; you must
write a hasher (or pack counts into a string). **Why not
`map`?** You iterate groups, not keys in alpha order.

## Subarray sum equals k — prefix + hash

```
  pref[i] = a[0] + ... + a[i]
  want pref[r] - pref[l-1] == k
  => how many previous prefixes == pref[r] - k

  unordered_map<long long, int> freq;  freq[0] = 1;  // empty prefix
```

```
  a:     1  1  1     k=2
  pref:  1  2  3
  at 2:  need 0  (one empty) + need 1 (one)  => 2 subarrays
```

The `vector` stores `a` (optional once you stream). The map
stores **counts of prefixes**. A `map` would be O(log n) per
step for no benefit — prefixes are not queried by range here.
(If they were, Fenwick / `map` of prefixes.)

## Related problems

- Two Sum (1), Two Sum II sorted (two pointers, **no hash**)
- 4Sum II (hash one half, O(n²))
- Group Anagrams (49), Valid Anagram (26-count `array`)
- Subarray Sum Equals K (560), Contiguous Array (525)
- Longest Consecutive Sequence (hash set, not map)
- Isomorphic strings / Word pattern (two maps, or map+set)
- Clone graph (map old node* → new node*)

Run [`examples/31_recipe_two_sum_anagrams.cpp`](../examples/31_recipe_two_sum_anagrams.cpp).
