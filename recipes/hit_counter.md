# Hit counter / rate limiter — a `queue` of timestamps

**Need:** record a hit at time `t`, and answer "how many hits
in the last 300 seconds?" Times are non-decreasing.

```
  t:  1  2  3  ...          301
  q:  [1, 2, 3]             pop 1 (1 <= 301-300), count = 2
```

The queue **is** the window. You never need random access.
A `deque` is the same (and is what `queue` uses anyway).

## Why a queue, not a counter or a map

| Design | hit | getHits | Notes |
| --- | --- | --- | --- |
| integer `count` | O(1) | O(1) | cannot expire |
| `vector` keep-all + scan | O(1) | O(n) | |
| `map<time, count>` | O(log n) | O(log n) + walk | batched same-second hits |
| **`queue` of times, pop expired** | A O(1) | A O(1) amortized | **the one** for 1 hit/call |
| ring buffer of 300 buckets | O(1) | O(1) | when you **bucket by second** |

Amortized: each timestamp is pushed once and popped once.

If many hits share a second, store `{time, count}` in the
queue (or 300 slots: `hits[t % 300]` plus a `stamp[t % 300]`
to detect wrap). That is the production rate-limiter shape.

## Sketch

```cpp
std::queue<int> q;

void hit(int t) { q.push(t); }

int get_hits(int t, int window = 300) {
    while (!q.empty() && q.front() <= t - window) q.pop();
    return (int)q.size();
}
```

**Leaky bucket / token bucket** (throttle outgoing) is a
scalar + last timestamp, not a queue of events. **Sliding
log** (this recipe) is exact. **Sliding window counter**
(two buckets) is approximate.

Full program: [`examples/45_recipe_hit_counter.cpp`](../examples/45_recipe_hit_counter.cpp).

## Related problems

- Design Hit Counter (362)
- Logger Rate Limiter (359) — `unordered_map<message, last_t>`
  (one key per message, not a global window)
- Number of recent calls (933) — this exact queue
- Moving average from data stream — queue of values + running
  sum ([`containers/queue.md`](../containers/queue.md))
- Sliding Window Maximum — deque of **indices**, not a FIFO
  of times ([`sliding_window.md`](sliding_window.md))
- 429 / API gateway: "100 req / 10 s / IP" — hash of IP →
  this queue (or token bucket per IP)
