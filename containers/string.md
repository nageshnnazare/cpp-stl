# `std::string` — a `vector<char>` that speaks text

Treat `string` as a sequence container first. The extra API
(`find`, `substr`, `+`, `compare`) sits on the same contiguous
buffer. Short strings usually live **inside** the object (SSO):
no heap until the text outgrows a dozen or so bytes (library
dependent, often 15–22).

```
  SSO (short):     object: [ "hi" | size=2 | ...inline... ]

  long:            object: { ptr, size=27, cap=31 }
                   heap:   [ H e l l o   w o r l d  ... ]
```

Header: `<string>`. Also useful: `string_view` (`<string_view>`)
as a non-owning slice (the `span` of text).

## CRUD

| | Methods | Cost |
| --- | --- | --- |
| C | `string(n, c)`, `+`, `append`, `push_back`, `insert` | end A O(1); middle O(n) |
| R | `[]`, `at`, `substr`, `find`, `rfind`, `starts_with`, `ends_with`, `compare` | `find` typically O(n·m) |
| U | `s[i]=`, `replace` | replace may shift O(n) |
| D | `erase`, `pop_back`, `clear` | same as vector |

`substr` **copies**. For read-only slices use `string_view`.

```
  string     s = "prefix-BODY-suffix";
  string_view body = std::string_view(s).substr(7, 4);   // no copy
  // body dies if s reallocates or dies
```

## Job 1 — tokenize without reinventing find

```cpp
std::vector<std::string_view> split(std::string_view s, char delim) {
    std::vector<std::string_view> out;
    for (size_t i = 0; i < s.size(); ) {
        auto j = s.find(delim, i);
        if (j == std::string_view::npos) j = s.size();
        out.emplace_back(s.substr(i, j - i));
        i = j + 1;
    }
    return out;
}
```

**Why not `stringstream` + `>>`?** Fine for simple whitespace.
`find` is explicit about the delimiter and does not allocate a
stream. **Why not `vector<string>` tokens?** `string_view` tokens
are valid only while `s` lives — return `vector<string>` if the
function must own them.

## Job 2 — sliding character window (anagram / coverage)

```
  s = "cbaebabacd"   p = "abc"
  need: {a:1, b:1, c:1}
  window counts; when they match, record start

  [c b a] e b a b a c d
   ^match^
```

Counts fit in `array<int,26>` or `unordered_map<char,int>`. The
string is just the tape you slide over. See
[`recipes/sliding_window.md`](../recipes/sliding_window.md).

## Job 3 — build vs `+` in a loop

```cpp
std::string acc;
acc.reserve(n);                 // if you know n
for (auto& piece : pieces) acc += piece;   // A O(1) per char
```

`acc = acc + piece` may allocate a **new** string every time
(O(n²)). `+=` / `append` amortizes.

## Why this, not that

| Alternative | When to use it |
| --- | --- |
| `vector<char>` | binary blobs, you want to `push_back` bytes without text API |
| `string_view` | read-only parameter, substring without copy |
| `rope` / piece table | huge edits in the middle (not in the STL) |
| `ostringstream` | many heterogeneous `<<` insertions |

Never store `string_view` / `span` into a string that might
reallocate (SSO → heap transition also moves the buffer).

## Related problems

- Valid anagram, group anagrams, longest substring without repeat
- Longest common prefix (sort the vector of strings; compare first
  and last — or a `map` trie)
- Multiply strings, add binary (schoolbook on `string` digits)
- Decode string / reverse words
- Implement `strStr` (`std::search` or `s.find`)

Run [`examples/06_string_split_window.cpp`](../examples/06_string_split_window.cpp).
