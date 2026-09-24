# 03. Iterator adapters: reverse, insert, move, streams

[`01_iterators.md`](01_iterators.md) is the category ladder.
This page is the **adapters**: objects that *look* like iterators
so an algorithm can talk to a container, a stream, or the
opposite direction without a new overload.

```
  algorithm  ----writes through---->  iterator
                                            |
                    +-----------------------+------------------+
                    |                       |                  |
                    v                       v                  v
              existing slot           push_back / insert    ostream
              (copy into v[i])        (back_inserter)       (<< token)
```

Header: `<iterator>`.

## Reverse — the same elements, `--` instead of `++`

```
  v:           [ A | B | C | D ]
  begin ------>  ^               ^  <------ end
  rbegin ----------------------->  ^
  rend -->  (one-past A, backwards)

  *rbegin() == D == *prev(end())
```

`rbegin()` / `rend()` are `reverse_iterator`. `std::reverse` *mutates*
the range; a reverse iterator **walks** it. Use reverse iterators
when the algorithm must see the sequence backwards but you do not
want to shuffle memory (search from the tail, `lower_bound` on a
descending view — usually a comparator on the forward range is
clearer).

`it.base()` on a `reverse_iterator` points **one forward** of `*it`.
Off-by-one lives here:

```
  reverse it names D
  it.base() names end() (one past D)
  to erase D via a reverse iterator:  v.erase(std::next(rit).base())
                                      or v.erase(std::prev(rit.base()))
```

## Insert iterators — `copy` that **grows**

```
  back_inserter(v)    *it = x   becomes   v.push_back(x)
  front_inserter(d)               becomes   d.push_front(x)   (deque/list)
  inserter(s, hint)               becomes   s.insert(hint, x)
```

These are **output** iterators. They do not read. `++` is a no-op
that returns `*this`. That is why an infinite `generate_n(...,
back_inserter(v))` just grows `v`.

**Why not `v.resize` then `copy` into `begin()`?** Fine when you
know `n` and `T` is cheap to default-construct. `back_inserter` is
the right default for a filter (`copy_if`) whose output size you
do not know. `reserve` first if you know a cap.

`inserter(s, s.end())` on a `set` still **orders** by key. The
hint only helps if it is close (amortized O(1) vs O(log n)).

## Move iterators — steal, do not copy

```
  vector<string> src = {"a","bb","ccc"};
  vector<string> dst(make_move_iterator(src.begin()),
                     make_move_iterator(src.end()));
  // dst owns the buffers; src holds moved-from strings
```

`std::move(first, last, out)` does the same for a copy-shaped
loop. Use move iterators when the **constructor** of the dest
container takes an iterator pair (the range constructor). After
a move, treat `src` as empty-ish: `clear()` it if you will reuse.

Do not mix move iterators with an algorithm that **reads twice**
(`max_element` then copy) — the first pass already stole.

## Stream iterators — the container is a file

```
  cin  -->  istream_iterator<int>(cin)     reads with >>
            istream_iterator<int>()        default = end-of-stream

  cout <--  ostream_iterator<int>(cout, " ")
```

```
  [ 3 1 4 1 5 ] as text
       |
       v  istream_iterator   (single pass, input iterator)
       |
       copy  -->  back_inserter(v)
```

**Why not `while (cin >> x)`?** Same machine. The iterator form
plugs into `copy`, `accumulate`, `set` range constructors.

`istreambuf_iterator<char>` copies **bytes** (no `>>` skip of
whitespace). That is how you slurp a file into a `string`.

Stream iterators are **single-pass**. Hold one, increment, the
old copy is a zombie. Do not `find` on an `istream_iterator`
and then walk from `begin` again.

## Counted / raw pointers

A `T*` to a buffer of n is a contiguous iterator.
`copy(p, p+n, ...)` is legal. Prefer `span<T>(p, n)` so the
length travels with the pointer ([`containers/span.md`](containers/span.md)).

## Why adapters exist at all

The library has ~100 algorithms × ~10 containers ≈ 1000
overloads if each algorithm took a container. Iterators make
that **addition**: one `copy`, many destinations. Adapters are
how a destination that is not "a slot in memory" still looks
like an iterator.

Practice: [`examples/15_iterator_adapters.cpp`](examples/15_iterator_adapters.cpp).
Next: [`04_insert_emplace_extract.md`](04_insert_emplace_extract.md).
