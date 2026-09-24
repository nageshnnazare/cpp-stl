# Binary search on a sorted (or partitioned) range

Header: `<algorithm>`. **Random-access** for O(log n) iterator
moves. On a `set`, use the **member**.

```
  sorted:  [ 1 | 3 | 3 | 7 | 9 | 12 ]
  lower_bound(3)  -> first 3          index 1
  upper_bound(3)  -> 7                index 3
  equal_range(3)  -> [1, 3)
  binary_search(3) -> true            exists, not the position

  lower_bound(4)  -> 7                insertion point
  lower_bound(13) -> end()
```

All of them need the same order you will query with: if you
sorted with `cmp`, pass `cmp`.

Time: O(log n) comparisons. On random-access, O(log n) hops.
On bidirectional (`list`, `set` via `std::lower_bound`), hops
are O(n).

## The four calls

| Call | Meaning | Use |
| --- | --- | --- |
| `lower_bound(x)` | first element **≥ x** | insert point, first true in `[false..., true...]` |
| `upper_bound(x)` | first element **> x** | end of a run of x |
| `equal_range(x)` | `[lower, upper)` | all copies of x |
| `binary_search(x)` | exists? | when you do not need the iterator |

```cpp
auto it = std::lower_bound(v.begin(), v.end(), x);
bool found = (it != v.end() && *it == x);
v.insert(it, x);                     // keep sorted, O(n) to shift
```

## Job 1 — predicate binary search ("first true")

The STL searchers compare **values**. Many problems compare a
**yes/no** that becomes true and stays true.

```
  "minimum capacity to ship in D days"
  capacity  1  2  3  4  5  6  7  8  9 10 ...
  feasible  n  n  n  n  n  y  y  y  y  y

  lo, hi on capacity; mid feasible? search left else right
  that is lower_bound on the implicit boolean array
```

You can implement it with `std::partition_point` on a counting
iterator, or write the `lo/hi` loop. The **invariant** is the
same as `lower_bound`.

```
  [ F F F F T T T ]
            ^ partition_point / lower_bound of first T
```

## Job 2 — on a `map` / `set`, members only

```cpp
auto it = m.lower_bound(t);          // O(log n) tree
if (it == m.begin()) { /* no predecessor */ }
else {
    auto pred = std::prev(it);       // O(1)
}
```

`std::lower_bound(m.begin(), m.end(), t, cmp)` compiles and is
the slow bug.

## Job 3 — sorted vector as a cheap set

Build once: `sort` + maybe `unique`. Query: `binary_search`.
Range count of values in `[L, R]`:

```
  count = upper_bound(R) - lower_bound(L)
          (random-access iterator difference)
```

**Why not `map`?** Static data: vector wins cache. Dynamic
inserts: map wins.

## Critical thinking

| Situation | Tool |
| --- | --- |
| sorted `vector` / `array` / `deque` | `std::lower_bound` |
| `set` / `map` | member `lower_bound` |
| unsorted | `find` or hash; binary search is wrong |
| "min x such that f(x) is true", f monotonic | binary search on x, not on a container |
| many queries, static ints in 0..1e9 | compress + Fenwick, or just `lower_bound` |

Off-by-one: `upper_bound(x) - 1` is the last element **≤ x**
(if it exists). Draw the array; do not memorize `±1`.

## Related problems

- Binary Search (704), Search Insert Position (35)
- First/last position in sorted array (`equal_range`)
- Koko eating bananas, ship packages in D days, split array
  largest sum (binary search the **answer**)
- Time-based key-value (`upper_bound` on times)
- Count of range sum (sorted prefix + `lower_bound`)

Run [`examples/23_algo_binary_search.cpp`](../examples/23_algo_binary_search.cpp).
