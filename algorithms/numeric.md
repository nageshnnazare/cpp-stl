# Numeric algorithms — running totals, not "math"

Header: `<numeric>`. These are the STL names for prefix sums
and folds. Time O(n). Extra O(1) besides output.

```
  v:            [ 2 | 3 | 1 | 5 ]
  partial_sum:  [ 2 | 5 | 6 | 11 ]
  exclusive:    [ 0 | 2 | 5 | 6  ]     (C++17 exclusive_scan)
  adjacent_diff:[ 2 | 1 |-2 | 4  ]     first element copied, then deltas
```

| Algorithm | Does | Notes |
| --- | --- | --- |
| `iota` | write `val, val+1, ...` | not a loop with `i++` |
| `accumulate` | left fold | order fixed; `init` type matters |
| `reduce` (C++17) | fold, may reorder | parallel-friendly; not for `string` + |
| `inner_product` | dot product (or custom) | two ranges |
| `partial_sum` | inclusive prefix | |
| `inclusive_scan` / `exclusive_scan` | C++17 names | `exclusive` needs identity |
| `adjacent_difference` | inverse of prefix | |
| `transform_reduce` / `transform_inclusive_scan` | map then fold | |
| `gcd` / `lcm` | two integers | C++17 |

## Job 1 — prefix sums (range sum, subarray tricks)

```cpp
std::vector<long long> pref(n + 1);
std::partial_sum(a.begin(), a.end(), pref.begin() + 1);
// sum l..r inclusive: pref[r+1] - pref[l]
```

**Why `long long`?** `int` overflow is UB. The algorithm does
not pick a wider type for you — `accumulate` uses the type of
`init`.

```cpp
std::accumulate(a.begin(), a.end(), 0);     // int, may overflow
std::accumulate(a.begin(), a.end(), 0LL);   // long long
```

That `0` vs `0LL` is the most common `accumulate` bug.

## Job 2 — `iota` for indices

```cpp
std::vector<int> idx(n);
std::iota(idx.begin(), idx.end(), 0);
std::sort(idx.begin(), idx.end(),
          [&](int i, int j){ return a[i] < a[j]; });
```

Sort indices, not the array, when you must recover original
positions. **Why not `pair{value,i}`?** Same idea; `iota` keeps
the values in `a` untouched.

## Job 3 — difference array (inverse prefix)

Range update `[l,r] += x` k times, then point queries:

```
  d[l] += x
  d[r+1] -= x
  a = prefix of d
```

`adjacent_difference` turns `a` into `d`. You usually build `d`
directly.

## `accumulate` vs `reduce`

```
  accumulate:  ((((init + a0) + a1) + a2) + ...)
  reduce:      may (a0+a1) + (a2+a3)   needs associative + identity
```

`float` sums change; `string` concat is not parallel-safe.
Default to `accumulate`. Reach for `reduce` when you opt into
`<execution>` policies.

## Related problems

- Range Sum Query
- Subarray sum equals k (prefix + `unordered_map`)
- Product of array except self (prefix/suffix)
- Running XOR / prefix XOR for subarray XOR
- `inner_product` for dot product / polynomial eval
- Josephus via `iota` + erase (or a math formula)

Run [`examples/27_algo_numeric.cpp`](../examples/27_algo_numeric.cpp).
