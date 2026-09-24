# Max stack — a list (the stack) plus a tree of positions

**Need:** `push` / `pop` / `top` in O(1), **and** `peekMax` /
`popMax` in O(log n). Min stack ([`min_stack.md`](min_stack.md))
only exposes the extreme of the **current top-prefix**; it
cannot delete a max that is buried.

```
  stack as a list (back = top)
  [ 3 ] <-> [ 8 ] <-> [ 5 ] <-> [ 8 ]
                                 ^ top
  at[8] = iterators to both eights (most recent last)
  at[5] = ...
  at[3] = ...

  popMax:  last iterator of at.rbegin()  -> the rightmost 8
           list.erase(that node)         O(1)
           at[8].pop_back()
  top is now 5
```

```
  list<int>                 order, O(1) pop/top, O(1) erase at it
  map<int, vector<list::iterator>>
                            max key = peekMax; last it = most recent max
```

A second stack of "max so far" gives `peekMax` but **not**
`popMax` of a buried node (you would have to replay).

## Why the list, not a `vector`

`popMax` erases a **middle** element. `vector::erase` is O(n)
and **invalidates iterators** stored in the map. `list::erase`
invalidates only that iterator; the other stored iterators
stay.

CRUD:

| Verb | How |
| --- | --- |
| C push | `push_back`, `at[x].push_back(it)` |
| R top / peekMax | `list.back()` / `at.rbegin()->first` |
| D pop | pop both back |
| D popMax | erase buried node + pop that vector |

Two eights: `popMax` must remove the **later** one (stack
semantics). That is why the map holds a **vector** (or
`list`) of iterators, not a single iterator.

Full program: [`examples/57_recipe_max_stack.cpp`](../examples/57_recipe_max_stack.cpp).

## Related problems

- Max Stack (716)
- Min Stack (155) — two stacks, no buried delete
- [`freq_stack.md`](freq_stack.md) — pop by frequency, not by
  max value
- [`lfu_cache.md`](lfu_cache.md) — map of lists of nodes, same
  "erase a held iterator" move
