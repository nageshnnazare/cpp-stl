# TTL cache — LRU plus an expiry index

**Need:** `get` / `put` like LRU, **and** every entry dies at
`now + ttl`. A get on a corpse must not resurrect it. Put may
refresh the deadline.

LRU is recency (`list` + hash). Deadlines are **time order**.
That is a third container.

```
  now=10   cap=2

  LRU  MRU-> [ B exp=25 ] <-> [ A exp=12 ] <-LRU
  pos    A,B -> iterators
  expiry { (12,A), (25,B) }

  get(A) at now=12:  12>=12 -> destroy A in all three
  put(C) at now=13:  purge expired, then LRU-evict if still full
```

## Why three

| Design | get | put | expire | Verdict |
| --- | --- | --- | --- | --- |
| LRU only | O(1) | O(1) | never | wrong spec |
| hash + `expire_at` field, scan | O(1) | O(1) | O(n) | |
| hash + min-heap of deadlines | O(1) | O(log n) | lazy O(log n) | heap cannot refresh without lazy |
| **list + pos hash + `set{exp,key}`** | O(1) + purge | O(log n) | O(log n) per dead | **the one** |

`priority_queue` of deadlines needs lazy skip (stale
`(exp,key)` after a refresh). A `set` can `erase({old_exp,key})`
on update. Same reason [`stock_tracker.md`](stock_tracker.md)
uses a `multiset` not a heap.

Purge is lazy: at the start of `get`/`put`, pop `expiry` while
`begin()->first <= now`. Each entry is created once and
destroyed once.

## Sketch

```cpp
struct Node { int key, val; long long exp; };
std::list<Node> lru;
std::unordered_map<int, std::list<Node>::iterator> pos;
std::set<std::pair<long long, int>> expiry;

void kill(std::list<Node>::iterator it) {
    expiry.erase({it->exp, it->key});
    pos.erase(it->key);
    lru.erase(it);
}

void purge(long long now) {
    while (!expiry.empty() && expiry.begin()->first <= now) {
        int k = expiry.begin()->second;
        kill(pos[k]);
    }
}
```

**Update** of TTL: erase the old pair from `expiry` **before**
the node.exp changes, then insert the new pair. Destroy always
touches three places.

Full program: [`examples/52_recipe_ttl_cache.cpp`](../examples/52_recipe_ttl_cache.cpp).

## Related problems

- LRU Cache (146) + a TTL field — this recipe
- Authentication Manager (1797) — hash of tokens + optional
  expiry `set` if you must count without scanning
- [`hit_counter.md`](hit_counter.md) — only time, no recency /
  capacity
- Redis `EXPIRE` + `LRU` eviction: same two policies, same two
  indexes plus the dict
