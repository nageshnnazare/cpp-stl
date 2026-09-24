# Monotonic stack and monotonic deque

"Monotonic" means the structure's values are always increasing
or always decreasing. You **pop** anything that would break the
invariant before you push. The container is `stack` (one end)
or `deque` (two ends). The answer array is a `vector`.

```
  next greater to the right
  a:  2  1  2  4  3
  ans:4  2  4 -1 -1

  walk i = n-1 .. 0, stack holds candidates to the right,
  increasing from top (nearest) to bottom:

  i=4  3    stack [3]
  i=3  4    pop 3 (4>3), stack [4]     ans[3]=-1
  i=2  2    4>2, ans[2]=4, push 2
  i=1  1    2>1, ans[1]=2, push 1
  i=0  2    pop 1, top 2 not greater, pop 2, top 4, ans[0]=4
```

Each index is pushed once and popped once → O(n). Nested loops
are O(n²).

## Stack vs deque

| Pattern | Ends you touch | Container |
| --- | --- | --- |
| next greater / smaller | only the top | `stack` (or `vector` as stack) |
| window maximum | pop front (out of window) **and** pop back (dominated) | `deque` |
| histogram largest rectangle | stack of increasing heights | `stack` |
| min-queue (sliding min) | deque | `deque` |

Using a `deque` for next-greater also works (you only use the
back). Prefer `stack` to document that the front is unused.

## Largest rectangle in histogram

```
  heights:  2 1 5 6 2 3

  stack of increasing heights' indices
  when a lower bar arrives, pop and compute
  width = i - new_top - 1
```

The stack stores **indices**, not values: you need the width.

## Related problems

- Next Greater Element I/II (496, 503) — II is circular: loop
  `2n` with `i % n`
- Daily Temperatures (739)
- Online Stock Span (901) — stack of (price, span)
- Largest Rectangle in Histogram (84), Maximal Rectangle (85)
- Trapping Rain Water (42) — stack of bars, or two pointers
- Remove K Digits / Create Maximum Number — monotonic stack
- Sliding Window Maximum (239) — deque, see
  [`sliding_window.md`](sliding_window.md)
- Shortest unsorted continuous subarray — monotonic from both
  ends

Run [`examples/37_recipe_monotonic.cpp`](../examples/37_recipe_monotonic.cpp).
