# Rules: the STL law book

Quote these. They are the constraints the library will not
bend. The method that uses them is
[`14_read_a_problem.md`](14_read_a_problem.md). Details and
tables: [`02_decision_guide.md`](02_decision_guide.md).

Numbering is stable. "R12" in a review comment means this
page.

---

## Defaults

**R1.** Default to `vector`. Leave it only when a hot
operation is the wrong shape (front insert, key lookup,
ordered predecessor, O(1) splice of a held node, compile-time
N, non-owning view).

**R2.** Asymptotics pick the family. Cache and constants pick
the winner inside a family. `vector` scan often beats
`map` find until n is large.

**R3.** Dense ids `0..n-1` are a `vector`. `map<int,V>` is a
tree of integers.

**R4.** One cheap lookup axis **the statement promised** = one
container. Two written axes = two containers. Destroy updates
every axis **that exists**. If delete is not in the sentence,
there is no destroy checklist. Do not invent cancel, order,
or “by id.”

**R5.** "I insert in the middle a lot" is not a reason for
`list` unless you **already hold** an iterator there.
Finding the spot is O(n) in both `list` and `vector`.

---

## Sequences

**R6.** `push_back` is amortized O(1). One call may be O(n)
realloc. `reserve(n)` when you know n.

**R7.** `reserve` does not create elements (`v[0]` is UB).
`resize` does.

**R8.** `clear` keeps capacity. To free: `vector<T>().swap(v)`
or `shrink_to_fit` (not guaranteed).

**R9.** Realloc invalidates **all** iterators, pointers, and
references into a `vector`/`string`. Holding `v[0]` across
`push_back` is a dangling reference.

**R10.** `erase` in the middle of a `vector` is O(n) and
invalidates at and after the hole. A loop of `erase` is
O(n²). Prefer erase-remove / `erase_if`.

**R11.** `deque` is random-access **not** contiguous. No
`data()`. End insert does not move elements; iterators are
still fragile — do not hold a `deque::iterator` across
`push_front`.

**R12.** `list` is for **stable handles** and `splice`, not
for "a list of things". No `operator[]`.

**R13.** `forward_list` has no `size()`, no `--it`, erase is
`erase_after`. Almost always `vector` is faster.

**R14.** `array<T,N>`: N is a compile-time constant. No
`push_back`. No heap.

**R15.** `vector<bool>` is not a `vector`. Use `vector<char>`
or `bitset<N>`.

**R16.** `string` may use SSO. A `string_view` into a string
that then grows can dangle even from a "small" string.

---

## Associative

**R17.** `unordered_map` for lookup without order.
`map` for sorted walk, predecessor, closest key, sweep.

**R18.** `operator[]` on `map` / `unordered_map` **inserts**
a default `V` if the key is missing. It is a write. It is
not legal on a `const` map. Membership is `find` /
`contains` / `count`. Counting (`++m[k]`) is the intended
use of `[]`.

**R19.** `at(k)` throws if missing. `find` returns `end()`.

**R20.** Keys in `set`/`map`/`unordered_*` are `const`. To
change a key: `extract`, mutate `nh.key()`, `insert`.
To change a `map` **value**: `it->second =`.

**R21.** `insert({k,v})` does not overwrite. `insert_or_assign`
does. `try_emplace` constructs `V` only when `k` is new.

**R22.** `set` uniqueness is `operator<` (equivalence).
`unordered_set` uniqueness is `hash` then `==`. They must
agree: `a == b` ⇒ `hash(a) == hash(b)`.

**R23.** A comparator for `sort`/`set` must be a **strict**
weak ordering. `<=` is undefined behavior. `NaN` in a
`set<double>` is undefined.

**R24.** Hash quality: hash **every** field that `==` uses,
and mix the bits. Hashing only `pair.first` collapses
buckets.

**R25.** There is no `std::hash<pair<T,U>>`. Provide one, or
do not use `pair` as an unordered key.

