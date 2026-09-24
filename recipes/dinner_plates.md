# Dinner plates — a row of stacks plus a hole index

**Need:** infinitely many stacks of capacity `cap`. `push` goes
to the **leftmost** stack that is not full (or a new one on
the right). `pop` takes from the **rightmost** nonempty.
`popAt(i)` pops stack `i`.

```
  cap=2
  [ 1 2 ] [ 3 4 ] [ 5 ]
      ^full   ^full   ^has a hole

  popAt(0) -> 2
  [ 1 ] [ 3 4 ] [ 5 ]
    ^ leftmost hole   next push lands here, not on a new stack
```

The `vector<stack<int>>` is the data. It cannot answer
"leftmost non-full" in O(1) / O(log n). A `set<int>` of
indices with `size < cap` can: `*begin()`.

```
  stacks:  vector<stack<int>>
  holes:   set<int>            indices you may push onto
```

`pop` also needs "rightmost nonempty". Either a second `set`
of nonempty indices (`*rbegin()`), or shrink the vector from
the back while `stacks.back().empty()`. The shrink is amortized
O(1) if you only do it on `pop`. `popAt` of a middle stack
**must not** shrink (indices of later stacks would shift —
that would invalidate the hole set). So: **never erase a
middle stack**; only trim empty **suffix**.

## Why not one vector of stacks

| Design | push | popAt | Verdict |
| --- | --- | --- | --- |
| scan for a hole | O(#stacks) | O(1) | |
| `queue` of hole indices | O(1) | stale holes | must lazy-skip |
| **`vector<stack>` + `set` of holes** | O(log n) | O(log n) | **the one** (1172) |

CRUD: create = push (maybe `push_back` a new stack and
`holes.insert`). Read = `top` of a stack. Destroy = pop;
if that stack was full, `holes.insert(i)` **recreates** a
hole. That insert is the whole point of the third structure.

Full program: [`examples/54_recipe_dinner_plates.cpp`](../examples/54_recipe_dinner_plates.cpp).

## Related problems

- Dinner Plate Stacks (1172)
- [`randomized_set.md`](randomized_set.md) — holes in a vector
  are filled by **swap-with-last** (indices may move). Here
  stack **indices are identities** and cannot move, so holes
  live in a `set` instead.
- Order book price levels: empty levels are erased because
  price is not a stable integer slot; plate indices are.
