# Ping-Pong / Double Buffering (DMA) — Theory & Derivation

Companion to `follow_ups.md` (Stage 3: DMA & cache coherency). This file derives
**why** double buffering / ping-pong exists, from the single-buffer limitation up
to multi-buffer and flow-control decisions. Same DMA context as the UART driver.

---

## 1. Starting point — single-buffer DMA RX

Model:
```
DMA fills buf --> DMA-complete IRQ --> CPU invalidates + reads buf --> restart DMA
```
One buffer, used in two phases that **cannot overlap**:
1. **DMA phase** — hardware is writing into `buf`.
2. **CPU phase** — after completion, CPU reads/processes `buf`.

**Why they can't overlap (single buffer forces one owner at a time):**
- While DMA writes `buf`, the CPU must not read it (partial data + coherency mess).
- While CPU reads `buf`, DMA must not write it (overwrites unconsumed bytes → torn data).

So the timeline is strictly **serial**:
```
[ DMA fills buf ][ CPU processes buf ][ DMA fills buf ][ CPU processes buf ] ...
                 ^^^^^^^^^^^^^^^^^^^^^
                 DMA is STOPPED / not-yet-restarted here
```

---

## 2. The problem the single buffer creates

**Failure A — data loss / overrun.** During the CPU's processing window the DMA is
stopped (or not yet restarted). A continuous source (UART stream, ADC, audio,
sensor) **keeps producing at line rate** — those bytes are **missed / dropped**, or
raise an **overrun error (ORE)**. The device does not pause because the CPU is busy.

**Failure B — no overlap = wasted parallelism.** CPU-processing time and
DMA-fill time are **mutually exclusive on the same memory**. Two activities that
*could* run in parallel are forced to serialize → lost throughput, and this is what
creates the gap in Failure A.

**Failure C — DMA races ahead (if restarted too eagerly).** If DMA restarts and the
CPU is still reading the old buffer, DMA **overwrites unconsumed data** → torn /
corrupted reads.

Root cause: **one buffer forces one owner at a time.**

---

## 3. The fix — ping-pong / double buffering

Give each party its **own** buffer so they work **simultaneously**:
- DMA fills **buffer A** while CPU processes **buffer B**.
- When DMA finishes A, it **immediately switches to filling B** (no stop → no gap →
  Failure A solved); the CPU switches to processing A.
- They **ping-pong**: at any instant one buffer is "owned by DMA", the other "owned
  by CPU"; they swap roles at each completion.

```
Single buffer (serial):
[ DMA fills ][ CPU processes ][ DMA fills ][ CPU processes ]
             ^ DMA idle         ^ CPU idle during fill

Ping-pong (overlapped):
DMA:  [ fills A ][ fills B ][ fills A ][ fills B ]   <- never stops
CPU:            [ proc A  ][ proc B  ][ proc A  ]    <- always has a full buffer
```

**Principle:** decouple the **producer (DMA)** from the **consumer (CPU)** *in time*
so neither waits for the other. This is the **same decoupling as the Stage 2 ring
buffer** — producer and consumer work on different regions. Ping-pong is essentially
a **2-slot ring buffer at DMA (whole-buffer) granularity** instead of per-byte.

---

## 4. The hard real-time deadline

When DMA finishes A and starts filling B, the CPU must **finish processing A before
DMA finishes filling B** (because DMA then swaps back to A and starts overwriting it).

**Deadline = one buffer's fill time:**
```
T_fill = (buffer size in bytes) / (data rate in bytes/sec)

Requirement:  T_process(A)  <=  T_fill(B)
```

Example — UART RX @ 115200 baud (~11,520 B/s), 256-byte buffer:
```
T_fill = 256 / 11520  ~=  22.2 ms   -> CPU has ~22 ms to invalidate + process A.
```

**Miss the deadline → corruption, NOT graceful backpressure.** Key distinction:
- Classic backpressure (ring buffer / socket) = producer blocks or is told to slow
  down.
- **DMA hardware and the wire cannot be throttled** (no flow-control handshake unless
  the protocol provides one — RTS/CTS, XON/XOFF). So a missed deadline = DMA
  overwrites unconsumed data = **data loss / silent corruption**, not polite waiting.

