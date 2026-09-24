# Live stock tracker — `map` of time + `multiset` of prices

**Need:** `update(timestamp, price)` (a timestamp may be
**corrected**), plus `current()` (latest time's price),
`maximum()`, `minimum()` among **current** prices (one per
timestamp that still exists).

Leaderboard ordered **by score**. This structure orders
**by time** and also needs min/max **by price**. Two orders ⇒
two trees.

```
  time -> price
    1 -> 10
    2 ->  5
    4 ->  7
  latest = 4 -> 7     *time.rbegin()
  prices bag:  {5, 7, 10}    *begin min, *rbegin max

  update(2, 8)  correct the 5:
      prices: erase 5, insert 8
      time[2] = 8
```

## Why these two, not a heap

| Design | update/correct | current | min/max | Verdict |
| --- | --- | --- | --- | --- |
| `priority_queue` of prices | cannot delete the old 5 | need last time separately | stale tops | lazy-delete possible |
| `map<time,price>` only | O(log n) | O(1) rbegin | O(n) scan | missing price order |
| `multiset` only | no "latest time" | — | O(1) | missing time order |
| **`map` + `multiset`** | O(log n) | O(1) | O(1) | **the one** |
| two heaps + lazy | O(log n) | extra `last_time` | O(1) amortized | more code |

`map` because you need the **predecessor/successor of a time**
and the maximum time. `unordered_map<time,price>` would lose
`current()` unless you also store `last_time` and handle
corrections that *are* the last time (OK) vs *are not*.

`multiset::erase(5)` wipes **all** 5s. Always
`prices.erase(prices.find(old))`.

## Sketch

```cpp
std::map<int, int> at;          // time -> price
std::multiset<int> prices;

void update(int t, int p) {
    if (auto it = at.find(t); it != at.end()) {
        prices.erase(prices.find(it->second));
        it->second = p;
    } else {
        at[t] = p;
    }
    prices.insert(p);
}

int current() { return at.rbegin()->second; }
int maximum() { return *prices.rbegin(); }
int minimum() { return *prices.begin(); }
```

Full program: [`examples/48_recipe_stock_tracker.cpp`](../examples/48_recipe_stock_tracker.cpp).

Same skeleton as the leaderboard (`hash/map` by id + ordered
bag of values), with **time** as the identity axis instead of
player id. If you need top-K *players*, go to
[`leaderboard.md`](leaderboard.md). If you need min/max of a
**window of times**, that is a deque /
[`median_stream.md`](median_stream.md), not this.

## Related problems

- Stock Price Fluctuation (2034)
- Design a Food Rating System (2353) — cuisine board, same
  erase+insert in a `set`
- Max stack (716)
- Snapshot / time KV — [`time_based_kv.md`](time_based_kv.md)
  (no min/max of all live values)
- Falling squares / skyline — `map` of x → height
