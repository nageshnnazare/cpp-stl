// =============================================================================
// WHAT  : Time-based KV: hash key → vector of {timestamp, value}, binary search.
// WHY   : Sets are append-only in time. A map<int,string> per key also works
//         (upper_bound) but the vector is contiguous and we never delete middles.
//         unordered_map of timestamps would not binary-search.
// THREAD: set is a write (push_back). get is const if no concurrent set on
//         that key. Fine-grain: mutex per key, or freeze then read.
// ERASE : No erase. History only grows. To drop a key: db_.erase(k) — do not
//         walk-and-erase the vector of stamps; just destroy the whole vector.
// REMEMBER: upper_bound for first stamp > t, then prev is last stamp <= t.
//           Comparator takes (int, pair) so we search by time only.
//           Assumes set() timestamps are non-decreasing per key.
// Guide : recipes/time_based_kv.md, algorithms/binary_search.md
// =============================================================================
#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class TimeMap {
    std::unordered_map<std::string, std::vector<std::pair<int, std::string>>> db_;

public:
    void set(const std::string& k, const std::string& v, int t) {
        db_[k].push_back({t, v});  // append-only; keeps the vector sorted by t
    }

    std::string get(const std::string& k, int t) const {
        auto it = db_.find(k);  // find, not [] — we are const
        if (it == db_.end()) return "";
        const auto& a = it->second;
        auto ub = std::upper_bound(
            a.begin(), a.end(), t,
            [](int time, const std::pair<int, std::string>& p) { return time < p.first; });
        if (ub == a.begin()) return "";
        return std::prev(ub)->second;
    }
};

int main() {
    TimeMap m;
    m.set("price", "5", 10);
    m.set("price", "8", 20);
    m.set("price", "3", 40);
    std::cout << "t=25 -> " << m.get("price", 25) << " (expect 8)\n";
    std::cout << "t=5  -> [" << m.get("price", 5) << "] (expect empty)\n";
    std::cout << "t=40 -> " << m.get("price", 40) << " (expect 3)\n";
    std::cout << "t=20 -> " << m.get("price", 20) << " (expect 8)\n";
    std::cout << "missing key -> [" << m.get("fx", 100) << "]\n";
}
