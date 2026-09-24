// =============================================================================
// WHAT  : Stock tracker: map<time,price> (latest time = current) + multiset of prices.
// WHY   : Need current (max key), min, max, and *correct* a past timestamp.
//         Heap cannot erase the old price on a correction. map alone would
//         scan all values for min/max. Two indexes, both updated on write.
// THREAD: update writes both. current/min/max are const. Lock the pair.
// ERASE : Correction: prices_.erase(prices_.find(old)) — find, not erase(old),
//         because erase(old) on a multiset drops ALL copies of that price.
//         We never range-for + erase. at_ update is it->second = (value, not key).
// REMEMBER: rbegin() of map is latest time. rbegin() of multiset is max price.
// Guide : recipes/stock_tracker.md, 02_decision_guide.md (multiset vs heap)
// =============================================================================
#include <iostream>
#include <map>
#include <set>

class StockTracker {
    std::map<int, int> at_;
    std::multiset<int> prices_;

public:
    void update(int t, int p) {
        if (auto it = at_.find(t); it != at_.end()) {
            prices_.erase(prices_.find(it->second));  // one occurrence, not equal_range
            it->second = p;
        } else {
            at_[t] = p;
        }
        prices_.insert(p);
    }

    int current() const { return at_.rbegin()->second; }
    int maximum() const { return *prices_.rbegin(); }
    int minimum() const { return *prices_.begin(); }
};

int main() {
    StockTracker s;
    s.update(1, 10);
    s.update(2, 5);
    std::cout << "current=" << s.current() << " min=" << s.minimum()
              << " max=" << s.maximum() << "\n";
    s.update(1, 3);  // correct time 1
    std::cout << "after correct t=1 to 3: current=" << s.current()
              << " min=" << s.minimum() << " max=" << s.maximum() << "\n";
    s.update(4, 2);
    std::cout << "after t=4 p=2: current=" << s.current() << " min=" << s.minimum()
              << " max=" << s.maximum() << "\n";
}
