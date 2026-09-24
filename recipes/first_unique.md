# First unique — recency list plus two hashes

**Need:** a stream. After each `add(x)`, answer "the first
value that has appeared **exactly once**" in O(1). Duplicates
fall out forever.

```
  add 2, 3, 2, 6

  unique list (insertion order of still-unique):
    after 2:     [ 2 ]
    after 3:     [ 2, 3 ]
    after 2:     [ 3 ]          2 demoted, dead
    after 6:     [ 3, 6 ]
    first unique = 3
```

A count map alone makes "first" an O(n) scan. A queue of
candidates plus counts is **amortized** O(1) (skip dead
fronts on query). True O(1) query **and** O(1) add uses the
LRU trick: a `list` of uniques, a hash to the node, and a
**dead set** so a third copy is a no-op.

```
  order:  list of still-unique values
  pos:    unordered_map<value, list::iterator>
  dead:   unordered_set of values seen ≥ 2 times
```

## Why three (not queue + count)

| Design | add | first unique | Notes |
| --- | --- | --- | --- |
| `unordered_map` counts + scan | O(1) | O(n) | |
| `queue` + counts | O(1) | amortized O(1) | queue holds corpses until queried |
| **`list` + pos + dead`** | O(1) | O(1) | destroy the node at 2nd sight |

CRUD: **create** = first sight, `push_back` + `pos[x]=it`.
**Read** = `order.front()`. **Destroy** = second sight,
`order.erase(pos[x])` (O(1) because you hold the iterator),
`dead.insert(x)`. There is no "update" of the value; the
update is a **move to the dead set**.

Same iterator-stability story as
[`lru_cache.md`](lru_cache.md). `vector` cannot erase the
middle unique in O(1).

Full program: [`examples/55_recipe_first_unique.cpp`](../examples/55_recipe_first_unique.cpp).

## Related problems

- First Unique Number (1429)
- First Unique Character in a String (387) — static: `array<int,26>`
  then one scan (no list)
- LRU / LFU — capacity eviction; here eviction is "became
  duplicate"
- All O(1) Unique: this plus [`randomized_set.md`](randomized_set.md)
  if you also need a random unique
