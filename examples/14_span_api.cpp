// =============================================================================
// WHAT  : span as a function parameter — one sum() for vector, array, C array.
// WHY   : Non-owning (ptr, len). const vector<int>& would force array callers
//         to copy into a vector. T* + n forgets the length at the first call.
// THREAD: The view is a pointer. Concurrent writes through two spans of the
//         SAME buffer race. The span object itself is just two registers.
// ERASE : Span cannot erase. Delete on the owner. subspan does not copy.
//         After v.push_back that reallocates, every span into v is dangling.
// REMEMBER: Store owners, pass views. span<int> can write; span<const int> cannot.
// Guide : containers/span.md, 07_pitfalls.md
// =============================================================================
#include <array>
#include <iostream>
#include <numeric>
#include <span>
#include <vector>

int sum(std::span<const int> xs) {
    return std::accumulate(xs.begin(), xs.end(), 0);  // init 0 is int — OK for tiny arrays
}

void scale(std::span<int> xs, int k) {
    for (int& x : xs) x *= k;  // writes through to the owner
}

int main() {
    int raw[] = {1, 2, 3};
    std::array<int, 2> a{4, 5};
    std::vector<int> v{6, 7, 8};
    std::cout << "sum raw=" << sum(raw) << " array=" << sum(a) << " vector=" << sum(v) << "\n";

    scale(v, 10);
    std::cout << "scaled vector:";
    for (int x : v) std::cout << " " << x;
    std::cout << "\n";

    auto mid = std::span<int>(v).subspan(1, 2);  // still aliases v — no copy
    std::cout << "subspan:";
    for (int x : mid) std::cout << " " << x;
    std::cout << "\n";
}
