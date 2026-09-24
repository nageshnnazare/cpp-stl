# Partition — a linear rearrange, not a sort

Header: `<algorithm>`. Split a range into **pred true** then
**pred false**. Unstable `partition` is O(n) extra O(1).
`stable_partition` keeps order inside each side and typically
uses O(n) extra memory.

```
  pred: even
  [ 7 | 2 | 5 | 8 | 4 | 1 ]

  partition (one possible):
  [ 2 | 8 | 4 | 7 | 5 | 1 ]
                ^ returned iterator = first false

  stable_partition:
  [ 2 | 8 | 4 | 7 | 5 | 1 ]
    evens in original order, odds in original order
```

| Algorithm | Time | Extra | Stable |
| --- | --- | --- | --- |
| `partition` | O(n) | O(1) | no |
| `stable_partition` | O(n) | O(n) | yes |
| `partition_copy` | O(n) | two outputs | yes (by construction) |
| `is_partitioned` | O(n) | O(1) | — |
| `partition_point` | O(log n) | O(1) | range **must** already be partitioned; random access |

## Job 1 — Dutch / "odds and evens" / move zeros

```cpp
auto mid = std::partition(v.begin(), v.end(),
                          [](int x){ return x != 0; });
// nonzeros | zeros     relative order of nonzeros NOT kept
```

If order of nonzeros matters: `stable_partition`, or two-pointer
write index (the usual interview move-zeroes).

**Why not `sort` with a comparator "nonzero first"?** O(n log n)
and you fight strict-weak-ordering (zeros equivalent). Partition
is the spec.

## Job 2 — quickselect's cousin

`nth_element` **is** a partition around a pivot, repeated. If
you only need "≤ pivot | ≥ pivot" once, call `partition` (or
`std::ranges::partition`).

## Job 3 — `partition_point` as binary search

After you already partitioned, or after you built a range that
is `[ok, ok, ..., bad, bad]`:

```cpp
auto it = std::partition_point(v.begin(), v.end(),
                               [](int x){ return x < 10; });
```

This is `lower_bound` with a predicate. Same O(log n).

## Why this, not that

| Need | Tool |
| --- | --- |
| two groups, order inside irrelevant | `partition` |
| two groups, keep relative order | `stable_partition` or `partition_copy` |
| three groups (0,1,2) | three-way partition loop, not two `partition`s (the second messes the first) unless you partition 0s then 1s on the right slice carefully |
| full order | `sort` |

## Related problems

- Sort colors (75), Partition labels (763) is a different
  "partition" (greedy intervals)
- Wiggle sort (partition around median)
- Quicksort's partition step
- Stable split of a linked list (`list` + two output lists,
  `partition_copy` thinking)

Run [`examples/25_algo_partition.cpp`](../examples/25_algo_partition.cpp).
