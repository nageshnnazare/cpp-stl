// =============================================================================
// WHAT  : next_permutation, minmax_element, clamp.
// WHY   : next_permutation walks all orderings in lex order — n! of them.
//         minmax_element is one pass, two results. clamp is just min(max(x,lo),hi).
// THREAD: next_permutation writes v in place. minmax_element is a const scan
//         (concurrent const OK). clamp is a pure value — no container.
// ERASE : next_permutation rearranges, does not erase. To "delete" a perm,
//         you skip it in the do-while, you do not erase from v.
// REMEMBER: start from sorted order or you miss earlier perms.
//           next_permutation returns false and resets to sorted when done.
// Guide : algorithms/permutations.md, algorithms/minmax.md
// =============================================================================
#include <algorithm>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> v{1, 2, 3};
    std::cout << "permutations of 1,2,3:\n";
    do {
        std::cout << " ";
        for (int x : v) std::cout << " " << x;
        std::cout << "\n";
    } while (std::next_permutation(v.begin(), v.end()));

    std::vector<int> a{7, 1, 5, 9, 2};
    auto [mn, mx] = std::minmax_element(a.begin(), a.end());
    std::cout << "min=" << *mn << " max=" << *mx << "\n";
    std::cout << "clamp 15 into [0,10]: " << std::clamp(15, 0, 10) << "\n";
}
