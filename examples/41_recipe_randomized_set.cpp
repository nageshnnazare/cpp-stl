// =============================================================================
// WHAT  : O(1) insert / erase / getRandom: vector of values + hash value→index.
// WHY   : unordered_set has O(1) insert/erase but cannot index a random slot.
//         vector has random index but erase(i) is O(n). Fix: swap-with-last
//         then pop_back — O(1) when order does not matter. Hash tracks the hole.
// THREAD: insert/erase write both containers. get_random reads the vector —
//         data race vs erase. Lock, or don't share.
// ERASE : Not it=erase(it). We overwrite a_[i] with the last element, fix
//         that element's index in the hash, pop_back, then at_.erase(x).
//         Erasing x from the hash AFTER moving last avoids the "erase last
//         while it is still the victim" footgun when x is already at the back.
// REMEMBER: at_[last]=i even when last==x (self-move). pop_back after the swap.
// Guide : recipes/randomized_set.md, 02_decision_guide.md (swap-pop)
// =============================================================================
#include <iostream>
#include <random>
#include <unordered_map>
#include <vector>

class RandomizedSet {
    std::vector<int> a_;
    std::unordered_map<int, int> at_;
    std::mt19937 rng_{42};

public:
    bool insert(int x) {
        if (at_.count(x)) return false;
        at_[x] = (int)a_.size();
        a_.push_back(x);
        return true;
    }

    bool erase(int x) {
        auto it = at_.find(x);
        if (it == at_.end()) return false;
        int i = it->second;
        int last = a_.back();
        a_[i] = last;     // order does not matter — fill the hole
        at_[last] = i;    // the moved value lives at i now
        a_.pop_back();
        at_.erase(x);
        return true;
    }

    int get_random() {
        std::uniform_int_distribution<int> dist(0, (int)a_.size() - 1);
        return a_[dist(rng_)];
    }

    int size() const { return (int)a_.size(); }
};

int main() {
    RandomizedSet s;
    std::cout << std::boolalpha;
    std::cout << "insert 1 " << s.insert(1) << "\n";
    std::cout << "insert 2 " << s.insert(2) << "\n";
    std::cout << "insert 1 again " << s.insert(1) << "\n";
    std::cout << "erase 3 (missing) " << s.erase(3) << "\n";
    std::cout << "erase 1 " << s.erase(1) << " size=" << s.size() << "\n";
    std::cout << "erase last remaining 2 " << s.erase(2) << " size=" << s.size() << "\n";
    s.insert(10);
    s.insert(20);
    s.insert(30);
    std::cout << "random sample:";
    for (int i = 0; i < 8; ++i) std::cout << " " << s.get_random();
    std::cout << "\n";
}
