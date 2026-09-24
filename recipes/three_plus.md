# Three-plus indexes — CRUD with more than two containers

Two containers cover most interview designs (hash + order,
hash + list). The recipes below need **three or more** because
each CRUD verb looks the data up on a **different axis**:

```
  Create    need a free slot / a time bucket / a price level
  Read      need "best" / "first unique" / "top 5 by other key"
  Update    need the row by id without scanning the ordered view
  Destroy   need to unhook the same row from EVERY index
```

If destroy only updates one index, the others dangle. That is
the whole subject of this group.

| Recipe | Containers | C | R | U | D |
| --- | --- | --- | --- | --- | --- |
| [`movie_rental.md`](movie_rental.md) | price hash, available `set`, rented `set` | stock | search / report | rent (move) | drop |
| [`mk_average.md`](mk_average.md) | `queue` + 3 `multiset`s | append | average | rebalance | expire oldest |
| [`ttl_cache.md`](ttl_cache.md) | LRU `list` + pos hash + expiry `set` | put | get | refresh TTL | expire / LRU evict |
| [`order_book.md`](order_book.md) | bid map, ask map, id → list it | limit order | best bid/ask | partial fill | cancel |
| [`dinner_plates.md`](dinner_plates.md) | `vector<stack>` + `set` of holes | push | pop / popAt | — | pop empties a stack |
| [`first_unique.md`](first_unique.md) | `list` + pos hash + dead `set` | add unique | front | — | demote on 2nd copy |
| [`delayed_queue.md`](delayed_queue.md) | time `map` of lists + id hash | schedule | pop due | — | cancel |
| [`max_stack.md`](max_stack.md) | `list` + `map` of iterators | push | top / peekMax | — | pop / popMax |
| [`many_to_many.md`](many_to_many.md) | two hashes of `set`s | tag | list either side | retag | erase item or tag |
| [`file_system.md`](file_system.md) | nested `map` trie | mkdir / write | ls / read | append | rm |

Rule of thumb: **one container per lookup you promised in O(log n)
or better.** A second ordered view that is not updated on
delete is not a design, it is a bug.
