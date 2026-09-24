# 04. Insert, emplace, extract, merge

CRUD "Create" is not one function. The STL has **copy in**,
**construct in place**, **steal a node**, and **splice a tree**.
Picking the wrong one is either a silent extra copy or a
wrong-key insert.

```
  insert(x)       x already exists as a T          copy/move T into the container
  emplace(args)   T is born inside the container   construct from args
  try_emplace     map only                         construct V only if K is new
  insert_or_assign map only                        overwrite V if K exists
  extract(k)      C++17                            unlinks a node, you hold it
  merge(other)    C++17                            splice unique nodes from other
```

## `push_back` vs `emplace_back` vs `insert`

```
  struct Hit { int t; string url; };

  v.push_back(Hit{t, u});      // build a Hit, then move it in
  v.emplace_back(t, u);        // Hit::Hit(int, string) runs in the slot
```

For `int` they are the same. For a type with a heavy constructor
plus a move, `emplace_*` skips the temporary. **If you already
have a `T` in hand, `push_back(std::move(x))` is the honest API.**
`emplace_back(x)` still copy-constructs from `x`.

`insert(v.begin()+i, x)` shifts a tail. Cost is the shift, not
the construct. Do not emplace in a loop at `begin()` — that is
still O(n²) moves.

`vector::emplace` at the middle is the same shift as `insert`.
Emplace is not "cheaper insert". It is "construct without a
temporary".

## Map: `[]` vs `insert` vs `emplace` vs `try_emplace`

```
  m[k] = v;                 if k missing: default-construct V, then assign
                            (V must be default-constructible)
  m.insert({k, v});         if k exists: no-op, returns {it, false}
  m.emplace(k, v);          similar; may construct V even when k exists
                            (then throw it away) — library-dependent waste
  m.try_emplace(k, args...); construct V from args **only** if k is new
  m.insert_or_assign(k, v);  always ends with m[k]==v; returns whether inserted
```

```
  expensive V, key maybe present
       |
       +-- you must not build V on a duplicate  -->  try_emplace
       +-- you want the new V to win            -->  insert_or_assign
       +-- V is cheap and defaulted             -->  m[k] =  is fine
       +-- you are counting                     -->  ++m[k]  (default 0)
```

`insert({k, v})` vs `emplace(k, v)` on a `map<string, int>`:
`emplace` can **move from `k`** even if the insert fails (the
standard allows the rvalue key to be consumed). If you still
need `k` after a failed insert, pass an lvalue or use
`try_emplace`.

## Node handles (C++17) — change a key without copy

A tree node is a heap object with pointers. `extract` unlinks it
and gives you a **handle**: the value lives, no destructor.

```
  map:   [ 8 ]--[ 3 ]--[ 10 ]

  auto nh = m.extract(3);     3 is gone from m, nh owns the node
  nh.key() = 7;               mutate the key (map/set only, ordered)
  m.insert(std::move(nh));    re-link, O(log n) place, **no copy of V**
```

Use this when `V` is huge (or non-copyable) and you need to
**rename** the key. The old recipe (erase + insert) destroys `V`
and rebuilds it.

`extract(it)` is O(1) unlink + the tree fix; `extract(key)` is
O(log n) find then unlink.

Empty handle: `nh.empty()` after a failed `extract(missing)`.

## `merge` — splice unique nodes between trees

```
  a: {1, 3, 5}     b: {3, 4, 5, 6}
  a.merge(b);
  a: {1, 3, 4, 5, 6}
  b: {3, 5}          duplicates stay in the source
```

O(n log n) comparisons in general; **no copies of `T`**. Pointers
to merged elements remain valid (they sit in the same nodes).
`list::splice` is the sequence version of this idea.

**Why not insert in a loop?** Each insert copies/moves `T` and
allocates. `merge` reallocates nothing for the nodes that move.

`unordered_*::merge` is the same contract: unique keys move,
collisions stay in the source.

## `hint` inserts

```
  auto it = s.lower_bound(x);
  s.insert(it, x);              // if it was the correct successor, A O(1)
```

Useful when you insert **already sorted** data into a `set`.
A wrong hint is not a bug — it falls back to O(log n). A
`vector` you `push_back` then `sort` once still beats n hinted
`set` inserts.

Practice: [`examples/16_emplace_extract.cpp`](examples/16_emplace_extract.cpp).
Related: [`05_hash_compare_lookup.md`](05_hash_compare_lookup.md).
