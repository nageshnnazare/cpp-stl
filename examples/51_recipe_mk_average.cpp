// =============================================================================
// WHAT  : MK average: queue of last m values + three multisets (lo / mid / hi).
// WHY   : Drop the k smallest and k largest, average the middle. A heap cannot
//         erase the expired window value. Three ordered bags + a running mid
//         sum: add/remove O(log m), average O(1). Queue remembers FIFO order.
// THREAD: add_element writes queue + bags. average is const. Lock the object.
// ERASE : remove(x): find in lo, else mid, else hi — erase THAT iterator
//         (one occurrence). rebalance moves extrema with erase(it) + insert.
//         Never range-for a bag and erase inside. Queue: pop the expired front.
// REMEMBER: mid_sum_ only tracks mid_. find(x) not erase(x) — duplicates exist.
//           After every add/remove, rebalance sizes then swap inversions.
// Guide : recipes/mk_average.md, 02_decision_guide.md (multiset vs heap)
// =============================================================================
#include <algorithm>
#include <iostream>
#include <iterator>
#include <queue>
#include <set>

class MKAverage {
    int m_, k_;
    std::queue<int> q_;
    std::multiset<int> lo_, mid_, hi_;
    long long mid_sum_ = 0;

    void rebalance() {
        int n = (int)(lo_.size() + mid_.size() + hi_.size());
        int want_lo = std::min(k_, n);
        int want_hi = std::min(k_, std::max(0, n - want_lo));

        while ((int)lo_.size() > want_lo) {
            auto it = std::prev(lo_.end());
            mid_sum_ += *it;
            mid_.insert(*it);
            lo_.erase(it);  // iterator erase — it is not used after
        }
        while ((int)hi_.size() > want_hi) {
            auto it = hi_.begin();
            mid_sum_ += *it;
            mid_.insert(*it);
            hi_.erase(it);
        }
        while ((int)lo_.size() < want_lo && !mid_.empty()) {
            auto it = mid_.begin();
            mid_sum_ -= *it;
            lo_.insert(*it);
            mid_.erase(it);
        }
        while ((int)hi_.size() < want_hi && !mid_.empty()) {
            auto it = std::prev(mid_.end());
            mid_sum_ -= *it;
            hi_.insert(*it);
            mid_.erase(it);
        }
        while (!lo_.empty() && !mid_.empty() && *std::prev(lo_.end()) > *mid_.begin()) {
            int a = *std::prev(lo_.end());
            int b = *mid_.begin();
            lo_.erase(std::prev(lo_.end()));
            mid_.erase(mid_.begin());
            lo_.insert(b);
            mid_.insert(a);
            mid_sum_ += a - b;
        }
        while (!mid_.empty() && !hi_.empty() && *std::prev(mid_.end()) > *hi_.begin()) {
            int a = *std::prev(mid_.end());
            int b = *hi_.begin();
            mid_.erase(std::prev(mid_.end()));
            hi_.erase(hi_.begin());
            mid_.insert(b);
            hi_.insert(a);
            mid_sum_ += b - a;
        }
        while (!lo_.empty() && !hi_.empty() && mid_.empty() && *std::prev(lo_.end()) > *hi_.begin()) {
            int a = *std::prev(lo_.end());
            int b = *hi_.begin();
            lo_.erase(std::prev(lo_.end()));
            hi_.erase(hi_.begin());
            lo_.insert(b);
            hi_.insert(a);
        }
    }

    void add(int x) {
        mid_.insert(x);
        mid_sum_ += x;
        rebalance();
    }

    void remove(int x) {
        if (auto it = lo_.find(x); it != lo_.end()) {
            lo_.erase(it);
        } else if (auto it = mid_.find(x); it != mid_.end()) {
            mid_sum_ -= x;
            mid_.erase(it);
        } else {
            hi_.erase(hi_.find(x));  // one occurrence
        }
        rebalance();
    }

public:
    MKAverage(int m, int k) : m_(m), k_(k) {}

    void add_element(int num) {
        q_.push(num);
        add(num);
        if ((int)q_.size() > m_) {
            int old = q_.front();
            q_.pop();
            remove(old);
        }
    }

    int average() const {
        if ((int)q_.size() < m_) return -1;
        return static_cast<int>(mid_sum_ / (m_ - 2 * k_));
    }
};

int main() {
    MKAverage mk(3, 1);
    mk.add_element(3);
    mk.add_element(1);
    std::cout << "after 3,1: " << mk.average() << " (expect -1)\n";
    mk.add_element(10);
    std::cout << "after 3,1,10: " << mk.average() << " (expect 3)\n";
    mk.add_element(5);
    std::cout << "after 1,10,5: " << mk.average() << " (expect 5)\n";
    mk.add_element(5);
    std::cout << "after 10,5,5: " << mk.average() << " (expect 5)\n";
}