**R26.** `reserve(n)` on unordered containers before a known
flood. Insert during iteration may **rehash** and kill **all**
iterators.

**R27.** Member `s.lower_bound(x)` on a tree is O(log n)
hops. `std::lower_bound(s.begin(), s.end(), x)` is O(n)
hops. Always the member.

**R28.** Transparent `set<string, less<>>` / transparent
hash+eq: `find(string_view)` without a temporary `string`.

**R29.** `multiset::erase(value)` erases **every** equivalent
element. `erase(find(value))` erases one.

**R30.** Hint insert (`insert(it, v)`) is amortized O(1) only
if `it` is the correct successor. A wrong hint is still
O(log n), not a speedup.

---

## Adapters

**R31.** `stack`, `queue`, `priority_queue` are not
containers. No iterators. No range-for. No `erase(7)`.

**R32.** Default `priority_queue` is a **max**-heap.
Min-heap: `priority_queue<T, vector<T>, greater<T>>`.

**R33.** Heap: `top` O(1), `push`/`pop` O(log n). To delete
an arbitrary value: `multiset`, or lazy tombstones.

**R34.** `pop()` returns `void`. Save `top()`/`front()` first.

**R35.** Empty `top`/`front`/`back`/`pop` is undefined
behavior. Check `empty()`.

**R36.** Dijkstra is a **min-heap**, not a FIFO `queue`.
Unweighted shortest path is a `queue`, not a heap.

---

## Iterators and invalidation

**R37.** A range is half-open `[begin, end)`. `end()` is not
dereferenceable. Algorithms return `end()` for "not found".

**R38.** `std::sort` needs random-access. It will not compile
on `list`. Use `list::sort` or copy to a `vector`.

**R39.** `deque` is random-access, not contiguous. You cannot
pass `&d[0]` as a buffer of `d.size()` elements to C.

**R40.** Range-for + `erase` is undefined on every container.
The hidden iterator dies.

**R41.** The one legal erase-while-walk:

```cpp
for (auto it = c.begin(); it != c.end(); )
    if (pred(*it)) it = c.erase(it);
    else ++it;
```

**R42.** `list`/`set`/`map` erase invalidates **only** that
iterator. Other stored iterators stay. That is why LRU is
legal.

**R43.** `vector` erase invalidates at and after. Realloc
invalidates all. `unordered_*` insert may invalidate all
(rehash). `unordered_*` `erase(it)` invalidates only `it`
and returns the next.

**R44.** `copy` / `transform` **write** through the output
iterator. Into an empty `v.begin()` is UB. Use
`back_inserter`.

**R45.** `reverse_iterator::base()` is one-past `*rit`. Easy
off-by-one if you erase through it. Prefer a forward
`it = erase(it)` loop.

---

## Algorithms

**R46.** `remove` / `remove_if` / `unique` do not shrink.
Always `erase` the tail (or C++20 `erase` / `erase_if`).

**R47.** `unique` is adjacent-only. Sort first, or use a
seen-set to keep first-occurrence order.

**R48.** `nth_element` puts the k-th in place; sides are
**not** sorted. Do not `binary_search` the whole vector
after it.

**R49.** `partial_sort(first, middle, last)` sorts
`[first, middle)`. The rest is unspecified.

**R50.** `stable_sort` keeps equal-key order. `sort` does not.
`stable_sort` needs extra memory.

**R51.** Binary search family requires the range to be
partitioned the same way as the comparator. Unsorted
`lower_bound` is a wrong answer, not a compile error.

**R52.** Set algorithms (`merge`, `set_intersection`, …)
require **both** ranges sorted the same way.

**R53.** `accumulate`'s result type is the type of the **init**.
`0` is `int`. `0LL` is `long long`. `""` is `const char*`,
not `string`.

**R54.** `reduce` may reorder. Use `accumulate` when order
matters (`float` sum, string concat).

