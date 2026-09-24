// =============================================================================
// WHAT  : LRU cache — list (recency) + unordered_map of list iterators (lookup).
// WHY   : Hash alone has no order. List alone has no O(1) find. splice moves a
//         node to MRU in O(1) without invalidating other list iterators — so
//         the hash's stored iterators stay valid. deque/vector cannot do that
//         (middle erase is O(n) and kills iterators).
// THREAD: Not thread-safe. get() mutates order (splice). Two threads get()
//         without a mutex = data race. Wrap the whole cache or don't share it.
// ERASE : list::erase / pop_back invalidates ONLY that node. Hash erase is
//         O(1) avg. Do not range-for the list and erase the current it without
//         `it = order_.erase(it)`. Eviction here uses pop_back (the LRU end).
// REMEMBER: Capacity full → delete LRU from BOTH structures. Forget the hash
//           and you leak a dangling iterator on the next get of that key.
// Guide : recipes/lru_cache.md, containers/list.md, 02_decision_guide.md
// =============================================================================
#include <iostream>
#include <list>
#include <unordered_map>
#include <utility>

class LRUCache {
    using Node = std::pair<int, int>;
    int cap_;
    std::list<Node> order_;  // MRU at begin, LRU at end
    std::unordered_map<int, std::list<Node>::iterator> pos_;

    void touch(std::list<Node>::iterator it) {
        // O(1) relink. `it` still names the same node afterwards.
        order_.splice(order_.begin(), order_, it);
    }

public:
    explicit LRUCache(int cap) : cap_(cap) {}

    int get(int k) {
        auto it = pos_.find(k);  // find, not [] — [] would insert a dummy
        if (it == pos_.end()) return -1;
        touch(it->second);
        return it->second->second;
    }

    void put(int k, int v) {
        if (auto it = pos_.find(k); it != pos_.end()) {
            it->second->second = v;  // mutate value in place; key stays
            touch(it->second);
            return;
        }
        if ((int)order_.size() == cap_) {
            pos_.erase(order_.back().first);  // both indexes, then the node
            order_.pop_back();
        }
        order_.push_front({k, v});
        pos_[k] = order_.begin();
    }

    void dump() const {
        // const method: concurrent dumps would be OK if no get/put ran.
        std::cout << "MRU";
        for (auto [k, v] : order_) std::cout << " (" << k << ":" << v << ")";
        std::cout << " LRU\n";
    }
};

int main() {
    LRUCache c(2);
    c.put(1, 10);
    c.put(2, 20);
    c.dump();
    std::cout << "get(1)=" << c.get(1) << "\n";
    c.dump();
    c.put(3, 30);  // evicts key 2 (LRU after get(1) promoted 1)
    c.dump();
    std::cout << "get(2)=" << c.get(2) << " (expect -1)\n";
}
