# 11. Threads and exceptions: what the STL promises

The library is **not** a concurrent data structure collection.
It **does** specify what happens when `T`'s constructor throws
during `push_back`. Mixing those two ideas up wastes weekends.

## Thread safety in one box

```
  many threads, all const methods on the same container
      (find, [], size, iteration that does not write)
      =>  OK   (C++11: concurrent const is defined)

  any thread calls a non-const method
      (insert, erase, operator[] on map that may insert, sort)
      =>  you provide the mutex.  data race = UB

  two containers, two threads, no shared T
      =>  OK
```

`operator[]` on `map` is **non-const** and may insert. A
"read" that uses `[]` is a write. Use `find` / `at` / `contains`.

`shared_mutex` for many-readers-one-writer is the usual wrap
when a `unordered_map` is a cache. The STL does not do that
for you. Intel TBB / `concurrent_hash_map` are different
libraries.

Never share a `reserve` / realloc with a reader holding a
pointer into the buffer. Invalidation rules still apply; the
race just makes them non-deterministic.

## Exception guarantees

| Guarantee | Meaning | Example |
| --- | --- | --- |
| No-throw | will not throw | `size()`, move of `vector` (pointers), `swap` of containers |
| Strong | throw ⇒ state unchanged | `push_back` when `T` move is `noexcept` |
| Basic | throw ⇒ valid but unspecified | some `insert` middle; `T` copy throws half-way |
| None | throw ⇒ the container is garbage | you broke `T`'s destructor |

**Practical rules:**

1. Do not throw from `T::~T()`. Containers call it while
   unwinding.
2. Mark move `noexcept` when true — `vector` realloc becomes
   strong and fast ([`08`](08_types_in_containers.md)).
3. `std::copy` into overlapping ranges is UB, not an exception.
4. `at()` throws `out_of_range`. `[]` does not. Pick.
5. Algorithms do not "roll back" a half-finished `transform`
   unless documented. Treat a throwing functor as basic
   guarantee: some elements already written.

```
  push_back while reallocating
     noexcept move:  new buffer, steal, destroy old, done
                     throw from T's constructor of the *new* element:
                     old buffer still intact, strong
     throwing move:  library copies instead, same idea
```

## What about `std::expected` / error codes?

STL containers use **exceptions** for true contract violations
(`at` out of range, `bitset::to_ulong` overflow) and **return
values** for normal outcomes (`insert` → `{it, false}` if
duplicate, `find` → `end()`). Do not wrap `find` in try/catch.
Do check `end()`.

Next: [`12_debug_measure.md`](12_debug_measure.md).
