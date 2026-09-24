# Autocomplete — `map` prefix walk, or a trie of maps

**Need:** given a prefix, return the hottest / lexicographically
first completions.

```
  dictionary:  cat 3, car 5, card 1, dog 2
  prefix "ca":   car, cat, card
```

Two honest designs. Pick by **whether prefixes are the hot
path** and how big the dictionary is.

## Design A — `map<string, int>` (sorted words)

```
  map:
    car:5
    card:1
    cat:3
    dog:2

  lower_bound("ca") -> car
  walk while key has prefix "ca"
```

**Why `map` not `unordered_map`?** You need the **successor**
of the prefix string. Hash tables have no `lower_bound`.

Top-k among the matches: dump into a heap of size k, or
`partial_sort`. Do not sort the whole dictionary on each
keystroke if only ~dozens of words share the prefix.

Good until the walk visits too many words (`"a"` on an English
dump). Then you want a trie.

## Design B — trie, `array` or `map` of children

```
            * 
          /   \
         c     d
         |     |
         a     o
        / \    |
       r   t   g
       |
       d
```

```cpp
struct Node {
    std::array<std::unique_ptr<Node>, 26> ch;
    bool end = false;
    int freq = 0;
};
```

Sparse alphabets (URLs, unicode): `unordered_map<char,
unique_ptr<Node>>` or `map<char, ...>` per node. ASCII letters:
`array<...,26>` wins.

Store at each node either:

- nothing extra — DFS the subtree on query (simple), or
- a heap / cached top-k of sentences that pass through
  (LeetCode 642; faster queries, heavier inserts)

## Why not a vector + `std::search`

| Design | lookup | prefix range | Notes |
| --- | --- | --- | --- |
| `vector<string>` + scan | O(n) | O(n) | tiny n |
| sort vector + two `lower_bound` | O(log n + hits) | static dict | rebuild on insert |
| **`map<string,int>`** | O(log n + hits) | online inserts | **start here** |
| **trie** | O(\|prefix\| + hits) | hot prefixes | production search |
| inverted index | terms, not prefixes | [`inverted_index.md`](inverted_index.md) | |

`lower_bound("ca")` and `lower_bound("ca" + '{')` (or a
successor prefix) can bound the range if you only want the
slice. Walking until the prefix fails is simpler.

## Sketch (map)

```cpp
std::map<std::string, int> dict;   // word -> freq

std::vector<std::string> complete(const std::string& p, int k) {
    std::vector<std::pair<int, std::string>> cand; // -freq, word
    for (auto it = dict.lower_bound(p);
         it != dict.end() && it->first.compare(0, p.size(), p) == 0;
         ++it)
        cand.push_back({-it->second, it->first});
    int take = std::min(k, (int)cand.size());
    std::partial_sort(cand.begin(), cand.begin() + take, cand.end());
    std::vector<std::string> out;
    for (int i = 0; i < take; ++i) out.push_back(cand[i].second);
    return out;
}
```

Full program (map + a small trie): [`examples/47_recipe_autocomplete.cpp`](../examples/47_recipe_autocomplete.cpp).

## Related problems

- Design Search Autocomplete System (642)
- Implement Trie (208), Replace Words (648)
- Search Suggestions System (1268) — sort once, binary search
  each growing prefix
- Longest Common Prefix — sort the `vector`, compare first and
  last (no trie)
- Word Search II (212) — trie + backtracking on a board
- Prefix and suffix search (745) — wrap `"suffix#word"` in a
  trie
