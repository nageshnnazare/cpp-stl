// =============================================================================
// WHAT  : Leaderboard: hash id→score (O(1) lookup) + set{-score,id} (ordered rank).
// WHY   : Heap cannot reset/correct a score (no erase-by-id). map<id,score>
//         cannot walk "top k" without scanning everyone. Two indexes: one
//         promise per container. Destroy must unhook BOTH.
// THREAD: add_score writes both. top_sum is a const walk — OK vs other const
//         reads, data race vs add_score. Lock the pair as one unit.
// ERASE : reset: ranked.erase({-old,id}) then score.erase. We erase by key,
//         not while range-for. add_score: erase old pair then insert new —
//         never mutate *it of a set (key is const).
// REMEMBER: Negative score so begin() is the winner. Tie-break is smaller id.
//           Update = delete old node + insert new node (extract would work too).
// Guide : recipes/leaderboard.md, 02_decision_guide.md (3+ indexes)
// =============================================================================
#include <iostream>
#include <set>
#include <unordered_map>
#include <utility>
#include <vector>

class Leaderboard {
    std::unordered_map<int, int> score_;
    std::set<std::pair<int, int>> ranked_;  // {-score, id}

public:
    void add_score(int id, int delta) {
        if (auto it = score_.find(id); it != score_.end())
            ranked_.erase({-it->second, id});  // unhook old rank before the value changes
        score_[id] += delta;
        ranked_.insert({-score_[id], id});
    }

    void reset(int id) {
        auto it = score_.find(id);
        if (it == score_.end()) return;
        ranked_.erase({-it->second, id});
        score_.erase(it);  // iterator erase — it is not used after
    }

    int top_sum(int k) const {
        int s = 0;
        for (auto it = ranked_.begin(); k > 0 && it != ranked_.end(); ++it, --k)
            s += -it->first;  // read-only walk; no erase
        return s;
    }

    std::vector<int> top_ids(int k) const {
        std::vector<int> ids;
        for (auto it = ranked_.begin(); k > 0 && it != ranked_.end(); ++it, --k)
            ids.push_back(it->second);
        return ids;
    }
};

int main() {
    Leaderboard b;
    b.add_score(1, 73);
    b.add_score(2, 56);
    b.add_score(3, 39);
    b.add_score(4, 51);
    b.add_score(5, 4);
    std::cout << "top(1) sum=" << b.top_sum(1) << " (expect 73)\n";
    std::cout << "top ids:";
    for (int id : b.top_ids(3)) std::cout << " " << id;
    std::cout << "\n";

    b.reset(1);
    b.reset(2);
    b.add_score(3, 10);  // 49
    std::cout << "after reset 1,2 and +10 to 3: top(3) sum=" << b.top_sum(3)
              << " (expect 104)\n";

    Leaderboard t;
    t.add_score(9, 10);
    t.add_score(2, 10);
    std::cout << "tie ids:";
    for (int id : t.top_ids(2)) std::cout << " " << id;
    std::cout << " (expect 2 9)\n";
}
