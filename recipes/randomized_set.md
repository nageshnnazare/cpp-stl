# Insert / delete / getRandom in O(1) — `vector` + `unordered_map`

**Need:** add a value, remove a value, and return a **uniform
random** element, all average O(1). No duplicates (the
multiset variant is a footnote).

A hash set has O(1) insert/delete but **cannot index** a random
slot. A `vector` has O(1) random index but `erase` in the
middle is O(n).

```
  vector  [ 7 | 3 | 9 | 1 ]
  index     0   1   2   3

  hash    7->0  3->1  9->2  1->3

  remove 3:  swap with last, pop, fix the swapped key's index

  vector  [ 7 | 1 | 9 ]        1 moved from 3 to 1
  hash    7->0  1->1  9->2     3 gone
```

That swap-with-last is the whole design. It is also how you
delete from an unordered object pool without leaving holes.

## Why these two, not the alternatives

| Design | insert | erase | getRandom | Verdict |
| --- | --- | --- | --- | --- |
| `unordered_set` only | O(1) | O(1) | O(n) to pick / not uniform if you skip | no `[]` |
| `vector` only | O(1) | O(n) | O(1) | erase kills you |
| `list` + hash of iterators | O(1) | O(1) | O(n) walk to the k-th | random needs an index |
| **`vector` + `unordered_map` to index** | O(1) | O(1) | O(1) | **the one** |

## Sketch

```cpp
std::vector<int> a;
std::unordered_map<int, int> at;   // value -> index in a

bool insert(int x) {
    if (at.count(x)) return false;
    at[x] = (int)a.size();
    a.push_back(x);
    return true;
}

bool erase(int x) {
    auto it = at.find(x);
    if (it == at.end()) return false;
    int i = it->second;
    int last = a.back();
    a[i] = last;
    at[last] = i;          // last now lives at i
    a.pop_back();
    at.erase(it);
    return true;
}

int get_random() {
    return a[rng() % a.size()];
}
```

**Order of the last two lines in `erase` when `x` is already
the last element:** if you `at.erase(x)` first you are fine;
if you write `at[last] = i` *after* erasing `x` and `last==x`,
you just re-insert x. Either special-case `i == size-1` or
erase from the map **after** fixing `last` only when
`last != x`. The code above uses `it` to erase x after moving
last; when x is last, `at[last]=i` is a no-op overwrite then
`at.erase(it)` drops it. Good.

Full program: [`examples/41_recipe_randomized_set.cpp`](../examples/41_recipe_randomized_set.cpp).

## Duplicates (380 → 381)

`unordered_map<int, unordered_set<int>>` of indices, still one
`vector`. On erase, pick **any** index of x from that set
(`.begin()`), swap-with-last, fix both index sets.

## Related problems

- Insert Delete GetRandom O(1) (380), with duplicates (381)
- Insert Delete GetRandom O(1) - Duplicates allowed
- Random pick index (398) — if the array is **static**, do not
  build this; reservoir or `vector` of positions
- Random pick with weight (528) — prefix sums +
  `upper_bound` (different recipe)
- Object pool / entity-component "sparse set" in games: this
  exact pair of containers
- LRU's cousin: you needed **order**, not random; that is a
  `list`, not a `vector`
