# Leaderboard — `unordered_map` + ordered `set`

**Need:** update a player's score, reset them, and read the **top
K** (or the rank of one id) while scores change online.

A hash table finds an id in O(1) but cannot walk "best first".
A sorted set walks best-first but cannot find an id without
knowing the current `(score, id)` key.

```
  score:   alice=50  bob=80  cara=80  dan=20

  ordered set of  {-score, id}     begin() = best
           (-80, bob)  (-80, cara)  (-50, alice)  (-20, dan)
               ^
               top(1) = bob     (tie: smaller id first)

  addScore(alice, +40):
      erase (-50, alice) from set
      score[alice] = 90
      insert (-90, alice)
           (-90, alice)  (-80, bob)  (-80, cara)  (-20, dan)
```

## Why these two, not the alternatives

| Design | update | top(K) | find id | Verdict |
| --- | --- | --- | --- | --- |
| `vector<pair>` + `sort` each query | O(1) | O(n log n) | O(n) | fine if n is tiny |
| `priority_queue` of (score, id) | O(log n) push | O(K log n) | stale scores | lazy-delete works; reset is messy |
| `map<id,score>` only | O(log n) | O(n) scan | O(log n) | no order by score |
| **`unordered_map<id,score>` + `set{-score,id}`** | O(log n) | O(K) | O(1) | **the one** |
| Fenwick / policy tree on compressed ranks | O(log n) | O(log n) rank | O(1)+log | when you need **rank of id** in O(log n) |

`top(K)` on a `set` is "walk `begin` K times". **Rank of an
arbitrary id** (`distance(begin, find)`) is **O(n)** on a
standard `set`. If that query is hot, this recipe is the wrong
shape — you want an order-statistic tree or a Fenwick tree on
compressed scores. Be honest about that in interviews.

`{-score, id}` in a default `set` gives **score desc, id asc**
without a custom comparator. `set<pair, greater<>>` would also
flip the id order; do not use it unless you want larger ids
first.

## Sketch

```cpp
std::unordered_map<int, int> score;          // id -> points
std::set<std::pair<int, int>> ranked;        // {-score, id}

void add_score(int id, int delta) {
    if (auto it = score.find(id); it != score.end())
        ranked.erase({-it->second, id});
    score[id] += delta;
    ranked.insert({-score[id], id});
}

int top_k_sum(int k) {
    int s = 0;
    for (auto it = ranked.begin(); k > 0 && it != ranked.end(); ++it, --k)
        s += -it->first;
    return s;
}

void reset(int id) {
    auto it = score.find(id);
    if (it == score.end()) return;
    ranked.erase({-it->second, id});
    score.erase(it);
}
```

Full program: [`examples/40_recipe_leaderboard.cpp`](../examples/40_recipe_leaderboard.cpp).

## Variants you will actually ship

**Per-category boards** (highest-rated food per cuisine, per-map
CS rankings):

```
  unordered_map<cuisine, set{-rating, name}>
  unordered_map<name, {cuisine, rating}>
```

Same two-structure idea, nested one level. Changing a rating is
erase old pair + insert new pair in that cuisine's set.

**Dense 1..n players, scores in 0..S with S small:** array of
counts + Fenwick. The `set` is for sparse ids and arbitrary
score ranges.

## Related problems

- Design A Leaderboard (1244)
- Design a Food Rating System (2353) — board **per cuisine**
- Rank Teams by Votes (1366) — static: `vector` + `sort` with a
  custom comparator (no online set)
- Relative Ranks (506) — sort once
- The Number of the Smallest Unoccupied Chair — `set` of free
  chairs + min-heap of leaving times (seating, not scores)
- Exam Room (855) — `set` of occupied seats, max gap
- Max Stack (716) — see also [`freq_stack.md`](freq_stack.md);
  a `set` of `(value, timestamp)` plus a `vector` of ids

If the interview is "print the top 10 once after all updates",
dump `score` into a `vector<pair>` and `partial_sort`. The dual
structure is for **interleaved** updates and queries.
