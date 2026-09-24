// =============================================================================
// WHAT  : Movie rental: price hash + per-movie available set + global rented set.
// WHY   : Three promises: (shop,movie)→price O(1); cheapest shops for a movie;
//         cheapest rented copies overall. One container per promise. Heap cannot
//         un-rent (no erase-by-key). search walks avail_[movie] from begin().
// THREAD: rent/drop write two indexes. search/report are const walks. Lock all
//         three as one unit.
// ERASE : rent: avail.erase({p,shop}) then rented.insert. drop: reverse.
//         Erase by key, not while range-for. report copies the first k of
//         rented_ — read-only. Destroy a copy: unhook price_, avail_, rented_.
// REMEMBER: pack(shop,movie) because unordered_map wants one key. set orders
//           {price,shop} so begin() is cheapest then smallest shop id.
// Guide : recipes/three_plus.md, recipes/movie_rental.md
// =============================================================================
#include <iostream>
#include <set>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

class MovieRenting {
    static long long pack(int shop, int movie) {
        return (static_cast<long long>(shop) << 32) | static_cast<unsigned>(movie);
    }

    std::unordered_map<long long, int> price_;
    std::unordered_map<int, std::set<std::pair<int, int>>> avail_;  // movie -> {price, shop}
    std::set<std::tuple<int, int, int>> rented_;                    // price, shop, movie

public:
    void add_copy(int shop, int movie, int p) {
        price_[pack(shop, movie)] = p;
        avail_[movie].insert({p, shop});
    }

    std::vector<int> search(int movie, int k = 5) const {
        std::vector<int> shops;
        auto it = avail_.find(movie);
        if (it == avail_.end()) return shops;
        for (auto [p, shop] : it->second) {  // cheapest first; read-only
            shops.push_back(shop);
            if ((int)shops.size() == k) break;
            (void)p;
        }
        return shops;
    }

    void rent(int shop, int movie) {
        int p = price_[pack(shop, movie)];
        avail_[movie].erase({p, shop});
        rented_.insert({p, shop, movie});
    }

    void drop(int shop, int movie) {
        int p = price_[pack(shop, movie)];
        rented_.erase({p, shop, movie});
        avail_[movie].insert({p, shop});
    }

    std::vector<std::tuple<int, int, int>> report(int k = 5) const {
        std::vector<std::tuple<int, int, int>> out;
        for (auto& t : rented_) {
            out.push_back(t);
            if ((int)out.size() == k) break;
        }
        return out;
    }
};

int main() {
    MovieRenting sys;
    sys.add_copy(1, 1, 3);
    sys.add_copy(1, 2, 3);
    sys.add_copy(2, 1, 4);
    sys.add_copy(2, 2, 5);
    sys.add_copy(3, 1, 2);

    std::cout << "search movie 1 shops:";
    for (int s : sys.search(1)) std::cout << " " << s;
    std::cout << " (expect 3 1 2)\n";

    sys.rent(3, 1);
    std::cout << "after rent 3/1, search 1:";
    for (int s : sys.search(1)) std::cout << " " << s;
    std::cout << " (expect 1 2)\n";

    sys.rent(2, 2);
    std::cout << "report (price shop movie):";
    for (auto [p, s, m] : sys.report()) std::cout << " (" << p << "," << s << "," << m << ")";
    std::cout << "\n";

    sys.drop(3, 1);
    std::cout << "after drop 3/1, search 1:";
    for (int s : sys.search(1)) std::cout << " " << s;
    std::cout << " (expect 3 1 2)\n";
}
