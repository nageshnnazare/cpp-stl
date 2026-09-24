# `std::span` — a pointer and a length, not a container

`span<T>` borrows memory. It does not allocate, grow, or destroy
elements. Think `string_view` for arbitrary `T`.

```
  vector<int> v{1,2,3,4,5};
  span<int>        all  = v;            // ptr=v.data(), len=5
  span<int>        mid  = all.subspan(1, 3);   // [2,3,4]
  span<const int>  view = all;          // read-only

  v.push_back(6);   // may realloc  => all, mid dangling
```

Header: `<span>`. Iterator: **contiguous**. C++20.

## CRUD

| | Methods | Cost |
| --- | --- | --- |
| C | `span(cont)`, `span(ptr, n)`, `first<N>()`, `subspan` | O(1) |
| R | `[]`, `front`, `back`, `data`, `size` | O(1) |
| U | `s[i]=` if `T` is not const | O(1) |
| D | not applicable | — |

Dynamic extent (`span<T>`) vs static (`span<T, 4>`): the length
is in the type, like `array`.

## Job — one function, many callers

```cpp
int sum(std::span<const int> xs) {
    int s = 0;
    for (int x : xs) s += x;
    return s;
}

int a[] = {1, 2, 3};
std::array<int, 2> b{4, 5};
std::vector<int> c{6, 7, 8};
sum(a); sum(b); sum(c);          // all legal
```

**Why not `const vector<int>&`?** Callers with `array` or C
arrays would copy into a vector. **Why not `const int*, n`?**
`span` keeps the length attached and can slice.

**Why not store a `span` in a struct for later?** Only if you
prove the owner outlives it. Prefer storing the `vector`.

## Why this, not that

| Alternative | Prefer span | Prefer the other |
| --- | --- | --- |
| `vector&` | callee must not own / grow | callee will `push_back` |
| `string_view` | bytes / text | `span<const char>` is similar; `string_view` has find |
| `array` | you do not own | you own a fixed buffer |

## Related problems

- APIs: hashing a slice, summing a window, passing a matrix row
- Avoiding copies in recursion on a subarray (`span` the left
  and right halves) — still dangling if the vector dies

Run [`examples/14_span_api.cpp`](../examples/14_span_api.cpp).
