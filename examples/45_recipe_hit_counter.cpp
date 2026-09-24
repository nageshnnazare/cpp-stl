// =============================================================================
// WHAT  : Hit counter: queue of timestamps, or 300 circular buckets (one per second).
// WHY   : Queue: expire the left as time advances (sliding window). O(hits).
//         Buckets: O(1) hit, O(window) get — the production shape when the
//         window is a known 300s. deque of hits would work; vector would
//         O(n) pop_front.
// THREAD: hit and get_hits both mutate the queue (get pops expired). Two
//         threads = race. Lock, or atomics on the bucket arrays.
// ERASE : Queue: pop_front while front is too old — that IS delete-while-
//         "iterating", but adapters have no iterators; pop is the API.
//         Never range-for the queue. Buckets: overwrite a stale slot, no erase.
// REMEMBER: get_hits mutates (lazy expire). Window is (t-window, t].
//           Bucket i is valid only if stamp[i] is within the last 300s.
// Guide : recipes/hit_counter.md, containers/queue.md
// =============================================================================
#include <array>
#include <iostream>
#include <queue>

class HitCounter {
    std::queue<int> q_;

public:
    void hit(int t) { q_.push(t); }

    int get_hits(int t, int window = 300) {
        while (!q_.empty() && q_.front() <= t - window) q_.pop();  // expire left
        return (int)q_.size();
    }
};

class HitCounterBuckets {
    std::array<int, 300> hits_{};
    std::array<int, 300> stamp_{};

public:
    void hit(int t) {
        int i = t % 300;
        if (stamp_[i] != t) {  // this slot is a different second — reset
            stamp_[i] = t;
            hits_[i] = 0;
        }
        ++hits_[i];
    }

    int get_hits(int t) {
        int s = 0;
        for (int i = 0; i < 300; ++i)
            if (t - stamp_[i] < 300) s += hits_[i];  // index loop; array cannot erase
        return s;
    }
};

int main() {
    HitCounter c;
    c.hit(1);
    c.hit(2);
    c.hit(3);
    std::cout << "at 4: " << c.get_hits(4) << " (expect 3)\n";
    std::cout << "at 301: " << c.get_hits(301) << " (expect 2, time 1 expired)\n";

    HitCounterBuckets b;
    b.hit(1);
    b.hit(1);
    b.hit(2);
    std::cout << "buckets at 2: " << b.get_hits(2) << " (expect 3)\n";
    std::cout << "buckets at 301: " << b.get_hits(301) << " (expect 1)\n";
}