Accurate framing: *consumer average rate must keep up with producer fill rate, AND
per-buffer worst-case processing time must fit within one `T_fill`.*

---

## 5. Two distinct failure regimes (they need different fixes)

1. **Fast on average, occasional latency spikes** (a higher-priority ISR delays the
   consumer past one `T_fill`). → **Solved by more than two buffers.** With N
   buffers the CPU can fall behind by up to (N-1) buffers and catch up; the extra
   slots **absorb jitter**. This is why audio / high-speed capture use **N-buffer /
   circular DMA with multiple descriptors**, not just 2.

2. **Fundamentally too slow** (average `T_process > T_fill`). → **No number of
   buffers saves you** — all slots eventually fill and overflow. Fixes: process
   faster, reduce data rate, or apply **real protocol-level flow control** to
   throttle the source.

---

## 6. Design decisions

- **Double buffer (2 slots):** enough when `T_process < T_fill` reliably, low jitter.
- **N buffers / circular DMA:** needed when latency **jitter** exists; extra slots =
  shock absorber for the consumer being momentarily late.
- **Buffer sizing tradeoff:** bigger buffer → longer `T_fill` → more slack per
  buffer, BUT higher **latency** (wait longer before a full buffer is ready to
  process). Classic **latency vs robustness** tradeoff.
- **When fundamentally rate-limited:** buffering only delays the overflow; you need
  flow control or a faster consumer.

---

## 7. Hardware realizations (how DMA engines implement this)

- **Two separate transfers, restart-on-complete:** simplest — completion ISR
  reprograms the DMA to the other buffer. Small risk of a gap during the ISR
  latency; often mitigated by hardware double-buffer mode.
- **DMA double-buffer / ping-pong mode:** hardware holds two memory pointers and
  auto-switches on completion (no CPU needed to swap) → truly gapless.
- **Circular DMA + half-transfer interrupt:** one big buffer treated as two halves;
  DMA wraps continuously. **Half-transfer IRQ** fires at the midpoint (process first
  half while DMA fills second), **transfer-complete IRQ** at the end (process second
  half while DMA wraps to fill first). This is ping-pong emulated on one circular
  buffer — very common on STM32-class parts.
- **Descriptor / linked-list DMA (scatter-gather):** a chain of N descriptors, each
  pointing at a buffer; DMA walks the list (and can loop) → N-buffer scheme without
  per-completion CPU reprogramming.

---

## 8. Cache-coherency interaction (ties to Stage 3)

Each buffer is still a DMA buffer, so the non-coherent rules apply **per buffer**:
- **RX**: invalidate a buffer's cache lines **after** its DMA fill completes, before
  the CPU reads it (the other buffer is being filled meanwhile — don't touch it).
- Each buffer must be **cache-line aligned and size-padded** so a destructive
  invalidate on one buffer never clobbers the other buffer or a neighbor sharing a
  boundary line. With ping-pong this matters **more**, because two buffers sit near
  each other in memory — a shared boundary line between A and B is a real corruption
  path. Align + pad both, or place them in a non-cacheable region.
- Still need **barriers** before starting/switching DMA (coherency ≠ ordering).

---

## 9. The through-line

```
single buffer  --(DMA idle gap -> data loss)-->  ping-pong (overlap producer/consumer)
   --(deadline = T_fill; miss => corruption, not backpressure)-->
   --(latency jitter)-->  N buffers / circular DMA (absorb jitter)
   --(consumer fundamentally too slow)-->  no buffering helps; need flow control
```

Same decoupling principle as the Stage 2 ring buffer, applied at whole-buffer
granularity. Buffering hides **latency/jitter**, never a permanent **rate deficit**.

Soundbite: *"Single-buffer DMA forces CPU and DMA to take turns, so the DMA-idle
window drops incoming data. Ping-pong gives each its own buffer to overlap them; the
CPU's deadline is one buffer's fill time, and missing it corrupts data because the
source can't be throttled. Two buffers cover the steady state, N buffers absorb
jitter, but a consumer that's fundamentally slower than the source needs real flow
control — buffering only hides jitter, not a rate deficit."*
