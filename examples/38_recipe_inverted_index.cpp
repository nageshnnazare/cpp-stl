// =============================================================================
// WHAT  : Inverted index: term → sorted doc-id vector; AND = set_intersection.
// WHY   : Hash for the term (no order of words). Vector of ids stays sorted
//         because we scan docs in id order — then intersection is linear.
//         set<int> per term would also work, slower and more allocations.
// THREAD: Building inv is a write. Queries can be concurrent const *after*
//         the index is frozen. Do not insert a new term while intersecting.
// ERASE : Query does not erase. To drop a document: collect its terms, then
//         erase id from each posting list (it = erase(it) or erase-remove).
//         Do not insert into inv while walking inv (rehash).
// REMEMBER: set_intersection needs both ranges sorted. inv[a] via [] would
//           insert an empty posting if the term is missing — find if that matters.
// Guide : recipes/inverted_index.md, algorithms/set_operations.md
// =============================================================================
#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <unordered_map>
#include <vector>

int main() {
    std::vector<std::vector<std::string>> docs{
        {"cat", "dog"},
        {"cat", "bird"},
        {"dog"},
        {"cat", "dog", "bird"},
    };

    std::unordered_map<std::string, std::vector<int>> inv;
    for (int id = 0; id < (int)docs.size(); ++id)
        for (auto& w : docs[id]) inv[w].push_back(id);
    // ids already increasing per term because we scan docs in order

    auto and_query = [&](const std::string& a, const std::string& b) {
        std::vector<int> out;
        auto& A = inv[a];
        auto& B = inv[b];
        std::set_intersection(A.begin(), A.end(), B.begin(), B.end(),
                              std::back_inserter(out));
        return out;
    };

    auto hits = and_query("cat", "dog");
    std::cout << "cat AND dog docs:";
    for (int id : hits) std::cout << " " << id;
    std::cout << "\n";

    std::cout << "bird postings:";
    for (int id : inv["bird"]) std::cout << " " << id;
    std::cout << "\n";
}
