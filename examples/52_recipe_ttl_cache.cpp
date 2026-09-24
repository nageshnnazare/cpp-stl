// =============================================================================
// WHAT  : TTL LRU: recency list + key→iterator hash + set{expiry, key}.
// WHY   : Three promises: O(1) get (hash+splice), O(1) LRU evict (list back),
//         O(log n) "what expired?" (set begin). A heap of expiries cannot
//         cancel/refresh a key's TTL (no erase-by-key).
// THREAD: get splices and may purge. Lock the triple.
// ERASE : kill(it) unhooks ALL three indexes, then list.erase(it). purge walks
//         expiry_.begin() while expired — but does not range-for+erase; it
//         calls kill which erases begin's key. After kill, begin() is the next.
// REMEMBER: refresh TTL = erase old {exp,key} then insert new. Capacity evict
//           is LRU (list back), not TTL. purge before get/put so expired
//           slots free capacity.
// Guide : recipes/ttl_cache.md, recipes/three_plus.md
// =============================================================================
#include <iostream>
#include <iterator>
#include <list>
#include <set>
#include <unordered_map>
#include <utility>

class TTLCache {
    struct Node {
        int key, val;
        long long exp;
    };
    int cap_;
    std::list<Node> lru_;
    std::unordered_map<int, std::list<Node>::iterator> pos_;
    std::set<std::pair<long long, int>> expiry_;

    void kill(std::list<Node>::iterator it) {
        expiry_.erase({it->exp, it->key});
        pos_.erase(it->key);
        lru_.erase(it);  // other list iterators in pos_ stay valid
    }

    void purge(long long now) {
        while (!expiry_.empty() && expiry_.begin()->first <= now)
            kill(pos_[expiry_.begin()->second]);
    }

public:
    explicit TTLCache(int cap) : cap_(cap) {}

    int get(int key, long long now) {
        purge(now);
        auto it = pos_.find(key);
        if (it == pos_.end()) return -1;
        lru_.splice(lru_.begin(), lru_, it->second);
        return it->second->val;
    }

    void put(int key, int val, long long now, long long ttl) {
        purge(now);
        if (auto it = pos_.find(key); it != pos_.end()) {
            expiry_.erase({it->second->exp, key});  // unhook old expiry before changing
            it->second->val = val;
            it->second->exp = now + ttl;
            expiry_.insert({it->second->exp, key});
            lru_.splice(lru_.begin(), lru_, it->second);
            return;
        }
        if (cap_ == 0) return;
        if ((int)pos_.size() == cap_) kill(std::prev(lru_.end()));
        lru_.push_front({key, val, now + ttl});
        pos_[key] = lru_.begin();
        expiry_.insert({now + ttl, key});
    }
};

int main() {
    TTLCache c(2);
    c.put(1, 10, 0, 5);   // dies at 5
    c.put(2, 20, 1, 100); // dies at 101
    std::cout << "t=2 get 1=" << c.get(1, 2) << "\n";
    std::cout << "t=5 get 1=" << c.get(1, 5) << " (expired, expect -1)\n";
    c.put(3, 30, 6, 10);  // 1 already gone; cap 2: {2,3}
    std::cout << "t=6 get 2=" << c.get(2, 6) << " get 3=" << c.get(3, 6) << "\n";
    c.put(4, 40, 7, 10);  // LRU evicts 2 (3 was touched)
    std::cout << "t=7 get 2=" << c.get(2, 7) << " (LRU evicted, expect -1) get 3="
              << c.get(3, 7) << " get 4=" << c.get(4, 7) << "\n";
}
