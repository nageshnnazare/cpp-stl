# Many-to-many — two hashes of sets (dual index)

**Need:** items have tags. `add_tag(item, tag)`,
`remove_tag`, `tags_of(item)`, `items_with(tag)`, and
**destroy an item** (or a tag) without leaving ghosts on the
other side.

```
  tags_of[7]   = { "cpp", "stl" }
  tags_of[8]   = { "stl" }

  items_of["cpp"] = { 7 }
  items_of["stl"] = { 7, 8 }

  erase_item(7):
      for t in tags_of[7]:  items_of[t].erase(7)
                            if items_of[t] empty: drop t
      tags_of.erase(7)
```

One map is O(1) in one direction and O(n) in the other.
**Destroy is the operation that proves you need both.**

```
  unordered_map<int, unordered_set<string>> tags_of
  unordered_map<string, unordered_set<int>> items_of
```

That is four container **objects** of two types. Each
membership is stored twice on purpose.

## Why not a `vector<pair<item,tag>>`

| Design | add | items_with(tag) | erase_item | Verdict |
| --- | --- | --- | --- | --- |
| vector of edges | O(1) | O(E) | O(E) | |
| only `tags_of` | O(1) | O(#items) | O(#tags) | scan the world |
| **both sides** | O(1) | O(1) + list | O(#tags) exact | **the one** |

CRUD: create/remove a **link** updates two sets. Read either
forward. Destroy a **row** walks one set and deletes from the
other (then drops empty keys so `items_of.size()` is live
tags).

Same shape: users ↔ groups, posts ↔ hashtags, students ↔
courses, graph **undirected** edge index (`adj[u]` and
`adj[v]`).

Full program: [`examples/58_recipe_many_to_many.cpp`](../examples/58_recipe_many_to_many.cpp).

## Related problems

- Design Twitter follow graph — one direction is enough until
  "who follows me?" then you add `followers` as the second
  index ([`news_feed.md`](news_feed.md) stores only
  `following`)
- Inverted index — term → docs, sometimes doc → terms for
  delete-document ([`inverted_index.md`](inverted_index.md))
- Foreign keys in a database: this is the in-memory version
  of two indexes on a join table
