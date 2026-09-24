// =============================================================================
// WHAT  : Freq stack: hash value→freq + vector of stacks, one stack per frequency.
// WHY   : pop the most frequent, ties = most recently pushed of that freq.
//         A single stack loses frequency. A heap of {freq, time, val} works
//         but is heavier. bucket_[f] is a stack so the top is the newest at
//         that frequency. Dense freq 1..n → vector of stacks, not a map.
// THREAD: push/pop write both indexes. Not thread-safe.
// ERASE : pop only from bucket_[maxf_]. --freq_[x] is a value update, not
//         map erase (the key stays until freq hits 0 — we leave a 0, harmless).
//         If the max-freq stack empties, --maxf_. No walk-and-erase.
// REMEMBER: push x at its NEW frequency (it also remains in lower buckets —
//           those older copies are the previous "most recent at freq f-1").
// Guide : recipes/freq_stack.md
// =============================================================================
#include <algorithm>
#include <iostream>
#include <stack>
#include <unordered_map>
#include <vector>

class FreqStack {
    std::unordered_map<int, int> freq_;
    std::vector<std::stack<int>> bucket_;
    int maxf_ = 0;

public:
    void push(int x) {
        int f = ++freq_[x];
        maxf_ = std::max(maxf_, f);
        if ((int)bucket_.size() <= f) bucket_.resize(f + 1);
        bucket_[f].push(x);
    }

    int pop() {
        int x = bucket_[maxf_].top();
        bucket_[maxf_].pop();
        --freq_[x];
        if (bucket_[maxf_].empty()) --maxf_;
        return x;
    }
};

int main() {
    FreqStack s;
    for (int x : {5, 7, 5, 7, 4, 5}) s.push(x);
    std::cout << "pops:";
    for (int i = 0; i < 4; ++i) std::cout << " " << s.pop();
    std::cout << " (expect 5 7 5 4)\n";
}
