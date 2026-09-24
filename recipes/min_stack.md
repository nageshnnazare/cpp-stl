# Min stack (and max stack) — two stacks, or one stack of pairs

**Need:** `push` / `pop` / `top` in O(1), **and** `getMin()`
(or `getMax()`) in O(1). A single stack only gives you the
top, not the extreme of everyone below.

```
  vals:     3   5   2   7   2
  mins:     3   3   2   2   2     mins.top() = min of current vals

  pop -> mins pop in lockstep
```

## Why two stacks, not the rest

| Design | getMin | extra space | Notes |
| --- | --- | --- | --- |
| scan the stack | O(n) | 0 | not the spec |
| `multiset` + stack | O(1) min | O(n) nodes | erase-one on pop; overkill |
| **parallel `mins` stack** | O(1) | O(n) worst | duplicates min when it does not change |
| **stack of `{val, min_so_far}`** | O(1) | O(n) | one object, same idea |
| `mins` only when `x <= min` | O(1) | smaller | pop must know when to pop min (`<=` for duplicates) |

The compact variant:

```
  push 3,5,2,7,2
  vals  [3 5 2 7 2]
  mins  [3   2   2]     push to mins only if x <= mins.top()
  pop 2: 2==mins.top() so mins pop
```

Use `<=` not `<`: two copies of the current min, two pops.

A **max stack** that also `popMax()` (716) is **not** this
recipe: you must delete a non-top. That is a `list` +
`multiset` of iterators, or a stack plus an auxiliary max
`stack` of *positions* plus lazy skip. Different CRUD.

## Sketch

```cpp
std::stack<int> vals, mins;

void push(int x) {
    vals.push(x);
    if (mins.empty() || x <= mins.top()) mins.push(x);
}
void pop() {
    if (vals.top() == mins.top()) mins.pop();
    vals.pop();
}
int top()    { return vals.top(); }
int getMin() { return mins.top(); }
```

Full program: [`examples/44_recipe_min_stack.cpp`](../examples/44_recipe_min_stack.cpp).

## Related problems

- Min Stack (155)
- Max Stack (716) — list + tree / two stacks + lazy
- Largest Rectangle in Histogram still uses a **monotonic**
  stack, not a min-stack
- Sliding window minimum — deque, not a stack
  ([`sliding_window.md`](sliding_window.md))
- Queue with getMin: two stacks (queue-from-two-stacks) each
  carrying a min, or a min-deque
