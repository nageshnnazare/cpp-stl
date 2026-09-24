# Permutations, min/max, clamp

Header: `<algorithm>`.

## Min / max

| Algorithm | On | Time |
| --- | --- | --- |
| `min` / `max` / `minmax` | two values or `initializer_list` | O(1) / O(k) |
| `min_element` / `max_element` / `minmax_element` | a range | O(n) |
| `clamp(v, lo, hi)` | one value | O(1) |

```cpp
auto [mn, mx] = std::minmax_element(v.begin(), v.end());  // one scan, not two
```

**Why not `sort` and take `front`/`back`?** O(n log n) vs O(n).
**Why not `set`?** If you only need min and max once, a scan
wins. If they update online, `multiset` or two heaps.

`minmax_element` does roughly 1.5 n comparisons, not 2n.

## Permutations

```
  v = [1, 2, 3]
  next_permutation in lexicographic order:
  1 2 3 -> 1 3 2 -> 2 1 3 -> 2 3 1 -> 3 1 2 -> 3 2 1 -> false (sorted again)
```

| Algorithm | Does | Time |
| --- | --- | --- |
| `next_permutation` | next lex perm; `false` if wrapped to sorted | O(n) |
| `prev_permutation` | previous | O(n) |
| `is_permutation` | same elements, any order | O(n²) or O(n) with extra |

`next_permutation` **rearranges in place**. Typical interview
"next permutation" (31) **is** this algorithm; implement it
once, then call it.

Generate all permutations of a small unique array:

```cpp
std::sort(v.begin(), v.end());
do { /* use v */ } while (std::next_permutation(v.begin(), v.end()));
```

Duplicates: `next_permutation` still works and skips identical
perms if the array has dups **and** you started from sorted.

**Why not recursion + `used[]`?** Fine for teaching. The
algorithm is shorter and already handles dups.

`is_permutation(a, b)`: for large ranges, sort copies and
`equal`, or count in a map — do not nest `find` if n is big.

## `shuffle` lives with modifying algorithms

Random permutation: `std::shuffle(v.begin(), v.end(), rng)`.
Not `next_permutation` n! times.

## Related problems

- Next Permutation (31)
- Permutations / Permutations II (46, 47)
- Beautiful arrangement (backtracking; not n!)
- Minimum / maximum in a rotating sorted array (not
  `min_element` — use binary search)
- Clamp a PID output / saturate a score

Run [`examples/28_algo_perm_minmax.cpp`](../examples/28_algo_perm_minmax.cpp).
