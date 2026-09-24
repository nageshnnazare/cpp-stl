// =============================================================================
// WHAT  : Autocomplete two ways: map.lower_bound walk of a prefix, or a trie.
// WHY   : map: keys are ordered, so the prefix range is [lower_bound(p), first
//         that does not start with p). Trie: O(|prefix|) to the node, then DFS.
//         unordered_map of words cannot walk a prefix. Heap of all words is overkill.
// THREAD: insert writes. complete is a const walk if the dict is frozen.
// ERASE : complete does not erase. To drop a word: dict.erase(w), or trie
//         mark freq=0 (lazy). Do not erase from map while walking the prefix
//         range — collect keys, then erase (pattern 3).
// REMEMBER: compare(0, n, p)==0 is "starts with p". partial_sort for top-k
//           by {-freq, word}. Trie children are array<26> because alphabet is dense.
// Guide : recipes/autocomplete.md, containers/map.md
// =============================================================================
#include <algorithm>
#include <array>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

std::vector<std::string> complete_map(const std::map<std::string, int>& dict,
                                      const std::string& p, int k) {
    std::vector<std::pair<int, std::string>> cand;
    for (auto it = dict.lower_bound(p);
         it != dict.end() && it->first.compare(0, p.size(), p) == 0; ++it)
        cand.push_back({-it->second, it->first});  // read-only range walk
    int take = std::min(k, (int)cand.size());
    std::partial_sort(cand.begin(), cand.begin() + take, cand.end());
    std::vector<std::string> out;
    for (int i = 0; i < take; ++i) out.push_back(cand[i].second);
    return out;
}

struct Trie {
    struct Node {
        std::array<std::unique_ptr<Node>, 26> ch{};
        int freq = 0;
    };
    Node root;

    void insert(std::string_view w, int freq) {
        Node* n = &root;
        for (char c : w) {
            int i = c - 'a';
            if (!n->ch[i]) n->ch[i] = std::make_unique<Node>();
            n = n->ch[i].get();
        }
        n->freq = freq;
    }

    std::vector<std::string> complete(const std::string& p, int k) const {
        const Node* n = &root;
        for (char c : p) {
            int i = c - 'a';
            if (i < 0 || i >= 26 || !n->ch[i]) return {};
            n = n->ch[i].get();
        }
        std::vector<std::pair<int, std::string>> cand;
        std::string cur = p;
        dfs(n, cur, cand);
        int take = std::min(k, (int)cand.size());
        std::partial_sort(cand.begin(), cand.begin() + take, cand.end());
        std::vector<std::string> out;
        for (int i = 0; i < take; ++i) out.push_back(cand[i].second);
        return out;
    }

private:
    static void dfs(const Node* n, std::string& cur,
                    std::vector<std::pair<int, std::string>>& cand) {
        if (n->freq) cand.push_back({-n->freq, cur});
        for (int i = 0; i < 26; ++i) {
            if (!n->ch[i]) continue;
            cur.push_back(static_cast<char>('a' + i));
            dfs(n->ch[i].get(), cur, cand);
            cur.pop_back();
        }
    }
};

int main() {
    std::map<std::string, int> dict{{"car", 5}, {"card", 1}, {"cat", 3}, {"dog", 2}};
    std::cout << "map 'ca':";
    for (auto& w : complete_map(dict, "ca", 3)) std::cout << " " << w;
    std::cout << " (expect car cat card)\n";

    Trie t;
    t.insert("car", 5);
    t.insert("card", 1);
    t.insert("cat", 3);
    t.insert("dog", 2);
    std::cout << "trie 'ca':";
    for (auto& w : t.complete("ca", 3)) std::cout << " " << w;
    std::cout << "\n";
}
