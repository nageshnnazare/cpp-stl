# Task scheduler / reorganize — heap now, `queue` for cooldown

**Need:** emit the most frequent remaining task, but the same
id cannot appear again before `n` idle slots.

```
  tasks A A A B B B    n=2

  A _ _ A _ _ A
  A B _ A B _ A B     (fill with next most frequent)

  heap of remaining counts:  A:3 B:3
  after picking A:  A:2,  sit in a cooldown queue until time t+n
```

## Two containers

```
  max-heap           remaining counts still eligible
  queue of (ready_time, count, id)
                     items cooling down
```

Each time unit:

1. Pop from cooldown into the heap if `ready_time == now`.
2. Pop heap, emit, if count left push into cooldown with
   `now + n`.

**Why a heap?** Always pick the largest remaining (greedy).
**Why a queue for cooldown?** FIFO by time; a `map<int,
vector<...>>` of wake-up time also works (and skips idle
directly to the next wake). The queue is enough if you simulate
idle ticks.

## Reorganize string (no adjacent duplicates)

Same greedy: max-heap of counts. Pop two different letters
(or one if heap size 1 and we fail). Cooldown is **1**, so a
queue of length 1 or just "hold the last letter out of the
heap for one step":

```
  hold the just-used letter
  push it back after the next pick
```

That "hold" is a one-element cooldown queue.

## Why not `sort` the counts once

Counts **change** after each emit. Re-sorting each step is
O(k log k) per step with k = alphabet size (26) — actually
fine for task scheduler with 26 letters! The heap is the same
O(log 26). For large alphabets the heap is the real win.

A `multiset` of counts works too (delete old count, insert
count-1).

## Related problems

- Task Scheduler (621)
- Reorganize String (767), Rearrange String k Distance Apart
  (358)
- Maximum CPU load / meeting rooms (heap of end times) —
  related "always the next finishing"
- Huffman coding (heap of frequencies, different combine)

Run [`examples/39_recipe_scheduler.cpp`](../examples/39_recipe_scheduler.cpp).
