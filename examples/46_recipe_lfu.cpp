// =============================================================================
// WHAT  : LFU cache: hash key→list iterator + hash freq→list (LRU within that freq).
// WHY   : Need O(1) get/put, evict least-frequent, ties broken by LRU.
//         One list is LRU only. A heap of frequencies cannot bump in O(1)
//         (no handle). Splice a node from bucket[f] to bucket[f+1] — list
//         iterators in pos_ stay valid (that is the whole point of list).
// THREAD: get splices. Lock the whole cache.
// ERASE : Evict: pos_.erase(front.key) then pop_front. splice moves without
//         invalidating the moved iterator. Empty freq list: bucket_.erase(f).
//         Never range-for a bucket and erase inside.
// REMEMBER: minf_ is the smallest live frequency. After bumping everyone
//           out of minf_, increment it. New keys start at freq 1.
// Guide : recipes/lfu_cache.md, 03_list_lru.cpp
// =============================================================================
#include <iostream>
#include <iterator>
#include <list>
#include <unordered_map>

class LFUCache {
    struct Node {
        int key, val, f;
    };
    int cap_;
    int minf_ = 0;
    std::unordered_map<int, std::list<Node>> bucket_;  // freq -> LRU..MRU
    std::unordered_map<int, std::list<Node>::iterator> pos_;

    void bump(std::list<Node>::iterator it) {
        int f = it->f;
        auto& dst = bucket_[f + 1];
        dst.splice(dst.end(), bucket_[f], it);  // O(1); it remains valid
        it->f++;
        if (bucket_[f].empty()) {
            bucket_.erase(f);
            if (minf_ == f) ++minf_;
        }
    }

public:
    explicit LFUCache(int cap) : cap_(cap) {}

    int get(int key) {
        auto it = pos_.find(key);
        if (it == pos_.end()) return -1;
        bump(it->second);
        return it->second->val;
    }

    void put(int key, int val) {
        if (cap_ == 0) return;
        if (auto it = pos_.find(key); it != pos_.end()) {
            it->second->val = val;
            bump(it->second);
            return;
        }
        if ((int)pos_.size() == cap_) {
            auto& lst = bucket_[minf_];
            pos_.erase(lst.front().key);  // unhook hash before the node dies
            lst.pop_front();
            if (lst.empty()) bucket_.erase(minf_);
        }
        bucket_[1].push_back({key, val, 1});
        pos_[key] = std::prev(bucket_[1].end());
        minf_ = 1;
    }
};

int main() {
    LFUCache c(2);
    c.put(1, 1);
    c.put(2, 2);
    std::cout << "get(1)=" << c.get(1) << "\n";
    c.put(3, 3);  // evicts 2 (LFU)
    std::cout << "get(2)=" << c.get(2) << " (expect -1)\n";
    std::cout << "get(3)=" << c.get(3) << "\n";
    c.put(4, 4);  // evicts 1 (freq 2 vs 3's freq 2: 1 is LRU among them)
    std::cout << "get(1)=" << c.get(1) << " (expect -1)\n";
    std::cout << "get(3)=" << c.get(3) << " get(4)=" << c.get(4) << "\n";
}
