# Set algorithms — linear merge of two **sorted** ranges

Header: `<algorithm>`. Both inputs must be sorted with the same
order. Time O(n+m). Extra space O(1) besides the output.

```
  A:  [ 1 | 3 | 3 | 5 | 8 ]
  B:  [ 3 | 4 | 8 | 9 ]

  merge           [ 1 3 3 3 4 5 8 8 9 ]
  set_union       [ 1 3 3 4 5 8 9 ]      (keeps one run of dups from A/B
                                         per unique; see standard for
                                         exact duplicate rules)
  set_intersection[ 3 8 ]
  set_difference  [ 1 3 5 ]              A minus B
  set_symmetric   [ 1 3 4 5 9 ]
  includes        B ⊆ A ?   false
```

Exact duplicate policy: they walk like `merge` and emit according
to the operation. For unique-set semantics, `unique` both inputs
first, or use `std::set`.

| Algorithm | Meaning | Output size |
| --- | --- | --- |
| `merge` | stable concatenate of two sorted | n+m |
| `inplace_merge` | merge `[first,mid)` with `[mid,last)` | n, extra O(n) typical |
| `set_union` | A ∪ B | ≤ n+m |
| `set_intersection` | A ∩ B | ≤ min(n,m) |
| `set_difference` | A − B | ≤ n |
| `set_symmetric_difference` | (A−B) ∪ (B−A) | ≤ n+m |
| `includes` | every of B appears in A | bool |

Write into `back_inserter(out)` or a pre-sized buffer.

## Job 1 — intersection of two arrays

```
  sort both, set_intersection into a vector
  or: dump the smaller into unordered_set, scan the larger
```

| | Sort + `set_intersection` | Hash set |
| --- | --- | --- |
| Time | O(n log n + m log m) | avg O(n+m) |
| Space | O(1) extra if in-place sort | O(min(n,m)) |
| Order | sorted output | hash order unless you restore |
| Need sorted anyway | **yes, this** | overkill |

Interview "intersection of two arrays II" (bags): hash
*counts*, or sort + two pointers (what `set_intersection` is).

## Job 2 — `inplace_merge` after sorting halves

```
  [ sorted left | sorted right ]
  inplace_merge(first, mid, last)
```

This is mergesort's combine. Also: "sort a vector that is two
sorted runs" (an almost-sorted log file).

## Job 3 — union of sorted unique ids

Two friend lists, already sorted: `set_union`. Building two
`set<int>` and walking is the same asymptotics with worse
constants.

## Why this, not `set` / `unordered_set`

```
  Static arrays you can sort once
       ->  set_* algorithms on vectors     cache-friendly

  Online: insert, query "is in A∩B" forever
       ->  unordered_set for each, or a counting map

  Need predecessor of the union
       ->  set / map, not a one-shot merge
```

## Related problems

- Intersection / union of two arrays (349, 350)
- Merge two sorted lists (21) — same two-pointer as `merge`
- Merge k sorted lists — heap, not pairwise `merge` in a loop
  (that is O(k n); heap is O(n log k))
- `inplace_merge` in count-of-inversions mergesort

Run [`examples/26_algo_set_ops.cpp`](../examples/26_algo_set_ops.cpp).
