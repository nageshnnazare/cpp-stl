# `std::vector` — the default sequence

A `vector` is a **contiguous growable array**. One pointer to the
buffer, a size, and a capacity. Almost every "I need a list of
things" problem should start here.

```
  object:   { ptr, size=4, cap=7 }

  heap:     [ 10 | 20 | 30 | 40 |  _  |  _  |  _  ]
              0    1    2    3     unused capacity

  push_back(50) that still fits:   write slot 4, size=5     O(1)
  push_back that does not fit:     allocate 2*cap, move all, free old
                                   O(n) this time, amortized O(1)
```

Header: `<vector>`. Iterator: **contiguous**.

## CRUD

| | Methods | Cost | Notes |
| --- | --- | --- | --- |
| C | `vector(n)`, `vector(n, val)`, `{a,b}`, `push_back`, `emplace_back`, `insert`, `assign` | `push_back` A O(1); `insert` at i is O(n−i) | `emplace_back` constructs in place |
| R | `v[i]`, `at`, `front`, `back`, `data()`, iterators | O(1) | `at` throws; `[]` does not check |
| U | `v[i] =`, `assign`, `swap` | O(1) per element | |
| D | `pop_back`, `erase(it)`, `erase(a,b)`, `clear` | pop O(1); erase O(n−i) | `clear` keeps capacity |

`reserve(n)` sets capacity without changing size. `resize(n)`
changes size (default-constructs or destroys). Mixing them up
is the first vector bug:

```
  reserve(8)   size=0  cap=8    [ _ _ _ _ _ _ _ _ ]   do not v[0]=
  resize(8)    size=8  cap≥8    [ 0 0 0 0 0 0 0 0 ]   v[0]= is fine
```

## Job 1 — a timeline you append and index

Chat messages, simulation ticks, undo records: write at the end,
read by index, occasionally replay the prefix.

```cpp
struct Tick { int t; double x; };
std::vector<Tick> log;
log.reserve(10'000);           // you know the ballpark
log.push_back({t, pos});
double at = log[i].x;          // O(1)
```

**Why not `deque`?** You never push the front. `vector` is
contiguous: prefix sums, `data()` to C APIs, and `sort` are
cheaper. **Why not `list`?** You need `log[i]`.

Related: reconstruct a queue from operations, flight-booking
seat lists, "return the k-th event".

## Job 2 — prefix sums (range sum in O(1) after O(n))

```
  a:      [ 2 | 3 | 1 | 5 | 4 ]
  pref: [0| 2 | 5 | 6 |11 |15 ]
  sum(l..r) = pref[r+1] - pref[l]
```

This is *the* reason a sequence is an array and not a tree: you
walk once, then answer many `[l, r]` sums. Code:
[`examples/01_vector_timeline.cpp`](../examples/01_vector_timeline.cpp).

Related: Range Sum Query (LeetCode 303), subarray-sum equals k
(needs a map too — [`recipes/two_sum_and_anagrams.md`](../recipes/two_sum_and_anagrams.md)),
difference arrays (mark `d[l]+=x; d[r+1]-=x`, then prefix).

## Job 3 — erase-remove, not a handwritten delete loop

```cpp
// drop every negative: one pass, then shrink
v.erase(std::remove_if(v.begin(), v.end(),
                       [](int x){ return x < 0; }),
        v.end());
```

```
  remove_if slides keepers left
  [ 3 | -1 | 4 | -2 | 5 ]  ->  [ 3 | 4 | 5 | ? | ? ]
                                      ^
                                   new logical end    erase the tail
```

A loop with `v.erase(it)` on every hit is O(n²).

## Job 4 — `vector<vector<T>>` as a graph

Adjacency list for nodes `0..n-1`:

```
  0 -> 1, 2
  1 -> 2
  2 ->

  vector<vector<int>> g(n);
  g[0] = {1, 2};
```

**Why not `unordered_map<int, vector<int>>`?** If ids are dense
`0..n-1`, the map's hash is pure overhead. Use a map only when
ids are sparse or not integers.

## Why this, not that

| Alternative | When it wins | When it loses |
| --- | --- | --- |
| `array<T,N>` | N fixed, no heap | N unknown |
| `deque` | push_front / pop_front | you only append; you need `T*` |
| `list` | O(1) erase of a **held** iterator, splice | everything else |
| `string` | text, SSO | non-char T |
| `set` | keep sorted unique under inserts | build-once then sort a vector |

## Iterator / reference traps

```
  auto& x = v[0];
  v.push_back(1);     // may realloc  => x is dangling
  auto it = v.begin();
  v.erase(it + 2);    // it+0, it+1 still ok; it+2 and after die
```

`vector<bool>` is **not** a real container (proxy references).
Use `vector<char>` or `bitset` / `vector<uint8_t>`.

## Related problems

- Running sum / product of an array
- Rotate array (`std::rotate`)
- Merge two sorted arrays (two pointers on vectors)
- Spiral matrix, set-matrix-zeroes (2D vector)
- Dynamic programming tables (`vector<vector<int>>` or flattened
  `vector<int>(n*m)`)
- Implement stack / queue **using** a vector (know the cost of
  `erase(begin)` = O(n))

Run [`examples/01_vector_timeline.cpp`](../examples/01_vector_timeline.cpp).
