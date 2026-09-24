// =============================================================================
// WHAT  : Limit order book: map<price, list of {id,qty}> × 2 (bids/asks) + id locator.
// WHY   : Best bid = rbegin of bids map. FIFO at a price = list. Cancel-by-id
//         in O(1) needs the list iterator stored in loc_. Heap of prices cannot
//         cancel. vector of orders would scan for cancel.
// THREAD: add/cancel/match all write. best_bid is const. Lock the book.
// ERASE : cancel: list.erase(lit) — only that iterator dies; loc_ still has
//         others. Empty price level: map.erase. match fills the head then
//         pop_front if qty hits 0. Never range-for a level and erase inside.
// REMEMBER: map[] on cancel would *create* an empty level — we find first via
//           loc_, then erase the node, then drop_level if empty.
//           Cross when best_bid >= best_ask.
// Guide : recipes/order_book.md, recipes/three_plus.md
// =============================================================================
#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <unordered_map>
#include <utility>

class OrderBook {
    using Book = std::map<int, std::list<std::pair<int, int>>>;  // price -> {id, qty}
    Book bids_;  // low..high; rbegin is best bid
    Book asks_;
    struct Loc {
        bool is_bid;
        int price;
        std::list<std::pair<int, int>>::iterator it;
    };
    std::unordered_map<int, Loc> loc_;

    static void drop_level(Book& b, int price) {
        if (auto it = b.find(price); it != b.end() && it->second.empty()) b.erase(it);
    }

    void fill_head(Book& b, bool is_bid, int take) {
        auto mit = is_bid ? std::prev(b.end()) : b.begin();
        int price = mit->first;
        auto& lst = mit->second;
        lst.front().second -= take;
        if (lst.front().second == 0) {
            loc_.erase(lst.front().first);
            lst.pop_front();
            drop_level(b, price);
        }
    }

    void match() {
        while (!bids_.empty() && !asks_.empty() &&
               std::prev(bids_.end())->first >= asks_.begin()->first) {
            auto& bl = std::prev(bids_.end())->second;
            auto& al = asks_.begin()->second;
            int take = std::min(bl.front().second, al.front().second);
            fill_head(bids_, true, take);
            fill_head(asks_, false, take);
        }
    }

public:
    void add_bid(int id, int price, int qty) {
        auto& lst = bids_[price];
        lst.push_back({id, qty});
        loc_[id] = Loc{true, price, std::prev(lst.end())};
        match();
    }

    void add_ask(int id, int price, int qty) {
        auto& lst = asks_[price];
        lst.push_back({id, qty});
        loc_[id] = Loc{false, price, std::prev(lst.end())};
        match();
    }

    bool cancel(int id) {
        auto it = loc_.find(id);
        if (it == loc_.end()) return false;
        auto [is_bid, price, lit] = it->second;
        Book& b = is_bid ? bids_ : asks_;
        b[price].erase(lit);  // O(1); other orders at this price stay
        drop_level(b, price);
        loc_.erase(it);
        return true;
    }

    int best_bid() const { return bids_.empty() ? -1 : std::prev(bids_.end())->first; }
    int best_ask() const { return asks_.empty() ? -1 : asks_.begin()->first; }
    bool live(int id) const { return loc_.count(id); }
};

int main() {
    OrderBook ob;
    ob.add_bid(1, 100, 5);
    ob.add_bid(2, 101, 3);
    std::cout << "best bid=" << ob.best_bid() << " (expect 101)\n";
    ob.add_ask(3, 101, 4);
    std::cout << "after ask 101x4: best bid=" << ob.best_bid()
              << " best ask=" << ob.best_ask() << " live(2)=" << ob.live(2)
              << " live(3)=" << ob.live(3) << "\n";
    ob.cancel(3);
    std::cout << "cancel leftover ask: best ask=" << ob.best_ask()
              << " best bid=" << ob.best_bid() << "\n";
    ob.add_ask(4, 99, 10);
    std::cout << "cross 99x10: best bid=" << ob.best_bid() << " best ask=" << ob.best_ask()
              << " live(1)=" << ob.live(1) << " live(4)=" << ob.live(4) << "\n";
}
