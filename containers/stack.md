# `std::stack` — LIFO, nothing else

A stack is an adapter. Default inner container: `deque`.
You may pass `vector` as the second template argument if you
never need the extra chunk logic.

```
           top
            |
            v
         +-----+
         |  )  |   push
         |  (  |
         |  {  |
         +-----+
            |
           pop
```

Header: `<stack>`. **No iterators.** If you need to walk the
stack, use a `vector` and treat `back()` as top.

## CRUD

| | Methods | Cost |
| --- | --- | --- |
| C | `push`, `emplace` | O(1) (A if inner is vector) |
| R | `top`, `empty`, `size` | O(1) |
| U | `top() = x` | O(1) |
| D | `pop` | O(1)  (`pop` returns `void`) |

```cpp
auto x = st.top(); st.pop();    // not pop() -> x
```

## Job 1 — matching delimiters / parse

```
  input:  { [ ] ( ) }

  {   stack: {
  [   stack: { [
  ]   pop [, matches
  (   stack: { (
  )   pop (, matches
  }   pop {, matches
  empty => ok
```

Related: Valid Parentheses, Score of Parentheses, Longest Valid
Parentheses, Decode String, Basic Calculator.

## Job 2 — monotonic stack (next greater / daily temperatures)

You walk left-to-right and pop while the stack top is **less
useful** than the new value. What remains is increasing or
decreasing by invariant.

```
  temps:  73 74 75 71 69 72 76 73

  stack of indices, decreasing temps:
  73
  73 popped by 74     answer[0]=1
  74 popped by 75     answer[1]=1
  75
  75 71 69
  69,71 popped by 72  ...
```

**Why a stack, not a `deque`?** You only touch one end. The
name documents the invariant. See
[`recipes/monotonic_stack_queue.md`](../recipes/monotonic_stack_queue.md).

## Job 3 — DFS / undo / recursion you want on the heap

```
  explicit DFS:
  stack<Node*> st; st.push(root);
  while (!st.empty()) { auto* u = st.top(); st.pop(); ... push children }
```

Recursion **is** a stack. Use `std::stack` when depth may blow
the call stack or you need to pause.

## Why this, not that

| Alternative | Use stack | Use the other |
| --- | --- | --- |
| `vector` + `back` | you want a narrow API | you also need to iterate / index |
| `deque` | LIFO only | both ends (monotonic *queue*) |
| recursion | depth / iterative control | simple tree walk, small depth |

## Related problems

- Valid Parentheses (20), Min Stack (155)
- Daily Temperatures (739), Next Greater Element
- Largest Rectangle in Histogram (84)
- Asteroid Collision, Remove K Digits (monotonic)
- Binary tree inorder iterative
- Evaluate RPN (150)

Run [`examples/11_stack_parens_next_greater.cpp`](../examples/11_stack_parens_next_greater.cpp).