**R55.** Predicates may be called more than once. They must
not mutate the range, must not assume they run once, and
must not lock under `execution::par_unseq`.

**R56.** A `ranges` view does not own. Storing
`filter_view` of a temporary vector is dangling (and often
ill-formed). Materialize with `copy` / `ranges::to`.

---

## Types, memory, exceptions, threads

**R57.** `T` in a `vector` should be `noexcept` movable or
realloc **copies**. Mark real steal-moves `noexcept`.

**R58.** Rule of Zero: if members already own, write no
special members. If you write one of the five, write all
five.

**R59.** `initializer_list` copies from `const T[]`. You
cannot move `unique_ptr` out of a braced list. Emplace in
a loop.

**R60.** No STL container is a concurrent data structure.
Concurrent **const** methods on one object are defined.
Any non-const vs another thread is a data race unless **you**
lock. `map[]` is non-const.

**R61.** Concurrent writes to **different** `vector` elements
are OK **if** no realloc/insert/erase. Still lock if unsure.

**R62.** `execution::par` still needs exclusive ownership of
the range. It is not a license to share a container.

**R63.** `push_back` of a `noexcept` move gives the **strong**
guarantee (throw ⇒ state unchanged). A throwing move may
force copies on realloc.

**R64.** PMR does not add locks. The mutex belongs on the
container, and on the resource if several containers share it.

**R65.** Store owners (`vector`, `string`, `map`). Pass views
(`span`, `string_view`). Views do not keep owners alive.

---

## Performance and honesty

**R66.** `list`/`map`/`unordered_map` pay a heap node per
element. n = 1e6 ints: `vector` ~4 MB; `list` tens of MB
and cold. Do not "upgrade" to nodes because it feels advanced.

**R67.** If you will `sort` the keys anyway, build a
`vector<pair>` and sort it. Do not fill a `map` then dump it.

**R68.** Online inserts + ordered queries: `map`/`set`.
Build-once, many lookups: sorted `vector` + `lower_bound`
(C++23: `flat_map`).

**R69.** Measure before replacing `unordered_map` with a
clever tree, and before `par`. A 100-element `par` sort
loses to fork/join.

**R70.** Hash worst case is O(n). Do not promise O(1) to a
security boundary without a hash that resists collision
attacks (or use `map`).

---

## Multi-index designs

**R71.** LRU is `list` + `unordered_map` of iterators
**because** list erase does not invalidate other iterators.

**R72.** A heap cannot reset a leaderboard score. Hash +
`set{-score,id}`; update = erase old pair + insert new.

**R73.** `getRandom` + O(1) delete is `vector` + index map
and **swap-with-last**, not `unordered_set`.

**R74.** Cancel-by-id **and** "next due" is two axes: hash
of handles + a time-ordered structure. A heap alone is not
enough unless cancel is lazy.

**R75.** `multiset::erase(it)` vs `erase(value)`: one node
vs all equivalents. Stock-price correction uses `find` then
iterator erase.

**R76.** Splice / extract / merge **move nodes**, they do not
copy `T`. That is the point of node containers.

---

## Views and supporting types

**R77.** `span` / `string_view` are `(pointer, length)`.
They are parameters, not storage.

**R78.** `optional<T>` is "maybe one T". It is not a
container of many. Do not use `vector` of size 0-or-1 as a
maybe.

**R79.** `pair`/`tuple` in a `set` order **lexicographically**.
Put the field you want to rank on **first**, or use a
custom comparator. Leaderboards use `{-score, id}` so
`begin()` is the winner.

**R80.** If the STL type does not have the operation
(heap erase, concurrent map, stable index after hole-punch),
compose two types or leave the STL. Do not pretend.

---

Cross links: [`cheat_sheet.md`](cheat_sheet.md) (one page),
[`07_pitfalls.md`](07_pitfalls.md) (the same laws as
accidents), [`15_expert.md`](15_expert.md) (invariants the
standard actually guarantees).
