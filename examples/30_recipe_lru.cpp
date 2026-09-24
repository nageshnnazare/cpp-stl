// =============================================================================
// WHAT  : LRU: list (recency order) + unordered_map (key -> list iterator).
// WHY   : Hash alone cannot move-to-front in O(1). List alone cannot find a
//         key in O(1). Together: get/put O(1). vector would invalidate every
//         stored index on a middle erase — list does not.
// THREAD: Every get splices (write). Two threads on one LRU = data race.
//         Lock the whole pair of containers as one critical section.
// ERASE : Evict: pos.erase(back.key) then pop_back. That list erase does NOT
//         invalidate other iterators sitting in the hash. Range-for + erase
//         on the list would still be UB — we never walk-and-erase.
// REMEMBER: splice(begin, order, it) is O(1) move, not copy. Store iterators,
//           not pointers to nodes you don't own. Capacity check before insert.
// Guide : recipes/lru_cache.md, 03_list_lru.cpp, 02_decision_guide.md
// =============================================================================
#include <iostream>
#include <list>
#include <unordered_map>
#include <utility>

int main() {
    using Node = std::pair<int, int>;
    const int cap = 2;
    std::list<Node> order;  // front = most recent
    std::unordered_map<int, std::list<Node>::iterator> pos;

    auto put = [&](int k, int v) {
        if (auto it = pos.find(k); it != pos.end()) {
            it->second->second = v;                         // update value in place
            order.splice(order.begin(), order, it->second);  // O(1) move-to-front
            return;
        }
        if ((int)order.size() == cap) {
            pos.erase(order.back().first);  // unhook hash BEFORE the node dies
            order.pop_back();
        }
        order.push_front({k, v});
        pos[k] = order.begin();
    };
    auto get = [&](int k) {
        auto it = pos.find(k);
        if (it == pos.end()) return -1;
        order.splice(order.begin(), order, it->second);
        return it->second->second;
    };

    put(1, 1);
    put(2, 2);
    std::cout << "get(1)=" << get(1) << "\n";
    put(3, 3);
    std::cout << "get(2)=" << get(2) << " get(3)=" << get(3) << " get(1)=" << get(1) << "\n";
}
