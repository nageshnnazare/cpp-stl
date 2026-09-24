# `std::string_view` — `span` for text

A `string_view` is `{pointer, length}` into **someone else's**
characters. It does not allocate, own, or zero-terminate
(it may point at a slice in the middle of a `string`).

```
  string s = "prefix-BODY-suffix";
  string_view body{s.data() + 7, 4};     // "BODY"
  string_view also = std::string_view(s).substr(7, 4);

  s += "....lots....";    // may realloc  => body dangling
```

Header: `<string_view>`. Iterator: contiguous, **const**.
C++17. This is the text twin of [`span.md`](span.md).

## CRUD

| | Methods | Cost |
| --- | --- | --- |
| C | `string_view(s)`, `{p, n}`, `substr`, `remove_prefix` / `remove_suffix` | O(1) |
| R | `[]`, `data`, `size`, `find`, `starts_with`, `ends_with`, `compare` | `find` O(n·m) |
| U | you cannot change characters through a view | — |
| D | not yours; `remove_prefix` only shrinks **the view** | O(1) |

`substr` on a view does **not** copy characters (unlike
`string::substr`). Returning a `string_view` to a function-local
`string` is a dangling view — return `string` instead.

## Job — parse without copies

Tokenize, strip, pass a substring into a hash map with a
**transparent hasher** ([`05_hash_compare_lookup.md`](../05_hash_compare_lookup.md))
so lookup does not build a `string`.

**Why not `const string&`?** A literal `"foo"` can bind to
`string_view` without allocating; `const string&` from a
literal constructs a temporary `string`. **Why not `const char*`?**
The length is not attached; embedded NULs exist in real data.

**Why not store `string_view` in a struct for later?** Only if
the owner is guaranteed to outlive it (string table, mmap,
`static` storage). Caches and containers should store `string`.

SSO trap: a short `string` lives inside the `string` object.
A view into it dangles when the `string` is **moved** (the
small buffer moves with the object — actually SSO move may
keep data in the destination object and leave the source
empty; the view still points at the **old object's inline
buffer**). Treat any view as invalid after the owner is moved
from or destroyed.

## Related

- [`string.md`](string.md), [`span.md`](span.md)
- Longest substring problems: views of the original tape
- `starts_with` / `ends_with` (C++20) on both `string` and view

Run the split example in [`examples/06_string_split_window.cpp`](../examples/06_string_split_window.cpp).
