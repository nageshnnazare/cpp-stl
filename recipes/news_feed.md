# News feed — follow graph + per-user tweet lists + a heap

**Need:** `follow` / `unfollow`, `postTweet`, and `getNewsFeed(u)`
= the **10 most recent** tweets from u and the people u follows.

Three nouns, three containers:

```
  following[u]  = unordered_set of user ids     graph
  tweets[u]     = vector of {time, tweetId}     append-only log
  merge         = min/max-heap of list heads    getNewsFeed
```

```
  users     1 follows {2, 3}

  tweets[1]   (t=7, A)
  tweets[2]   (t=5, B) (t=9, C)
  tweets[3]   (t=4, D) (t=8, E)

  feed(1): merge heads C, E, A  ->  C E A  (times 9,8,7)
```

This is **merge k sorted lists** where k = 1 + |followees|
and each list is already newest-last (or newest-first — pick
one and stick to it).

## Why this, not "one global timeline"

| Design | post | feed | Verdict |
| --- | --- | --- | --- |
| one `deque` of all tweets, filter on read | O(1) | O(world) | does not scale |
| store the feed per user, push on every post | O(|followers|) | O(10) | write-heavy; fan-out-on-write |
| **log per author + merge on read** | O(1) | O(k log k) for k followees | fan-out-on-read; classic |
| `set` of all (time, tweet, author) | O(log n) | walk and skip non-followees | O(world) in disguise |

Fan-out-on-write (Twitter's old celebrity problem) is a
`list`/`deque` **per follower**. The STL interview problem
(355) wants fan-out-on-read + a heap.

## Sketch

```cpp
int clock = 0;
std::unordered_map<int, std::unordered_set<int>> following;
std::unordered_map<int, std::vector<std::pair<int,int>>> tweets; // time, id

void post(int u, int id) { tweets[u].push_back({++clock, id}); }
void follow(int u, int v) { if (u != v) following[u].insert(v); }

std::vector<int> feed(int u, int k = 10) {
    using Node = std::tuple<int, int, int, int>; // time, tweet, author, idx
    std::priority_queue<Node> pq;
    auto pull = [&](int author) {
        auto& a = tweets[author];
        if (a.empty()) return;
        int i = (int)a.size() - 1;
        pq.push({a[i].first, a[i].second, author, i});
    };
    pull(u);
    for (int v : following[u]) pull(v);
    std::vector<int> out;
    while (!pq.empty() && (int)out.size() < k) {
        auto [t, id, author, i] = pq.top(); pq.pop();
        out.push_back(id);
        if (i > 0) {
            auto& a = tweets[author];
            pq.push({a[i-1].first, a[i-1].second, author, i-1});
        }
    }
    return out;
}
```

**Why a heap, not `sort` every tweet?** You only need 10. k
followees × 10 heap ops, not the whole history.

**Why `unordered_set` for follows?** Unfollow is O(1); a
`vector` would be O(degree) and could duplicate.

Full program: [`examples/43_recipe_news_feed.cpp`](../examples/43_recipe_news_feed.cpp).

## Related problems

- Design Twitter (355)
- Merge k Sorted Lists (23) — same heap, no graph
- Design Search Autocomplete (642) — different merge (heap of
  hot sentences)
- Feed with "mute" — extra `unordered_set` skip on pop
- Instagram-style fan-out-on-write: `deque` of tweet ids per
  inbox, cap at N, `push_front` on post to each follower
