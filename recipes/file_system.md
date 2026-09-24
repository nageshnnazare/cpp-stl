# In-memory file system — a trie of `map`s

**Need:** `mkdir`, `ls`, `write` (create/append a file),
`read`, `rm` a file or empty directory. Paths are nested.
`ls` on a directory is **sorted**.

```
  /
  ├── a/            map children, key order = ls order
  │   ├── b.txt     file node, holds string content
  │   └── c/
  └── d.txt
```

```
  struct Node {
      std::map<std::string, std::unique_ptr<Node>> ch;
      std::string data;
      bool is_file = false;
  };
```

`map` not `unordered_map`: `ls` walks `ch` in lex order for
free. Sparse names; an `array<26>` trie is the wrong alphabet.

Each path walk is a chain of `map::operator[]` / `find`. That
is several containers **nested**, not side by side — still
more than two lookups (parent dir, child name, file buffer).

## CRUD on a path

| Verb | How |
| --- | --- |
| C mkdir | walk/create dirs; last must not be a file |
| C write | mkdir parents, create file node, `data +=` |
| R ls | if file, return its name; else keys of `ch` |
| R read | walk, return `data` |
| D rm | `parent.ch.erase(name)` after checks (file, or empty dir) |

Destroy is `map::erase` of one child. The unique_ptr frees
the subtree (do **not** rm a non-empty dir unless you mean
`rm -r`, which is erase of the child and done).

## Why not `unordered_map<string path, string content>`

A flat map of full paths makes `ls /a` an O(#files) prefix
scan, and `rm -r` a prefix delete. The trie makes directory
ops proportional to **directory size**, which is the POSIX
cost model. Use a flat map if the problem never lists a
directory (LeetCode 1166 "Design File System" is parent-must-
exist, still walkable as a flat map of paths).

Full program: [`examples/59_recipe_file_system.cpp`](../examples/59_recipe_file_system.cpp).

## Related problems

- Design In-Memory File System (588)
- Design File System (1166) — create path with value, parent
  must exist
- [`autocomplete.md`](autocomplete.md) — trie of letters, not
  path segments
- Nested JSON / config: `map<string, Node>` with variant
  children
