# MK average — a window plus three bags

**Need:** stream of numbers. After each insert, if you have at
least `m` values, drop the **smallest k** and **largest k** of
the last `m`, and return the average of the **middle**
(truncated). Insert and expire the oldest in O(log m).

```
  m=5  k=1     window (queue, oldest left)
  [ 9 | 3 | 8 | 1 | 7 ]
    drop 1 (smallest k) and 9 (largest k)
    mid = {3,8,7}   avg = 6
```

The queue knows **age**. It cannot find "the 1" to delete from
"smallest" without help. Three `multiset`s partition value:

```
  lo   (k smallest)     mid (m-2k)     hi  (k largest)
  {1}                    {3,7,8}        {9}

  expire 9 (front of queue):  9 sits in hi, hi.erase(find(9))
  then steal from mid to refill hi
```

Four containers: `queue` + `lo` + `mid` + `hi` (+ a running
`mid` sum so fetch is O(1)).

## Why four, not `sort` the window

| Design | add + expire | query | Verdict |
| --- | --- | --- | --- |
| keep `vector`, sort each query | O(1) | O(m log m) | |
| one `multiset` of the window | O(log m) | O(m) walk to skip k | |
| **queue + 3 multisets** | O(log m) | O(1) | **the one** (1825) |
| Fenwick on compressed values | O(log U) | O(log U) | overkill for interviews |

A `priority_queue` cannot expire an arbitrary oldest value
unless you lazy-delete with a second structure — at which
point you have reinvented the multisets.

## Invariant

```
  lo.size()  == min(k, n)           smallest
  hi.size()  == min(k, max(0, n-k))
  mid        holds the rest
  max(lo) ≤ min(mid) ≤ max(mid) ≤ min(hi)
```

Insert always lands in one bag (compare with the pivots), then
**size-repair** moves one element across a boundary. Expire
`find`s in `lo`, else `mid`, else `hi` (`multiset::find` is
O(log m); `erase(value)` would wipe duplicates).

Full program: [`examples/51_recipe_mk_average.cpp`](../examples/51_recipe_mk_average.cpp).

## Related problems

- Finding MK Average (1825)
- Sliding Window Median (480) — two bags, not three;
  [`median_stream.md`](median_stream.md)
- Sliding window mean without dropping extremes — `queue` +
  running sum only ([`hit_counter.md`](hit_counter.md) shape)
- [`stock_tracker.md`](stock_tracker.md) — two trees, no window
  expiry
