# Cheat sheet (one sitting, one page)

Print this. Decode with [`14_read_a_problem.md`](14_read_a_problem.md).
Laws: [`rules.md`](rules.md). Catalog: [`problem_catalog.md`](problem_catalog.md).

## ASK / ARRIVE / EXTRA (always in the sentence)

```
  ASK      what to return / support          (always there)
  ARRIVE   array | stream | mixed API        (always there)
  EXTRA    other verbs actually written      (often none)

  Missing delete / update / "by id" / order  →  do not pay for them
  ASK is cheap.  EXTRA is the only reason to add a second container.
```

## Picker

```
  index / append / undo one end ............... vector
  compile-time N .............................. array / bitset
  non-owning slice / text ..................... span / string_view
  both ends / sliding window / BFS queue ...... deque / queue
  seen? / freq / id→value, no order ........... unordered_set / map
  sorted keys / predecessor / sweep ........... set / map
  only the extreme, stream .................... priority_queue
  extreme AND erase 7 / both ends ordered ..... multiset
  O(1) cut of a node you hold ................. list + hash of iterators
  dense 0..n-1 ................................ vector, never map<int,V>
```

## Hot operation → type

| Hot op | Type |
| --- | --- |
| `v[i]`, `push_back` | `vector` |
| `push_front` and `push_back` | `deque` |
| `find(k)` average O(1) | `unordered_*` |
| `lower_bound` / in-order walk | `set`/`map` |
| `top` max, `pop` max | `priority_queue` |
| `begin` min, `erase(7)` | `set`/`multiset` |
| `splice` / LRU | `list` + hash |
| swap-pop + `getRandom` | `vector` + index hash |

## Complexity pocket

```
                 find        insert end     middle / by key     ordered walk
  vector         O(n)        A O(1)         O(n)                cache-hot
  deque          O(n)        A O(1) both    O(n)                OK
  list           O(n)        O(1) ends      O(1) if you sit     cold
  set/map        O(log n)    O(log n)       O(log n)            sorted
  unordered_*    A O(1)      A O(1)         A O(1)              random
  heap           n/a         O(log n)       cannot erase 7      cannot walk
```

## Erase

```
  NEVER  for (auto& x : c) c.erase(...)

  vector:   erase(remove_if(v.begin(), v.end(), pred), v.end());  // or erase_if
  nodes:    it = c.erase(it);  else ++it;
  hash+insert in same loop:  collect keys, then mutate
  order-doesn't-matter:      v[i] = move(v.back()); v.pop_back();
```

## Algorithms

```
  find / count / all_of          scan
  transform / copy_if            write a new range
  erase_if / remove+erase        drop
  sort / stable_sort             full order
  partial_sort                   k smallest, sorted
  nth_element                    k-th
  lower_bound  (MEMBER on set)   sorted find
  set_intersection / merge       two sorted ranges
  partition                      pred in front
  partial_sum                    prefix
```

## Map `[]` / heap / list

```
  m[k]           INSERTS 0.  Membership = find / contains
  heap           no erase(7), no iterators.  Need those → multiset
  list           only if you hold a handle.  Else vector
```

## Invalidation (short)

```
  vector realloc          ALL it / ref / ptr die
  vector erase            at and after die
  list/set/map erase      only that it
  unordered insert        MAY rehash → ALL it die
  unordered erase(it)     only it (returns next)
```

## Threads

```
  many const methods, one object     OK
  any non-const vs another thread    YOU lock.  map[] is non-const
  STL is not TBB
```

## T in a container

```
  vector grow     wants noexcept move
  set             wants strict <   (not <=)
  unordered       wants hash + ==  (equal ⇒ same hash)
```

## If the problem says…

| Says | Open |
| --- | --- |
| Two Sum / anagrams / sum k | recipe two_sum |
| LRU | list + hash |
| top-k / median stream | heap / two heaps |
| window max | deque of indices |
| next greater | monotonic stack |
| BFS / Dijkstra | queue / min-heap |
| leaderboard / cancel-by-id | hash + ordered set / three_plus |
| getRandom O(1) | vector + index map |
