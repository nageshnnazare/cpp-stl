# `std::forward_list` — one pointer per node

Singly linked. Smaller than `list`, weaker API. You insert
**after** a position because there is no `prev`.

```
  before_begin()     begin()
        |              |
        v              v
     sentinel --> [ A |*] --> [ B |*] --> [ C |*] --> null
```

Header: `<forward_list>`. Iterator: **forward** only.
There is **no** `size()`, `back()`, `--it`, or `push_back`.

## CRUD

| | Methods | Cost |
| --- | --- | --- |
| C | `push_front`, `insert_after(it, x)`, `emplace_after` | O(1) |
| R | `front`; walk `++` | O(1) / O(n) |
| U | `*it =` | O(1) |
| D | `pop_front`, `erase_after(it)`, `remove` | O(1) after `it` |

`before_begin()` is the handle for "insert at front via
`insert_after`". Forgetting it is the first compile error.

## Job — separate chaining you control

A tiny hash table, or "stack of frames" that only ever pushes
and pops the head:

```
  bucket[h] --> [k1|*] --> [k2|*] --> null
  insert:  insert_after(before_begin of that list)
  find:    walk, O(chain length)
```

In real code you usually want `unordered_map`. `forward_list`
shows up when you cannot pay two pointers per node or you are
implementing a container yourself.

## Why this, not that

| Alternative | Prefer it when |
| --- | --- |
| `vector` | almost always |
| `list` | you need `--it`, `push_back`, or `size()` |
| `forward_list` | memory is the product constraint and the walk is forward-only |

## Related problems

- Reverse a singly linked list (member `reverse()` is O(n))
- Detect a cycle (Floyd) — practice on raw nodes; `forward_list`
  does not give you "a random middle pointer" cheaply
- Merge two sorted lists (`merge` member, like `list`)

Run [`examples/04_forward_list_chain.cpp`](../examples/04_forward_list_chain.cpp).
