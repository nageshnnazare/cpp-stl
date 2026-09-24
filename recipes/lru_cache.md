# LRU cache — `list` + `unordered_map`

**Need:** `get(key)` and `put(key, val)` in **average O(1)**,
evict the least recently used entry when at capacity.

One container cannot do both: a hash table has no recency
order; a list has no O(1) lookup.

```
  capacity 3

  recency list (MRU at front):
  front [ (B,2) ] <-> [ (A,1) ] <-> [ (C,3) ] back=LRU

  hash:   A -> iterator-to-A
          B -> iterator-to-B
          C -> iterator-to-C

  get(A):  splice A node to front
  put(D):  full -> erase C from list and hash, then push D front
```

## Why these two, not the alternatives

| Design | get | put / evict | Verdict |
| --- | --- | --- | --- |
| `vector<pair>` + scan | O(n) | O(n) | toy only |
| `unordered_map` only | O(1) | cannot know LRU | missing order |
| `map<timestamp, key>` + hash | O(log n) | O(log n) | works, slower, timestamps are messy on every get |
| `deque` + hash of index | O(1) find | middle erase O(n), indices dangle | broken |
| **`list` + hash of iterators** | O(1) | O(1) splice / erase | **the one** |

`std::list::splice` moves a node in O(1) and **does not
invalidate** the iterator the hash holds. That iterator
stability is why `list` appears at all.

## Sketch

```cpp
using Node = std::pair<int, int>;          // key, value
std::list<Node> order;                     // MRU..LRU
std::unordered_map<int, std::list<Node>::iterator> pos;

int get(int k) {
    auto it = pos.find(k);
    if (it == pos.end()) return -1;
    order.splice(order.begin(), order, it->second);  // move to MRU
    return it->second->second;
}

void put(int k, int v) {
    if (auto it = pos.find(k); it != pos.end()) {
        it->second->second = v;
        order.splice(order.begin(), order, it->second);
        return;
    }
    if ((int)order.size() == cap) {
        pos.erase(order.back().first);
        order.pop_back();
    }
    order.push_front({k, v});
    pos[k] = order.begin();
}
```

Full program: [`examples/30_recipe_lru.cpp`](../examples/30_recipe_lru.cpp).

## Related problems (same two-structure idea)

- **LFU cache (460):** add `unordered_map<freq, list<...>>` or a
  freq→list map. Three maps. The extra dimension is frequency.
- **First unique number in a stream:** queue of candidates +
  map of counts; pop while front is no longer unique.
- **All O(1) data structure (insert, delete, getRandom):**
  `vector` + `unordered_map` to index. Delete = swap-with-last
  (vector cannot hole-punch in O(1) otherwise).
- **Design Twitter (355):** `unordered_map` of heaps / lists of
  tweets + follow graph (`unordered_map<int, unordered_set<int>>`).
- **LRU *picture* for a page cache, buffer pool, editor tabs.**

If someone asks "why not `std::map` for order?": recency is not
sorted by key. A tree ordered by timestamp works but get becomes
O(log n) and you update timestamps constantly. The list **is**
the timestamp.
