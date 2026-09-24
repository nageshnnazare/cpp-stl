// =============================================================================
// WHAT  : Delay queue: map<deadline, list<Job>> + hash id → list iterator.
// WHY   : Next due job = map.begin(). Cancel-by-id without scanning the time
//         map needs the stored iterator. priority_queue cannot cancel. One
//         list per timestamp keeps FIFO among equal deadlines.
// THREAD: schedule/cancel/pop_ready write. next_deadline is const. Lock.
// ERASE : cancel: lst.erase(lit) then drop empty time bucket. pop_ready:
//         walk the due list (read), unhook loc_, then erase the whole bucket
//         — we do not erase-from-list while walking; we destroy the list after.
// REMEMBER: Destroy must update loc_ AND by_time_. Empty map.erase(t) so
//           begin() is the next live deadline.
// Guide : recipes/delayed_queue.md, recipes/three_plus.md
// =============================================================================
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class DelayQueue {
public:
    struct Job {
        int id;
        std::string payload;
    };

private:
    std::map<long long, std::list<Job>> by_time_;
    struct Loc {
        long long t;
        std::list<Job>::iterator it;
    };
    std::unordered_map<int, Loc> loc_;

public:
    bool schedule(int id, long long when, std::string payload) {
        if (loc_.count(id)) return false;
        auto& lst = by_time_[when];
        lst.push_back(Job{id, std::move(payload)});
        loc_[id] = Loc{when, std::prev(lst.end())};
        return true;
    }

    bool cancel(int id) {
        auto it = loc_.find(id);
        if (it == loc_.end()) return false;
        auto [t, lit] = it->second;
        auto& lst = by_time_[t];
        lst.erase(lit);
        if (lst.empty()) by_time_.erase(t);
        loc_.erase(it);
        return true;
    }

    std::vector<Job> pop_ready(long long now) {
        std::vector<Job> out;
        while (!by_time_.empty() && by_time_.begin()->first <= now) {
            auto t = by_time_.begin()->first;
            auto& lst = by_time_.begin()->second;
            for (auto& j : lst) {  // steal jobs; list is destroyed after the walk
                loc_.erase(j.id);
                out.push_back(std::move(j));
            }
            (void)t;
            by_time_.erase(by_time_.begin());  // after the walk, not during
        }
        return out;
    }

    long long next_deadline() const {
        return by_time_.empty() ? -1 : by_time_.begin()->first;
    }
};

int main() {
    DelayQueue q;
    q.schedule(1, 10, "A");
    q.schedule(2, 25, "B");
    q.schedule(3, 10, "C");
    q.cancel(3);
    std::cout << "next=" << q.next_deadline() << "\n";
    auto a = q.pop_ready(10);
    std::cout << "at 10:";
    for (auto& j : a) std::cout << " " << j.payload;
    std::cout << " (expect A)\n";
    std::cout << "cancel 3 after fire=" << std::boolalpha << q.cancel(3)
              << " cancel 2=" << q.cancel(2) << "\n";
    std::cout << "next after cancel B=" << q.next_deadline() << " (expect -1)\n";
}
