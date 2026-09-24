# `std::map` / `std::multimap` — sorted key → value

A `map` is a `set` of `pair<const Key, T>` ordered on `Key`.
Same O(log n) tree. You walk keys in sorted order and you can
ask for the **successor** of a key that is not present.

```
  m["ada"] = 1815;  m["grace"] = 1906;  m["alan"] = 1912;

              "grace"
             /       \
         "alan"     (empty)
         /
      "ada"

  lower_bound("alex")  -> "grace"     first key ≥ "alex"
  upper_bound("alan")  -> "grace"     first key >  "alan"
```

Header: `<map>`. Iterator: bidirectional.
`it->first` is const; `it->second` is mutable.

## CRUD — and the `operator[]` trap

| | Methods | Cost | Danger |
| --- | --- | --- | --- |
| C | `insert`, `emplace`, `try_emplace`, `insert_or_assign`, `m[k]=` | O(log n) | `m[k]` **inserts** a default `T` if `k` missing |
| R | `find`, `contains`, `at` | O(log n) | `at` throws `out_of_range`; `find` returns `end()` |
| U | `it->second =`, `insert_or_assign` | O(log n) to locate | |
| D | `erase(k)`, `erase(it)` | O(log n) / O(1) | |

```cpp
if (m[k] == 0) { /* bug: just inserted k if it was new */ }
auto it = m.find(k);
if (it != m.end() && it->second == 0) { /* read-only */ }
```

`try_emplace(k, args...)` constructs `T` only on a real insert.
Use it when `T` is expensive.

`multimap`: several values per key, stored as consecutive
equal-key nodes. `equal_range(k)` is the slice. There is no
`operator[]`.

## Job 1 — time → snapshot (predecessor query)

"What was the stock price at time t?" You do not have every t,
only writes.

```
  10 -> 5
  20 -> 8
  40 -> 3

  query 25:  lower_bound(25), then --it   => 20 -> 8
  query  5:  lower_bound(5) == begin() and 10>5  => none
```

**Why not `unordered_map`?** Hash tables have no predecessor.
**Why not a sorted `vector<pair>`?** Perfect if all writes arrive
before queries (`upper_bound`). Use `map` when writes and queries
interleave.

Related: Time Based Key-Value Store (981), snapshot arrays.

## Job 2 — sweep line / range coverage

```
  intervals [1,4), [2,5), [7,8)
  delta map:  1:+1  2:+1  4:-1  5:-1  7:+1  8:-1

  walk keys in order, running += delta
  overlap height:  1:1  2:2  4:1  5:0  7:1  8:0
```

The **order of keys is the algorithm**. A hash map would force
you to sort the keys anyway — then you wanted a `vector` of
events + `sort`.

## Job 3 — frequency, but you will iterate alphabetically

Word count that must print in key order: `map<string,int>`.
If you only need top-k later, count with `unordered_map` and
sort a vector of pairs — cheaper.

## Coordinate compression (map as ranker)

```
  values:  40  10  40  25
  map:     10->0, 25->1, 40->2     assign ranks while walking
```

Or `vector` + sort + unique + `lower_bound`. The vector path is
faster for a static set. The map path is shorter to type when
ranks are assigned during a single in-order walk.

## Why this, not that

| Alternative | Prefer `map` | Prefer the other |
| --- | --- | --- |
| `unordered_map` | successor / ordered walk / no hash | hot lookup, no order |
| `set<pair<K,V>>` | you need to find by K alone | compound order on (K,V) |
| `vector<pair>` + sort | interleaved updates + queries | static data |
| `array` / `vector` | sparse or non-int keys | dense `0..n` ids |

## Related problems

- Merge intervals (usually sort a `vector`; map if online)
- My Calendar I/II/III (`map` of deltas)
- Range module (map of disjoint [l,r))
- Vertical order traversal of a tree (`map<x, ...>`)
- Stock price fluctuation (map of time→price + multiset of prices)
- Count intervals overlapping a point (sweep + map)

Run [`examples/08_map_timeline.cpp`](../examples/08_map_timeline.cpp).
