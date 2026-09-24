// =============================================================================
// WHAT  : Time 1e6 push_back with vs without reserve. Compare the ratio.
// WHY   : Unreserved growth copies the whole buffer on each realloc. reserve
//         is one allocation. The absolute microseconds vary by machine; the
//         ratio is the lesson. This is also why iterators die without reserve.
// THREAD: Each run owns its vector. Timing is not a concurrency demo.
// ERASE : Not involved. The growth path is insert-at-end.
// REMEMBER: volatile sink so the compiler cannot delete the loop. warmup
//           first so the with/without pair is fairer. Do not treat one sample
//           as a benchmark lab — look at the ratio.
// Guide : 09_memory_reserve_pmr.md, 12_measure.md
// =============================================================================
#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {
long long push_n(int n, bool reserve) {
    std::vector<int> v;
    if (reserve) v.reserve(static_cast<std::size_t>(n));
    auto t0 = std::chrono::steady_clock::now();
    for (int i = 0; i < n; ++i) v.push_back(i);
    auto t1 = std::chrono::steady_clock::now();
    volatile std::size_t sink = v.size();
    (void)sink;
    return std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
}
}  // namespace

int main() {
    constexpr int n = 1'000'000;
    push_n(n, true);  // warmup
    auto with = push_n(n, true);
    auto without = push_n(n, false);
    std::cout << "n=" << n << "  with reserve=" << with
              << " us  without=" << without << " us";
    if (with > 0)
        std::cout << "  ratio without/with=" << (double)without / (double)with;
    std::cout << "\n";
}
