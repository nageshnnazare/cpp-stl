# LFU cache — hash + **frequency lists**

**Need:** O(1) `get` / `put` like LRU, but evict the **least
frequently used** key. Ties: least **recent** among that
frequency.

LRU was one list (recency). LFU is **one list per frequency**,
plus a hash to the node, plus the current `min_freq`.

```
  cap=2
  put(1), put(2)          freq 1:  1 <-> 2     min_freq=1
  get(1)                  freq 1:  2
                          freq 2:  1           min_freq=1
  put(3)  evicts 2        freq 1:  3
                          freq 2:  1
```

```
  pos[key]  ----->  node sitting inside freq_lists[f]

  freq_lists[1]:  LRU ... MRU     (we still splice to back on use)
  freq_lists[2]:  ...
  freq_lists[3]:  ...
  min_freq -----> smallest f that still has a node
```

## Why three maps, not "LRU + a counter"

| Design | get | evict | Verdict |
| --- | --- | --- | --- |
| LRU only | O(1) | wrong victim | different policy |
| hash of counts + scan min | O(1) | O(n) | |
| `multiset` of (freq, time, key) | O(log n) | O(log n) | acceptable; not O(1) |
| **hash to node + map<freq, list>** | O(1) | O(1) | **the one** |

`std::map<int, list<...>>` for `freq_lists` is O(log F) on
the first access to a new frequency. F is tiny in practice;
`unordered_map<int, list<...>>` is average O(1). Keep
`min_freq` so you do not scan.

When a node's frequency goes from `f` to `f+1`:

1. `splice` it from `freq_lists[f]` to the **back** of
   `freq_lists[f+1]` (it is now MRU at that freq).
2. If `freq_lists[f]` is empty and `f == min_freq`,
   `++min_freq`.
3. If you just inserted a brand-new key, `min_freq = 1`.

Evict: `pop_front` of `freq_lists[min_freq]` (LRU among LFU).

## Sketch

```cpp
struct Node { int key, val, f; };
int cap, minf = 0;
std::list<Node> empty; // not used; lists live in the map
std::unordered_map<int, std::list<Node>> bucket;          // freq -> LRU..MRU
std::unordered_map<int, std::list<Node>::iterator> pos;   // key -> node

void bump(std::list<Node>::iterator it) {
    int f = it->f;
    bucket[f + 1].splice(bucket[f + 1].end(), bucket[f], it);
    it->f++;
    if (bucket[f].empty()) {
        bucket.erase(f);
        if (minf == f) ++minf;
    }
}
```

Full program: [`examples/46_recipe_lfu.cpp`](../examples/46_recipe_lfu.cpp).

## Related problems

- LFU Cache (460)
- LRU Cache (146) — [`lru_cache.md`](lru_cache.md)
- All O(1) LFU is a favorite "three hash maps" interview
- TinyLFU / W-TinyLFU (production): sketch + LRU window, not
  this exact structure
- First unique number: frequency map + queue — a *stream*
  cousin, not a capacity cache
