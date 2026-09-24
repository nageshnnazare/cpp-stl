# Median from a data stream — two heaps (or two multisets)

**Need:** after each insert, report the median in O(1). Insert
in O(log n).

A single `priority_queue` only gives you the extreme, not the
middle. A sorted `vector` insert is O(n). A `multiset` +
iterator at the median works (see below). The classic design is
**two heaps**:

```
  lower  (max-heap)        upper  (min-heap)
  ... smaller half ...     ... larger half ...

  n even:  1  3  5          7  9  11
           ^ top            ^ top
           median = avg of the two tops

  n odd:   1  3  5  8       9  11
                    ^
                    median = lower.top()   (keep lower bigger)
```

## Invariant

```
  all(lower) ≤ all(upper)
  size(lower) == size(upper)   or   size(lower) == size(upper)+1
```

Insert:

1. Push into `lower` (max-heap) by default, **or** compare with
   tops and send to the correct side.
2. If sizes violate, **move** `lower.top()` → `upper` or the
   reverse.

Move = pop one heap, push the other. O(log n).

## Why two heaps, not the rest

| Design | insert | median | erase arbitrary | Notes |
| --- | --- | --- | --- | --- |
| sort vector each time | O(n log n) | O(1) | O(n) | |
| keep vector sorted | O(n) | O(1) | O(n) | |
| **two heaps** | O(log n) | O(1) | hard | stream, no delete |
| two `multiset`s | O(log n) | O(1) | O(log n) | sliding-window median |
| one `multiset` + middle iterator | O(log n) | O(1) | O(log n) | carefully `++`/`--` middle |

Sliding window median (480) **must** erase the value leaving
the window → heaps need lazy deletion (tombstones + counts).
`multiset` erase is the straightforward code.

```
  two multisets lo, hi     // lo has the smaller half, rbegin = median side
  same size invariant
  erase(x):  if x in lo then lo.erase(lo.find(x)) else ...
             rebalance
```

Use `ms.erase(ms.find(x))` not `ms.erase(x)` (the latter wipes
all copies of x).

## Related problems

- Find Median from Data Stream (295)
- Sliding Window Median (480)
- IPO (502) — two heaps, different meaning (capital vs profit)
- Find right interval / running percentile (policy tree,
  Fenwick on compressed ranks — when you need k-th for changing k)

Run [`examples/34_recipe_median.cpp`](../examples/34_recipe_median.cpp).
