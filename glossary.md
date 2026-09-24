# Glossary

Words this tutorial uses with a **fixed** meaning. If a chapter
felt slippery, the slip is often here.

**Adapter.** A type that restricts a real container (`stack`,
`queue`, `priority_queue`) or that *looks* like an iterator
(`back_inserter`). Not a container of its own.

**Amortized O(1).** Each call is O(1) on average over a
sequence; one call in the sequence may be O(n) (vector
`push_back` on realloc).

**Associative.** Key-based containers: `set` / `map` /
`unordered_*`. Opposite of sequence.

**Begin / end.** Half-open range `[begin, end)`. `end` is not
dereferenceable. "Not found" from an algorithm is `end()`.

**Bidirectional iterator.** `++` and `--`. `list`, `set`, `map`.

**Bucket.** One slot of a hash table. Collisions chain or
probe inside it.

**Capacity.** Allocated slots, ≥ size. `reserve` changes
capacity; `resize` changes size.

**Contiguous iterator.** Random-access **and** `&*(it+n) ==
&*it + n`. `vector`, `array`, `string`, `span`. Not `deque`.

**CRUD.** Create, Read, Update, Delete — the four questions
every container answers, some of them slowly.

**Emplace.** Construct `T` from arguments *inside* the
container. Contrast `insert` of an already-built `T`.

**Equivalence.** For a strict `<`, `!comp(a,b) && !comp(b,a)`.
`set` treats equivalent keys as duplicates.

**Extract / node handle.** C++17: unlink a tree/hash node
without destroying `T`. Lets you mutate a `map` key and
relink.

**Forward iterator.** Multi-pass `++` only. `forward_list`,
`unordered_*`.

**Heterogeneous lookup.** `find` with a type other than `Key`
(`string_view` into `set<string>`) via a transparent
comparator or hash.

**Hint insert.** `insert(iterator, value)` on a tree; O(1)
amortized if the hint is the correct successor.

**Input / output iterator.** Single-pass read or write.
Streams.

**Invalidation.** An iterator or reference that used to name
an element and now does not (realloc, erase, rehash). Use is
UB.

**Load factor.** `size / bucket_count` in a hash table.
`reserve` lowers it before a bulk insert.

**Move iterator.** `*it` is an rvalue. Range constructors can
steal.

**Node-based.** Each element is its own allocation with
pointers (`list`, `set`, `map`, typical `unordered_*`).
Erase of one does not move the others.

**PMR.** Polymorphic memory resource: type-erased allocator
arena (`std::pmr::*`).

**Predicate.** Callable returning something usable as `bool`.
Must tolerate being called more than once.

**Projection.** C++20: map an element to a key before
compare/find (`&Person::age`).

**Random-access iterator.** `it+n`, `it[i]`, `a-b` in O(1).
`vector`, `deque`, `array`.

**Range.** C++20: a thing with `begin`/`end` (possibly a
sentinel end). A view is a non-owning range.

**Rehash.** Hash table rebuild; all iterators die.

**Sequence.** Order is insertion / index: `array` `vector`
`deque` `list` `forward_list` `string`.

**Sentinel.** End object that may have a different type from
the iterator; answers "are we done?"

**SSO.** Small-string optimization: short `string` lives
inside the object, no heap.

**Stable.** Equal keys keep their relative order (`stable_sort`,
`stable_partition`).

**Strict weak ordering.** The laws `<` must obey for `sort` /
`set`. `<=` is not strict.

**Strong exception guarantee.** Throw ⇒ the container looks
as it did before the call.

**Transparent comparator.** `is_transparent` typedef;
enables heterogeneous lookup.

**View.** Non-owning (`span`, `string_view`, range views).
Does not keep the owner alive.

**Word (bitset).** Native integer width used to pack bits.
`count()` is O(N/word), not O(N) bit tests.

**Axis.** A promised cheap lookup (by id, by score, by time).
One axis = one container. Destroy must unhook every axis.

**Hot operation.** The CRUD verb that runs in the inner loop.
The container is chosen so *this* verb is cheap.

**QoI.** Quality of implementation — faster than the standard
requires, not a portable contract (SSO size, `par` backend).

