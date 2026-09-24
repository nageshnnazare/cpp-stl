// =============================================================================
// WHAT  : unordered_map frequency (Create-or-Update) and Two Sum (complement).
// WHY   : No ordered walk. Avg O(1) find. map would be O(log n) for no benefit.
//         ++m[k] is the rare case where [] inserting 0 is exactly what you want.
// THREAD: ++freq[w] is a write. Two threads counting the same map = data race.
//         Shard by thread-local maps, then merge, or lock the map.
// ERASE : Not shown. Walking freq and inserting new keys may rehash — ALL
//         iterators die. Collect new keys, insert after the walk.
//         it = m.erase(it) is the legal in-loop delete.
// REMEMBER: find, not [] , for "is this complement present?" [] would insert 0.
//           reserve before a known number of keys.
// Guide : containers/unordered_map.md, recipes/two_sum_and_anagrams.md
// =============================================================================
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

std::pair<int, int> two_sum(const std::vector<int>& a, int target) {
    std::unordered_map<int, int> last;  // value -> index
    for (int i = 0; i < (int)a.size(); ++i) {
        if (auto it = last.find(target - a[i]); it != last.end())
            return {it->second, i};
        last[a[i]] = i;  // insert or overwrite last index of this value
    }
    return {-1, -1};
}

int main() {
    std::vector<std::string> words{"apple", "pear", "apple", "apple", "pear"};
    std::unordered_map<std::string, int> freq;
    freq.reserve(words.size());
    for (auto& w : words) ++freq[w];  // [] default-constructs 0, then ++
    std::cout << "apple=" << freq["apple"] << " pear=" << freq["pear"] << "\n";

    auto [i, j] = two_sum({2, 7, 11, 15}, 9);
    std::cout << "two_sum indices: " << i << " " << j << "\n";
}
