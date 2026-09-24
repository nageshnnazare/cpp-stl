# Limit order book — two price ladders plus an id index

**Need:** `add` a limit bid or ask, **match** against the other
side, `cancel` by id, `best_bid` / `best_ask`. FIFO at a price
(time priority). Partial fills update quantity in place.

```
  bids (high → low)              asks (low → high)
  101:  [id2 qty3]               103:  [id7 qty1]
  100:  [id1 qty5] -> [id4 qty2] 104:  [id8 qty9]

  add ask id9 price=100 qty=4
      matches id2 (3) then id1 (1)     id1 left with 4
      best bid still 100
```

Cancel is the operation that forces the third container: you
are given **id**, not price. A `map<price, queue>` cannot find
id 4 in the middle of the 100-queue in O(1). So:

```
  bids:  map<price, list<{id,qty}>, greater<>>
  asks:  map<price, list<{id,qty}>>
  loc:   unordered_map<id, {side, price, list::iterator}>
```

`list` (not `queue`) because cancel **erases a middle node**
in O(1) given the iterator. Empty price levels are `map.erase`d
so `begin()` is always the true best.

## Why these three (really four types)

| Design | add/match | cancel | best | Verdict |
| --- | --- | --- | --- | --- |
| two heaps of orders | O(log n) | lazy / no | O(1) | cannot cancel honestly |
| two `map<price, qty>` | O(log P) | no ids | O(1) | loses FIFO and cancel |
| **two `map<price,list>` + id hash** | O(log P + fills) | O(1) + maybe log P | O(1) | **the one** |

CRUD mapping:

| Verb | What happens |
| --- | --- |
| C | new id sits at back of its price list; loc[id] stored |
| R | `bids.begin()` / `asks.begin()` |
| U | match decrements `qty` on the head (or on both heads) |
| D | cancel `list.erase(it)`; fill-to-zero is the same erase |

Forgetting `loc.erase(id)` on a full fill is a use-after-free
on a later cancel.

Full program: [`examples/53_recipe_order_book.cpp`](../examples/53_recipe_order_book.cpp).

## Related problems

- Exchange matching engine / "design a stock exchange"
- Dinner plates — another "stack at an index + find a hole"
  ([`dinner_plates.md`](dinner_plates.md))
- [`leaderboard.md`](leaderboard.md) — one ordered axis, not two
  sides that **eat each other**
- Movie rental — move between two sets; here quantity mutates
  in place
