# Inverted index — `unordered_map` of posting lists

Search, tags, "which docs contain this word?": invert
doc→words into word→docs.

```
  doc 1:  cat dog
  doc 2:  cat bird
  doc 3:  dog

  inverted:
  cat  ->  [1, 2]
  dog  ->  [1, 3]
  bird ->  [2]
```

## Choose the posting-list type

| Need | `mapped_type` | Why |
| --- | --- | --- |
| append-only, then AND/OR with two pointers | `vector<int>` **kept sorted** | `set_intersection` |
| unique docs, online insert, need order | `set<int>` | O(log n) insert, already sorted |
| unique, no order | `unordered_set<int>` | membership "is doc in this term?" |
| positions in the doc (phrase search) | `vector<int>` or `map<int, vector<int>>` | extra coordinate |

Building from a static corpus: `unordered_map<string,
vector<int>>`, push doc ids in increasing order (group by doc
first) so vectors stay sorted for free.

AND query: `set_intersection` on two sorted vectors — see
[`algorithms/set_operations.md`](../algorithms/set_operations.md).
OR: `set_union`. NOT: `set_difference` against the universe
vector of all ids.

```
  query: cat AND dog

  cat:  1, 2
  dog:  1, 3
  intersect: 1
```

## Why not one `map<string, set<int>>` always

A `map` (tree) on **words** sorts terms; useful for prefix
autocomplete (`lower_bound("ca")` walk while prefix matches).
If you only look up whole words, `unordered_map` is the lookup.
If you need prefixes, either:

- `map<string, ...>` (simple), or
- a trie: `struct Node { array<unique_ptr<Node>,26> ch;
  vector<int> docs; }` — more code, faster prefix.

Autocomplete-hot: trie. Sparse prefixes, small n: `map`.

## Related problems

- Design search autocomplete system (642) — trie + heap of
  sentences
- Time Based Key-Value Store — `unordered_map<string,
  vector<pair<int,string>>>` + `upper_bound` on time
- Snapshot / versioned KV
- Hashtag inverted index, inverted file in information retrieval
- Range frequency queries (Fenwick / segment on compressed
  values — next level after this recipe)

Run [`examples/38_recipe_inverted_index.cpp`](../examples/38_recipe_inverted_index.cpp).
