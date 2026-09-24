# Intervals — sort a `vector`, sometimes sweep a `map`

An interval is a pair. The default container is
`vector<pair<int,int>>`. The algorithm is **sort by start**,
then a linear merge. A tree (`map`) appears when intervals
arrive **online** or you need overlap counts at every point.

```
  [1,4)  [2,5)  [7,9)  [8,10)

  sort by start:
  [1,4) [2,5) [7,9) [8,10)

  merge if next.start < cur.end:
  [1,5) [7,10)
```

## Why `vector` + `sort`, not `set`

You have all intervals up front. Sort is O(n log n) once; then
O(n). Inserting into a `set<pair>` is also O(n log n) but with
node allocation, and merge is clumsier (`erase` while iterating).

`set` wins for **My Calendar**: each `book` must fail if overlap,
and the set stays. `lower_bound(start)` + check previous.

## Sweep line when you need "how many overlap here"

```
  +1 at start,  -1 at end   (half-open [s,e))

  map<int,int> delta;
  for ([s,e) : ints) { delta[s]++; delta[e]--; }

  walk delta in key order, cur += d
  max(cur) = max concurrent  (meeting rooms II)
```

```
  time:   0    1    2    3    4    5
  A:      [---------)
  B:           [--------------)
  delta: +1   +1         -1        -1
  cur:    1    2          1         0
```

**Why `map` not `unordered_map`?** You must walk times in
order. **Why not `vector` of events + `sort`?** That is the
same algorithm and usually **faster** (one sort, contiguous).
Use `map` when keys are sparse and you merge duplicate times
automatically (`delta[t] +=`).

Meeting rooms II alternative: sort starts, sort ends, two
pointers — or min-heap of end times. Three designs, one
problem.

## Related problems

- Merge Intervals (56), Insert Interval (57)
- Meeting Rooms I/II (252/253)
- Non-overlapping intervals (greedy sort by end)
- My Calendar I/II/III (`map` deltas or `set` of pairs)
- Car pooling (sweep on trip stops)
- Range module (map of disjoint segments, split on add/remove)
- Video stitching / jump game II (sort + greedy cover)

Run [`examples/35_recipe_intervals.cpp`](../examples/35_recipe_intervals.cpp).
