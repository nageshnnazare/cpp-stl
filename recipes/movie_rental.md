# Movie rental — three indexes on the same copy

**Need:** shops stock movies at a price. `search` cheapest
shops that still have a movie; `rent` / `drop` move a copy
between "on shelf" and "out"; `report` the cheapest rented
copies. All without scanning the inventory.

One copy of `(shop, movie, price)` must be findable by:

```
  1. (shop, movie)     -> price, and "is it rented?"
  2. movie             -> cheapest available shops
  3. global rented     -> cheapest outstanding rentals
```

Three questions ⇒ three containers. Rent is not an update of
price; it is **destroy in index 2 + create in index 3**.

```
  price[(shop,movie)] = 3     (never moves)

  available[movie=1]:     { (2, shopA), (3, shopB) }     set of {price, shop}
  rented:                 { (2, shopC, movie=2), ... }   set of {price, shop, movie}

  rent(shopA, 1):
      available[1].erase({2, shopA})
      rented.insert({2, shopA, 1})
```

## Why three, not "a vector of rows"

| Design | search(movie) | rent / drop | report | Verdict |
| --- | --- | --- | --- | --- |
| `vector` of structs | O(n) | O(n) | O(n log n) sort | toy |
| one `map<(shop,movie), …>` | O(n) movies | O(log n) | O(n) | missing movie index |
| **price hash + avail set + rented set** | O(log n + 5) | O(log n) | O(5) | **the one** (LeetCode 1912) |

Price is immutable here. If prices could change, you would
erase+insert in **both** ordered sets (same pattern as
[`leaderboard.md`](leaderboard.md)).

## Sketch

```cpp
std::unordered_map<long long, int> price;                 // (shop,movie) -> p
std::unordered_map<int, std::set<std::pair<int,int>>> av; // movie -> {p,shop}
std::set<std::tuple<int,int,int>> rented;                 // {p,shop,movie}

void rent(int shop, int movie) {
    int p = price[pack(shop, movie)];
    av[movie].erase({p, shop});
    rented.insert({p, shop, movie});
}

void drop(int shop, int movie) {
    int p = price[pack(shop, movie)];
    rented.erase({p, shop, movie});
    av[movie].insert({p, shop});
}
```

`search`: walk `av[movie]` until 5 shops. `report`: walk
`rented` until 5 tuples. **Destroy** of a copy (drop) must
touch both ordered indexes; forgetting `rented.erase` makes
report lie.

Full program: [`examples/50_recipe_movie_rental.cpp`](../examples/50_recipe_movie_rental.cpp).

## Related problems

- Design Movie Rental System (1912)
- Design a Food Rating System (2353) — two indexes (name →
  cuisine/rating, cuisine → ordered names)
- [`leaderboard.md`](leaderboard.md) — id + score order; no
  third "rented" axis
- Inventory / library checkout: same three questions
  (identity, available-by-isbn, outstanding-loans)
