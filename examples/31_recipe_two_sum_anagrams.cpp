// =============================================================================
// WHAT  : Two Sum, group anagrams (sorted-string key), subarray-sum-k (prefix freq).
// WHY   : All three are "have I seen this complement?" — unordered_map, not map.
//         Anagram key is the sorted letters. Prefix map: how many times pref-k
//         already occurred (so current window sums to k).
// THREAD: Local maps, single-threaded. Sharing last[] across threads: lock,
//         or per-thread maps + merge. [] is a write.
// ERASE : Walks are read/insert of *new* keys. subarray_sum only ++freq[pref]
//         — insert during iteration of freq would rehash; we do not iterate.
//         Group: after the fill, we move values out — do not erase while walking;
//         we consume kv.second.
// REMEMBER: find the complement, do not [] it (would insert 0). freq[0]=1 for
//           the empty prefix. sort the copy, not the original word, as the key.
// Guide : recipes/two_sum_and_anagrams.md, 10_unordered_map_freq.cpp
// =============================================================================
#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

std::vector<std::vector<std::string>> group_anagrams(std::vector<std::string> words) {
    std::unordered_map<std::string, std::vector<std::string>> g;
    for (auto& w : words) {
        auto key = w;
        std::sort(key.begin(), key.end());
        g[key].push_back(std::move(w));
    }
    std::vector<std::vector<std::string>> out;
    for (auto& kv : g) out.push_back(std::move(kv.second));  // read-only keys; steal values
    return out;
}

int subarray_sum_k(const std::vector<int>& a, int k) {
    std::unordered_map<long long, int> freq;
    freq[0] = 1;
    long long pref = 0;
    int ans = 0;
    for (int x : a) {
        pref += x;
        if (auto it = freq.find(pref - k); it != freq.end()) ans += it->second;
        ++freq[pref];
    }
    return ans;
}

int main() {
    std::unordered_map<int, int> last;
    std::vector<int> nums{2, 7, 11, 15};
    int target = 9;
    for (int i = 0; i < (int)nums.size(); ++i) {
        if (auto it = last.find(target - nums[i]); it != last.end()) {
            std::cout << "two_sum " << it->second << " " << i << "\n";
            break;
        }
        last[nums[i]] = i;
    }

    auto groups = group_anagrams({"eat", "tea", "tan", "ate", "nat", "bat"});
    std::cout << "anagram groups: " << groups.size() << "\n";
    for (auto& g : groups) {
        std::cout << " ";
        for (auto& w : g) std::cout << " " << w;
        std::cout << "\n";
    }

    std::cout << "subarrays sum to 2 in [1,1,1]: " << subarray_sum_k({1, 1, 1}, 2) << "\n";
}
