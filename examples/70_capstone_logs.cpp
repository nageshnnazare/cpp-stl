// =============================================================================
// WHAT  : Capstone: word frequency (hash) + top-k (partial_sort) + sliding
//         window of tokens (deque + count map).
// WHY   : Three jobs, three shapes. Count does not need order → unordered_map.
//         Top-k of a tiny unique set → vector of pairs + partial_sort, not a
//         heap (n is tiny). Window of last W tokens: deque for ends, hash for
//         "is this token already in the window?".
// THREAD: All local. A production log pipeline would shard counts per thread
//         then merge — do not ++ the same unordered_map from two threads.
// ERASE : Frequency walk is read-only into ranked. Window: pop_front when
//         size > W, then --win_count. Do not range-for win_count and erase
//         zeros (harmless leftovers). Do not insert new keys while iterating
//         freq (we don't — we fill ranked after the count loop).
// REMEMBER: unique tokens can repeat; window counts, not a set. partial_sort
//           comparator: higher count first, then name. W=3 deque is the live
//           window at the end of the scan.
// Guide : 00_mental_model.md, 02_decision_guide.md, recipes/top_k_frequent.md
// =============================================================================
#include <algorithm>
#include <deque>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

int main() {
    const std::vector<std::pair<std::string, std::string>> log{
        {"ada", "vector"}, {"grace", "map"},   {"ada", "set"},
        {"al", "vector"},  {"ada", "vector"},  {"grace", "set"},
        {"al", "deque"},   {"ada", "map"},
    };

    std::unordered_map<std::string, int> freq;
    for (auto& [user, word] : log) {
        (void)user;
        ++freq[word];
    }

    std::vector<std::pair<int, std::string>> ranked;
    ranked.reserve(freq.size());
    for (auto& [w, c] : freq) ranked.push_back({c, w});  // read-only; no insert into freq
    int k = 3;
    int take = std::min(k, (int)ranked.size());
    std::partial_sort(ranked.begin(), ranked.begin() + take, ranked.end(),
                      [](auto& a, auto& b) {
                          if (a.first != b.first) return a.first > b.first;
                          return a.second < b.second;
                      });
    std::cout << "top " << take << " words:";
    for (int i = 0; i < take; ++i)
        std::cout << " " << ranked[i].second << "(" << ranked[i].first << ")";
    std::cout << "\n";

    const int W = 3;
    std::deque<std::string> window;
    std::unordered_map<std::string, int> win_count;
    int duplicate_events = 0;
    for (auto& [user, word] : log) {
        (void)user;
        if (win_count[word] > 0) ++duplicate_events;
        window.push_back(word);
        ++win_count[word];
        if ((int)window.size() > W) {
            --win_count[window.front()];
            window.pop_front();  // ends only — deque's cheap erase
        }
    }
    std::cout << "sliding W=" << W << " duplicate-word arrivals=" << duplicate_events
              << "\nwindow:";
    for (auto& w : window) std::cout << " " << w;
    std::cout << "\n";
}
