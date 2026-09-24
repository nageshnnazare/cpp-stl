# Time-based key-value store — hash of **sorted vectors**

**Need:** `set(key, value, timestamp)` with timestamps that
**never go backward per key**, and `get(key, t)` = the value
whose timestamp is the largest `≤ t` (or missing).

```
  "price"
     time:   10    20        40
     val:     5     8         3

  get("price", 25)  -> 8     last write at or before 25
  get("price",  5)  -> ""    nothing yet
  get("price", 40)  -> 3
```

The outer container is identity ("which key"). The inner
container is **order by time**. That split is the whole
recipe.

```
  unordered_map<string, vector<pair<int,string>>>

  "price"  ->  [(10,5), (20,8), (40,3)]    already sorted
                 upper_bound(25) sits on 40, then --it
```

## Why this, not a `map<int,string>` per key

| Design | set | get | Notes |
| --- | --- | --- | --- |
| `map<string, map<int,string>>` | O(log n) | O(log n) predecessor | fine; extra nodes |
| **`unordered_map` + `vector` + `upper_bound`** | A O(1) append | O(log n) | **best if times increase** |
| `unordered_map` + `map<int,string>` | O(log n) | O(log n) | needed if timestamps arrive **out of order** |
| one global `map<time, ...>` | — | must filter by key | wrong axis |

If the problem **guarantees** strictly increasing timestamps
per key (LeetCode 981), appending to a `vector` and binary
searching is the cache-friendly choice. If writes can be
late/out-of-order, inner `map<int,string>` (or insert into the
vector with `lower_bound` — O(n) shift).

A single `map<pair<key,time>, value>` also works (`upper_bound`
on `(key, t)`). Two-level is clearer.

## Sketch

```cpp
std::unordered_map<std::string, std::vector<std::pair<int, std::string>>> db;

void set(const std::string& k, const std::string& v, int t) {
    db[k].push_back({t, v});          // t increasing
}

std::string get(const std::string& k, int t) {
    auto it = db.find(k);
    if (it == db.end()) return "";
    auto& a = it->second;
    auto ub = std::upper_bound(a.begin(), a.end(), t,
        [](int time, const auto& p){ return time < p.first; });
    if (ub == a.begin()) return "";
    return std::prev(ub)->second;
}
```

`upper_bound` with a mixed-type comparator: first argument is
the search key (`int`), second is a range element. Easy to
get backwards — draw the array.

Full program: [`examples/42_recipe_time_kv.cpp`](../examples/42_recipe_time_kv.cpp).

## Related problems

- Time Based Key-Value Store (981)
- Snapshot Array (1146) — `vector` of `vector<pair<snap,val>>`
  per index; same binary search
- Stock price at time t — [`stock_tracker.md`](stock_tracker.md)
  adds a **multiset of live prices**
- Versioned config / Git-like "value as of commit"
- Online election (911) — prefix of winners + `upper_bound` on
  time
- Find right interval (436) — `map` of start → index

This is the same predecessor idea as
[`containers/map.md`](../containers/map.md) Job 1, with a hash
on the outside so the inner structure stays a tight array.
