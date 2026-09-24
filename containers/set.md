# `std::set` / `std::multiset` — unique (or not), always sorted

A `set` is a **balanced tree of keys**. Walk is in-order.
`insert` / `find` / `erase` are O(log n). There is no index.

```
  insert 5, 1, 9, 3, 7

              5
            /   \
           1     9
            \   /
             3 7

  begin() -----> 1, 3, 5, 7, 9  <----- --end()
  lower_bound(4) ----------------> 5
  upper_bound(5) ----------------> 7
```

Header: `<set>`. Iterator: **bidirectional**.
`*it` is `const Key` — you cannot change a key in place (that
would break the tree). Erase + insert instead.

`multiset` allows duplicates. `set` silently refuses them
(`insert` returns `{it, false}`).

## CRUD

| | Methods | Cost |
| --- | --- | --- |
| C | `insert`, `emplace`, `insert(hint, x)` | O(log n); sequential hint A O(1) |
| R | `find`, `contains`, `count`, `lower_bound`, `upper_bound`, `equal_range` | O(log n) |
| U | not in place | erase + insert |
| D | `erase(key)`, `erase(it)`, `extract` (C++17 node handle) | O(log n) / O(1) at `it` |

Use **members** `s.lower_bound(x)`, not `std::lower_bound(s.begin(),
s.end(), x)`. See [`01_iterators.md`](../01_iterators.md).

## Job 1 — online unique events, always able to print min/max

A stream of user ids; you need "how many distinct right now",
"smallest id", "next id ≥ x".

```
  set:  { 2, 5, 9 }
  arrive 5  -> already there, size unchanged
  arrive 7  -> { 2, 5, 7, 9 }     lower_bound(6) == 7
  leave  2  -> { 5, 7, 9 }        *begin() == 5
```

**Why not `unordered_set`?** You cannot get min / predecessor in
O(log n). **Why not a sorted `vector`?** Each insert in the
middle is O(n). Fine if you insert-all then `sort` once.

## Job 2 — `multiset` as a delete-anywhere heap

Need the current min (or max) **and** "remove the value 7":

```
  multiset<int> ms;          // begin = min, --end = max
  ms.insert(x);
  ms.erase(ms.find(x));      // erase ONE 7, not every 7
  // ms.erase(x) erases ALL x  <-- the classic bug
```

`priority_queue` cannot delete 7. Lazy-delete heaps (push a
tombstone, skip on pop) work if you can stand stale tops.

This is half of "median of a stream" (two multisets). See
[`recipes/median_stream.md`](../recipes/median_stream.md).

## Job 3 — sweep unique coordinates

Compress coordinates: dump values into a `set`, then rank with
`distance(s.begin(), s.find(x))` — **that distance is O(n)**.
Better: copy the set into a `vector` once, `lower_bound` on the
vector. Or skip the set: `vector` + `sort` + `unique`.

```
  raw:     100, 5, 100, 30
  set:     5, 30, 100            then vector ranks
  rank:    5->0  30->1  100->2
```

Prefer `vector` + `sort` + `unique` unless you truly insert
online.

## Why this, not that

| Alternative | Prefer set when | Prefer the other when |
| --- | --- | --- |
| `unordered_set` | need order / closest key | only "have I seen x?" |
| sorted `vector` | many interleaved insert+query of min/closest | build once, query many |
| `map<K,int>` counts | you only care about presence | you need frequencies |
| `priority_queue` | need arbitrary erase or both ends | only ever pop the extreme |

Custom order: `set<T, decltype(cmp)> s(cmp);` or
`set<T, greater<T>>` for reverse in-order.

## Related problems

- Contains Duplicate (set or unordered_set)
- Exam room / find nearest (set of taken seats + distances)
- Number of distinct islands after hashing — set of canonical
  shapes
- Sliding window unique count (`set` or hash + sliding)
- My Calendar I (set of intervals, `lower_bound` on start)
- Count of smaller numbers after self (policy tree / Fenwick;
  `multiset` is the slow correct version)

Run [`examples/07_set_online.cpp`](../examples/07_set_online.cpp).
