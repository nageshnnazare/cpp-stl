# Delay queue — time buckets plus cancel-by-id

**Need:** `schedule(id, when, payload)`, `cancel(id)`,
`pop_ready(now)` (every job with `when <= now`),
`next_deadline()`. Jobs are not FIFO — they are **time
ordered**. Cancel is by id, at a time you do not have in
hand.

```
  by_time (map)
    10:  [ jobA ] [ jobC ]
    25:  [ jobB ]
    40:  [ jobD ]

  loc[id] -> (time, list iterator)

  cancel(C):  erase node from the 10-list; if list empty, erase 10
  pop_ready(25):  drain 10 and 25
```

```
  map<time, list<Job>>              ordered deadlines
  unordered_map<id, {time, it}>     cancel / "already scheduled?"
```

A `priority_queue` of `(when, id)` is fine if you **never
cancel** (or lazy-skip cancelled ids with a dead set — a
third container again). Honest cancel wants a `list` node.

## Why not one `set<{when,id}>`

A `set` of pairs **does** cancel in O(log n) if you store
`{when, id}` and you still know `when`. The hash of id → when
is then the second container. The `map`+`list` form wins when
**many jobs share a timestamp** (one tree node, O(1) splice)
or payloads are fat (move the node, not the payload, on
internal reshuffles — here they do not reshuffle).

| Verb | Containers touched |
| --- | --- |
| C schedule | `by_time[t].push_back`, `loc[id]=` |
| R pop_ready | walk `by_time.begin()` while `<= now` |
| D cancel | `list.erase(it)`, maybe `map.erase(t)`, `loc.erase` |

Double-destroy (cancel after pop) is the bug: `pop_ready`
must `loc.erase` every drained id so a late cancel is a no-op.

Full program: [`examples/56_recipe_delayed_queue.cpp`](../examples/56_recipe_delayed_queue.cpp).

## Related problems

- Task scheduler with `n` cooldown — different
  ([`task_scheduler.md`](task_scheduler.md)); that heap is
  "next eligible count", not wall-clock cancel
- Authentication Manager (1797) — hash + time; count live
  tokens
- [`ttl_cache.md`](ttl_cache.md) — same expiry `set`, plus LRU
- Cron / job runner / `timerfd` wheel: this recipe
- Meeting rooms: heap of end times, **no cancel**
