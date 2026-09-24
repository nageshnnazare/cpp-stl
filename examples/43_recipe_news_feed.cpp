// =============================================================================
// WHAT  : News feed: follow graph (hash of sets) + per-user tweet logs + k-way merge.
// WHY   : following is membership (unordered_set). Tweets append in time (vector).
//         Feed is "latest k among me + friends" — max-heap of each author's tip,
//         then walk backward. Merge-k sorted lists, not a global sorted set
//         (that would cost on every post).
// THREAD: post/follow write. feed reads several containers — snapshot under
//         a lock, or don't share a Twitter instance.
// ERASE : unfollow is set.erase(v). Tweets are never deleted here. Heap pop
//         is not "delete a tweet". Walk of following_ is read-only during feed.
// REMEMBER: Each author list is already sorted by time. Pull index i-1 after
//           taking i. Include self via pull(u).
// Guide : recipes/news_feed.md, algorithms/heap.md
// =============================================================================
#include <iostream>
#include <queue>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

class Twitter {
    int clock_ = 0;
    std::unordered_map<int, std::unordered_set<int>> following_;
    std::unordered_map<int, std::vector<std::pair<int, int>>> tweets_;  // {time, id}

public:
    void post(int u, int tweet_id) { tweets_[u].push_back({++clock_, tweet_id}); }

    void follow(int u, int v) {
        if (u != v) following_[u].insert(v);
    }

    void unfollow(int u, int v) { following_[u].erase(v); }

    std::vector<int> feed(int u, int k = 10) {
        using Node = std::tuple<int, int, int, int>;  // time, tweet, author, idx
        std::priority_queue<Node> pq;                 // max-heap by time
        auto pull = [&](int author) {
            auto it = tweets_.find(author);
            if (it == tweets_.end() || it->second.empty()) return;
            int i = (int)it->second.size() - 1;
            auto [t, id] = it->second[i];
            pq.push({t, id, author, i});
        };
        pull(u);
        if (auto it = following_.find(u); it != following_.end())
            for (int v : it->second) pull(v);  // read-only walk of the follow set

        std::vector<int> out;
        while (!pq.empty() && (int)out.size() < k) {
            auto [t, id, author, i] = pq.top();
            pq.pop();
            (void)t;
            out.push_back(id);
            if (i > 0) {
                auto [t2, id2] = tweets_[author][i - 1];
                pq.push({t2, id2, author, i - 1});
            }
        }
        return out;
    }
};

int main() {
    Twitter tw;
    tw.post(1, 5);
    tw.follow(1, 2);
    tw.post(2, 6);
    tw.post(2, 7);
    tw.post(1, 8);
    std::cout << "feed 1:";
    for (int id : tw.feed(1)) std::cout << " " << id;
    std::cout << " (expect 8 7 6 5)\n";
    tw.unfollow(1, 2);
    std::cout << "after unfollow:";
    for (int id : tw.feed(1)) std::cout << " " << id;
    std::cout << " (expect 8 5)\n";
}
