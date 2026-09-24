// =============================================================================
// WHAT  : Sliding-window maximum via a deque of indices (values decreasing).
// WHY   : Need pop_front (index slid out of window) AND pop_back (dominated
//         older values). vector pop_front is O(n). queue cannot touch the back.
// THREAD: dq is local. A producer filling `a` on another thread without a
//         happens-before is a race. Do not share a deque across writers.
// ERASE : We only pop_front / pop_back — O(1), no iterator held across them.
//         NEVER hold deque::iterator across push_front/push_back (iterators die;
//         *references* to elements stay valid on end insert).
// REMEMBER: Each index is pushed once and popped once → amortized O(1) per i.
//           deque is random-access but NOT contiguous: no data().
// Guide : containers/deque.md, recipes/sliding_window.md, 02_decision_guide.md
// =============================================================================
#include <deque>
#include <iostream>
#include <vector>

std::vector<int> window_max(const std::vector<int>& a, int k) {
    std::deque<int> dq;  // indices; a[dq.front()] is the max of the window
    std::vector<int> out;
    for (int i = 0; i < (int)a.size(); ++i) {
        // Drop indices whose values can never be the max once a[i] is in.
        while (!dq.empty() && a[dq.back()] <= a[i]) dq.pop_back();
        dq.push_back(i);
        if (dq.front() <= i - k) dq.pop_front();  // slid out of [i-k+1, i]
        if (i >= k - 1) out.push_back(a[dq.front()]);
    }
    return out;
}

int main() {
    std::vector<int> a{1, 3, -1, -3, 5, 3, 6, 7};
    int k = 3;
    auto mx = window_max(a, k);
    std::cout << "window max k=" << k << ":";
    for (int x : mx) std::cout << " " << x;
    std::cout << "\n";  // 3 3 5 5 6 7

    std::deque<char> work{'b', 'c'};
    work.push_front('a');  // this is O(1); vector insert(begin) would be O(n)
    work.push_back('d');
    std::cout << "deque as both-ends list: ";
    for (char c : work) std::cout << c;  // read-only walk: OK
    std::cout << "  [0]=" << work[0] << " [3]=" << work[3] << "\n";
}
