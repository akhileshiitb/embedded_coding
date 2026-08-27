# UART Driver — Senior Engineer Interview Follow-Ups

Q&A distilled from a mock senior/staff embedded interview on the UART driver
(`solution.c`). Answers are written as the "correct, complete" version — including
the nuances that separate a senior from a staff-level answer.

---

## Q1. `volatile` on memory-mapped registers

**Q:** Your poll loops do `while (!(dev->regs->SR & UART_SR_TXE)) { }`. What does
`volatile` guarantee? What breaks without it? Does it give ordering or atomicity?

### (a) What `volatile` guarantees / what breaks without it
- `volatile` forces the compiler to **actually perform each read/write** to the
  register — it may not cache the value in a CPU register or assume it is
  unchanged between iterations.
- Without `volatile`, the compiler sees nothing in the loop body writes `SR`, so
  it reads `SR` **once** into a register and spins on the cached copy →
  **infinite loop even though the hardware flag flipped**. It may also hoist the
  read out entirely and delete the loop.
- `volatile` also prevents **eliding/coalescing** accesses: two consecutive `DR`
  writes are not merged into one, and a read whose result looks "unused" is not
  dropped. Critical for registers with **read side effects** (e.g. reading `DR`
  clears `RXNE`).

### (b) Ordering and atomicity
There are **two distinct reordering risks** with **different fixes**:

1. **Compiler reordering** — `volatile` *does* guarantee ordering **among
   volatile accesses themselves**, but it can reorder a volatile access relative
   to a **non-volatile** one (e.g. a plain flag update floating across a register
   write).
2. **CPU / hardware reordering** — `volatile` does **nothing** here. The CPU can
   reorder memory accesses. This is controlled by the **memory type**: marking
   the peripheral region as **Device / strongly-ordered** in the MMU/MPU keeps
   accesses in program order at the CPU level.
   - Even Device ordering only covers accesses **to the same peripheral/region**.
     Ordering between a UART write and, say, a DMA descriptor in normal cacheable
     RAM needs an explicit **memory barrier** (`DMB`/`DSB` on ARM).

**Complete answer:** `volatile` (compiler, volatile-to-volatile) + Device memory
type (CPU, same region) + barriers (cross-region / DMA / normal↔device).

**Atomicity:** `volatile` does **not** make access atomic. `CR |= EN` is a
read-modify-write (3 bus transactions); an ISR touching `CR` in between is lost.

---

## Q1 deep-dive. Do mutex/spinlock fix the RMW atomicity issue?

**Key principle:** *Locks serialize software agents that agree to cooperate. They
do not serialize the hardware.* First ask: **who else writes this register?**

| Other writer | Correct tool |
|---|---|
| Another task/thread | mutex or spinlock (same lock everywhere) |
| ISR on the **same** core | **disable interrupts** (critical section) — **never a mutex** (can't block in ISR; spinlock self-deadlocks) |
| ISR / code on **another** core (SMP) | **spinlock + local IRQ disable** together (`spin_lock_irqsave`) |
| **Hardware / DMA** | Locks are useless — hardware doesn't honor them. Use **SET/CLEAR registers** or avoid RMW |

- **Same-core ISR self-deadlock:** thread takes spinlock → ISR preempts → ISR
  spins on the same lock → thread can't run to release → deadlock forever. Fix =
  mask IRQs before taking the lock.
- The hardware RMW is **still 3 bus cycles** even with perfect locking; the lock
  only stops *cooperating software* from interleaving.

### UP vs SMP summary

| System | Threats | Mechanism |
|---|---|---|
| Uniprocessor | same-core ISR / preemption | **IRQ disable** does the work; the spinlock is a **no-op**. `spin_lock_irqsave()` is the idiomatic API. |
| SMP | same-core ISR **+** other core | **IRQ disable (my core) + spinlock (other cores)** — which *is* what `spin_lock_irqsave()` bundles. |

- On UP, the load-bearing mechanism is the **IRQ mask**, not the lock.
- On SMP, IRQ-disable and spinlock guard **two different threats** and you need
  both. They are not alternatives — `spin_lock_irqsave()` = the two combined.
- If **no ISR is involved** (thread↔thread on SMP), plain `spin_lock()` (no
  irqsave) is enough — don't pay the IRQ-disable cost needlessly.
- **Keep the critical section tiny** — lock only the register RMW, **never** the
  polling wait loop (`while (!TXE)`), or you block all interrupts for the whole
  transfer and destroy real-time responsiveness.

---

## Q1 deep-dive. Purpose of separate SET / CLEAR (and W1C) registers

- They **eliminate the read-modify-write**: setting a bit becomes a **single
  write** (`CR_SET = EN;`), where writing 0 to other bits **has no effect**. One
  bus cycle, nothing to interrupt → **atomic without any lock or IRQ mask**.
- **Primary motivation is software-vs-hardware atomicity**, not just IRQs. The
  race that locks **cannot** solve is the **hardware** setting sticky/status bits
  in the same word between your read and writeback — a plain `CR |= X` would
  clobber it. SET/CLEAR is the only correct tool there.
- Against a mere ISR you *could* disable interrupts; against the hardware you
  cannot — hence SET/CLEAR is *essential*, not merely convenient.
- **W1C (write-1-to-clear)** is the status-register counterpart: clear exactly the
  flag you acknowledge without disturbing a flag hardware just raised (avoids the
  `SR &= ~A` bug that also clears a freshly-set flag B).
- Caveat: not every IP has them; "write 0 = no effect" is a **property of that
  register** you must confirm in the datasheet, not a universal law.

**Soundbite:** *"SET/CLEAR/W1C eliminate RMW to give single-cycle atomicity
against every concurrent writer — but the case that makes them essential is the
hardware as co-writer, the one race a spinlock or IRQ mask can't cover."*

---

## Q2. The vtable (`driver_ops`) pattern and `void *handle`

### (a) Why `const` on `uart_ops` matters on embedded
- **Language guarantee:** no code can modify the function-pointer table at
  compile time — protects the driver from a corrupted vtable.
- **Section placement:** goes in `.rodata`/`.const`; linker/MMU/MPU can place it
  in **read-only** memory.
- **Embedded-specific:** `const` data can live directly in **flash/ROM**, never
  copied to RAM at boot → **saves scarce RAM** and boot-time copy. A non-const
  vtable would be `.data` (RAM + copy cost).
- **Security:** a corrupted function-pointer table is a classic **control-flow
  hijack** primitive; write-protecting the vtable means even a buffer overflow
  can't repoint `ops->write` to attacker code.

### (b) `void *handle` — tradeoff and the safety cost
- **Benefit:** lets the **generic framework** treat every device uniformly — one
  `driver_ops` shape serves UART/SPI/I2C. Enables polymorphism.
- **Cost (the key point):** `void *` **discards static type safety**. Every
  function begins with an **unchecked downcast**:
  ```c
  struct uart_device *dev = (struct uart_device *)handle;
  ```
  If the framework passes an `spi_device *` by mistake, it **compiles cleanly**
  and reinterprets the bytes → `dev->regs` points at a garbage field → silent
  memory corruption the compiler cannot catch.
- **Mitigation:** a **magic/type tag** in the struct
  (`if (dev->magic != UART_MAGIC) return -1;`) to detect a wrong-type handle at
  runtime; or `container_of`-style patterns.

### (c) When is the vtable indirection worth it vs pure overhead?
- **Worth it:** when there is **>1 implementation** behind one interface and the
  caller must stay **driver-agnostic** (a HAL). The upper layer calls
  `dev->ops->write()` without knowing UART vs SPI vs I2C — decoupling, swappable
  drivers, clean layering (like Linux `file_operations`).
- **Pure overhead:** when there is **one implementation** and the concrete type is
  known at the call site. Costs:
  - **Indirect call** — can't inline; harder to branch-predict/speculate than a
    direct call. Real cycles on a hot per-byte path.
  - **Lost cross-function optimization** (no inlining, no constant propagation).
  - **Extra memory indirection** (load `ops`, load `ops->write`, then call).
- **Framing:** accept the indirection at **layer boundaries**; avoid it in **tight
  inner loops**. Some deeply-embedded teams skip vtables entirely (cost + the
  function-pointer-in-RAM exploit surface) and use link-time or compile-time
  selection instead.

---


# Incremental Driver Evolution (Design Progression)

A logical progression: each stage is motivated by a **concrete limitation** of the
previous one. Polling → thread-safe polling → interrupt-driven + ring buffer →
DMA + cache coherency → power management. The fundamentals from Q1/Q2 reappear at
each step.

---

## Stage 1 — Make the polling driver thread-safe with locks

### Q: The current polling driver is single-threaded. Two tasks call `uart_write()` concurrently. What breaks, and how do locks fix it?

**What breaks (two separate races):**

1. **Interleaved data on the wire.** `uart_write` is not atomic as a whole. Task A
   writes bytes `A0 A1 A2...`, Task B writes `B0 B1...`; the scheduler interleaves
   them → the receiver sees `A0 B0 A1 B1...`. The **byte stream is corrupted** even
   though each individual `DR` write is fine. The critical region is the **entire
   transfer**, not one register access.
2. **Lifecycle RMW races.** `dev->opened = 1`, and checks like `if (!dev->opened)`
   are **non-atomic read-modify-write** on shared state. Two concurrent `open()`
   calls can both pass the "not open" check → double-init.

**The fix — a per-device lock protecting the whole operation:**

```c
struct uart_device {
    ...
    mutex_t lock;   /* per-device, NOT global — one UART's traffic must not block another */
};

int uart_write(void *handle, const uint8_t *data, size_t len) {
    struct uart_device *dev = handle;
    /* validate params BEFORE locking (cheap, no shared state) */
    if (!dev || !data || len == 0) return DRV_ERR_INVALID;

    mutex_lock(&dev->lock);
    if (!dev->opened) { mutex_unlock(&dev->lock); return DRV_ERR_NOT_OPEN; }
    for (size_t i = 0; i < len; i++) {
        /* poll TXE + write DR */
    }
    mutex_unlock(&dev->lock);
    return DRV_OK;
}
```

**Key design decisions & fundamentals:**
- **Granularity: per-device, not global.** A global lock serializes *all* UARTs —
  false contention. One lock per `uart_device` = independent devices run in
  parallel.
- **Lock scope = the whole transfer** (not per-byte), because the invariant being
  protected is "bytes of one write stay contiguous."
- **Mutex vs spinlock (ties to Q1):** here the critical section **contains a
  polling wait** (`while (!TXE)`), which can be long. A **mutex** (sleeps, yields
  the CPU) is correct for a thread context. A **spinlock would be wrong** — you'd
  burn a core spinning for the whole transmit. *Rule: never hold a spinlock across
  a wait loop.*
- **Cannot call this from an ISR** — mutex can block. If an ISR must transmit,
  that's the motivation for Stage 2.
- **The lock does NOT protect against the hardware** (Q1 principle) — but here the
  only writers to the data stream are software tasks, so a lock suffices.

**Limitation that motivates Stage 2:** the caller's thread is **blocked spinning on
TXE** for the entire transfer. On a 9600-baud link, sending 100 bytes ties up the
CPU for ~100 ms. Wasteful. We want `write()` to return immediately.

---

## Stage 2 — Interrupt-driven driver with a ring buffer

### Q: Redesign so `write()` returns immediately instead of blocking on TXE. Walk through the data structures, the ISR, and the critical sections.

**Core idea:** decouple the caller from the hardware with a **ring buffer (FIFO)**.
`write()` copies bytes into the buffer and enables the TX interrupt, then returns.
The **TXE interrupt fires** each time the hardware can accept a byte; the ISR pulls
the next byte from the buffer and writes `DR`. When the buffer empties, the ISR
**disables the TX interrupt** (so it doesn't fire endlessly on an empty buffer).

```c
#define RB_SIZE 256   /* power of two -> mask instead of modulo */
struct ring_buf {
    uint8_t  buf[RB_SIZE];
    volatile uint32_t head;   /* producer writes head (thread) */
    volatile uint32_t tail;   /* consumer writes tail (ISR)    */
};
```

**Producer (thread) — `write()`:**
```c
/* copy bytes into buf at head, advance head */
rb->buf[rb->head & (RB_SIZE-1)] = byte;
rb->head++;
enable_tx_interrupt(dev);   /* kick: ensures ISR will run */
return;                      /* returns immediately — non-blocking */
```

**Consumer (ISR) — TX empty interrupt:**
```c
void uart_tx_isr(struct uart_device *dev) {
    if (rb->tail == rb->head) {          /* buffer empty */
        disable_tx_interrupt(dev);       /* stop the interrupt storm */
        return;
    }
    dev->regs->DR = rb->buf[rb->tail & (RB_SIZE-1)];
    rb->tail++;
}
```

**Why this is (often) lock-free — the SPSC insight:**
- **Single Producer, Single Consumer**: exactly **one** writer of `head` (the
  thread) and **one** writer of `tail` (the ISR). Each index has a single owner;
  the other side only **reads** it.
- With one producer and one consumer, **no lock is needed** — correct index
  ordering (write data *before* advancing head; read data *before* advancing tail)
  plus the reader seeing a consistent index is sufficient.
- **Why `volatile` on head/tail (ties to Q1):** the ISR and thread run in different
  contexts; each must see the other's *latest* index, not a cached copy. `volatile`
  forces the reload. On a weakly-ordered CPU you *also* need a **memory barrier**
  between "write data slot" and "advance head" so the consumer never sees an
  advanced head before the data landed (Q1: `volatile` ≠ CPU ordering).
- **Multiple producers?** Then head becomes a contended RMW → you need a lock
  (or atomic CAS). SPSC lock-freedom is a *property of the 1:1 topology*.

**The TX "kick-start" state machine (the subtle part):**
- If the buffer was **empty** and idle, no TX interrupt is pending, so enabling it
  starts the chain. Some hardware only raises TXE-interrupt on a 1→... transition;
  you may need to **prime the first byte** in `write()` and let the ISR feed the
  rest. The invariant: *TX interrupt is enabled iff there is data to send.*

**RX path (mirror image):** RXNE interrupt fires when a byte arrives → ISR reads
`DR`, pushes into the RX ring buffer. `read()` (thread) pops from it. Here the
**ISR is the producer**, the **thread is the consumer** — SPSC again.

**Critical sections — exactly where (ties to Q1 UP/SMP):**
- If **only** SPSC indices are shared and the CPU is single-core: **no lock**, just
  `volatile` + barrier.
- If `write()` can be called from **multiple threads**: protect the *producer* side
  (head advance + copy) with a **mutex/spinlock** — but now the shared data is also
  touched by the ISR, so a plain mutex is illegal against the ISR. Use
  **`spin_lock_irqsave`** (disable local IRQ so the TX ISR can't preempt mid-update,
  + spinlock on SMP). This is the concrete payoff of the Q1 discussion.
- **Keep the ISR tiny**: one byte in/out, adjust index, return. No loops, no
  blocking, no locks held long. (Top-half/bottom-half: defer heavy work to a
  thread/tasklet.)

**ISR discipline fundamentals:**
- ISRs must be **short and non-blocking** — they run at elevated priority and delay
  everything else.
- **Never take a mutex** in an ISR; never call blocking APIs.
- Long work → **defer** to a bottom half (workqueue / tasklet / DPC / RTOS task).

**Limitation that motivates Stage 3:** at high baud rates, **one interrupt per byte**
becomes crippling — e.g. 1 Mbaud ≈ 100 K interrupts/s, each with save/restore
overhead. The CPU drowns in interrupt entry/exit. We want the hardware to move
blocks **without per-byte CPU involvement**.

---

## Stage 3 — DMA and cache coherency

### Q: Convert the interrupt-per-byte design to DMA. What new class of bug appears, and how do you handle it?

**Core idea:** program a **DMA controller** with a source buffer, destination
(`DR`), and length. The DMA engine transfers the whole block autonomously and
raises **one interrupt at completion** (or half/full for double-buffering). CPU
involvement drops from N interrupts to ~1.

**What changes:**
- `write(buf, len)` → set up DMA descriptor (src=buf, dst=&DR, len), start DMA,
  return. **One** "DMA complete" ISR signals done.
- Enables **zero-copy**, high-throughput streaming; frees the CPU during transfer.

**The new bug class — CACHE COHERENCY (the headline answer):**

On an SoC with a **data cache** where the DMA engine is **not cache-coherent** with
the CPU (common in MCUs/embedded), the CPU and DMA see **different views of memory**:

- **TX (memory → peripheral):** CPU fills `buf` — writes may sit in the **cache**,
  not yet in DRAM (write-back cache). DMA reads from **DRAM** → sends **stale data**.
  **Fix:** *clean/flush* the cache lines for `buf` **before** starting DMA (force
  dirty lines out to DRAM).
- **RX (peripheral → memory):** DMA writes fresh data into **DRAM**. CPU reads `buf`
  but gets a **stale cached copy**. **Fix:** *invalidate* the cache lines for `buf`
  **after** DMA completes (before the CPU reads), so the CPU refetches from DRAM.

Mnemonic: **clean before TX, invalidate after RX.**

**The alignment landmine (staff-level detail):**
- Cache maintenance operates on **whole cache lines** (e.g. 32/64 bytes). If `buf`
  is **not cache-line aligned** or its length isn't a multiple of the line size, a
  maintenance op on `buf` also affects **neighboring data** sharing the boundary
  line → invalidating can **discard a neighbor's unrelated dirty data**, or cleaning
  can be partial. **Fix:** allocate DMA buffers **cache-line aligned** and
  size-padded to a line multiple. Or place DMA buffers in a **non-cacheable** memory
  region (MPU/MMU attribute) — simpler, at a performance cost.

**Ordering / barriers (ties directly to Q1):**
- `volatile` does **nothing** for DMA coherency — it's a *compiler* directive, and
  the DMA sees *memory*, not the compiler's view.
- You need **cache maintenance** (clean/invalidate) **plus a memory barrier**
  (`DSB`) before writing the "start DMA" register, so the buffer writes + cache
  clean are globally visible **before** the DMA engine begins. This is the
  "cross-region ordering needs a barrier" point from Q1 made concrete.

**Coherent alternative:** on SoCs with a **coherent interconnect** (ACE/CHI, or an
IOMMU/SMMU snooping the cache), hardware keeps DMA and CPU in sync — no manual
maintenance. Knowing whether your platform is coherent is the first question to ask.

**Limitation that motivates Stage 4:** the peripheral, DMA engine, and CPU all
consume **power**. On a battery device, leaving the UART clocked and enabled when
idle wastes energy. We want to shut things down when not in use.

---

## Stage 4 — Power management

### Q: How do you add power management to this driver, and what ordering hazards appear?

**Mechanisms:**
- **Clock gating:** disable the UART's peripheral clock when idle (`close()` /
  runtime-idle). No clock → near-zero dynamic power. Biggest, easiest win.
- **Power domains / deep sleep:** the UART's power island can be turned off in deep
  low-power modes — but this **loses register state** (BRR, CR config).
- **Runtime PM:** ungate on first activity, gate after an idle timeout.

**The critical ordering hazards (this is what the interviewer probes):**

1. **Gate the clock only AFTER the transmitter drains.** If you clock-gate while
   `BUSY == 1`, you **freeze mid-byte** → truncated/corrupt output. This is exactly
   why our `uart_close` **drains `BUSY` before clearing enables** — the same
   invariant extends to clock gating: *wait BUSY==0, then gate.*
2. **A gated peripheral's registers are inaccessible / read garbage.** Accessing a
   register with its clock off can **hang the bus** or fault on some SoCs.
   **Invariant:** *ungate the clock BEFORE the first register access; gate only
   AFTER the last.*
3. **State loss across power-off → must re-init on resume.** If the power domain
   dropped, `resume()` must **reprogram BRR/CR** from the stored `dev->config`
   (which is why we keep config in the handle). Save/restore register context.
4. **Wake-up source:** to receive while asleep, the RX line must be configured as a
   **wake interrupt**; the first edge wakes the SoC, but the **first byte may be
   lost** during the wake latency unless hardware buffers it — a known tradeoff.

**Ties to fundamentals:**
- The **`BUSY`-drain-before-teardown** invariant (from the close() design) is the
  same principle reused for clock gating.
- **Ordering matters** again (Q1): clock/power operations must be sequenced with
  register access and barriers — you can't reorder "ungate clock" after "touch
  register."
- **Config stored in the handle** (Q2 state design) is what makes resume possible.

**Full lifecycle with PM:**
`init (store config) → resume/ungate clock → open (enable, program regs) →
read/write → close (drain BUSY, disable) → gate clock → deinit (reset state)`

---

## The through-line (how the stages connect)

| Stage | Limitation solved | New concept | Fundamental reused |
|---|---|---|---|
| 1. Polling + locks | data races between threads | per-device mutex, lock granularity | mutex-vs-spinlock, "never spin across a wait" (Q1) |
| 2. IRQ + ring buffer | CPU blocked spinning on TXE | SPSC lock-free FIFO, ISR discipline | `volatile` + barriers, `spin_lock_irqsave` for multi-producer (Q1) |
| 3. DMA + coherency | interrupt-per-byte overhead | DMA, cache clean/invalidate, alignment | barriers, `volatile`≠coherency, cross-region ordering (Q1) |
| 4. Power management | wasted energy when idle | clock gating, state save/restore | BUSY-drain invariant (close design), config-in-handle (Q2) |

Each step **keeps the same generic `driver_ops` interface** (Q2) — the upper layer
never changes as the implementation evolves from polling to DMA. That is the whole
point of the vtable abstraction.

# Generic Driver Design Fundamentals (Peripheral-Agnostic)

Concepts a senior firmware engineer must reason through when designing **any**
device driver — UART, SPI, I2C, GPIO, timers, ADC, Ethernet, USB, sensors. UART is
just one instance; the grammar below is shared. A driver is ~90% reusable skeleton
+ ~10% peripheral-specific glue (register map, status flags, protocol semantics).

---

## 1. Register & memory correctness
- **`volatile` on all MMIO** — force actual reads/writes; no caching/eliding. Without
  it, poll loops get optimized away.
- **Memory ordering** — two separate risks: compiler reordering (`volatile` orders
  volatile-vs-volatile only) and CPU reordering (needs Device memory type +
  barriers `DMB`/`DSB` for cross-region / DMA ordering). `volatile` is NOT a barrier.
- **Atomicity** — register RMW (`REG |= X`) is multiple bus cycles, not atomic. Use
  SET/CLEAR and W1C registers to avoid RMW where hardware co-writes the word.
- **Struct/register overlay** — exact layout match: explicit `reserved` fields,
  correct alignment/packing. **Avoid C bitfields for registers** (layout is
  compiler/endian-defined) — use masks + shifts.
- **Endianness** — define byte order at bus/network boundaries; know where to swap.
- **Bit discipline** — mask reserved bits, honor write-1-to-clear, prefer SET/CLEAR.

## 2. Concurrency & synchronization
- **First question: "who else touches this state/register?"** → pick the tool:
  - thread ↔ thread → mutex / spinlock (same lock, right granularity).
  - thread ↔ ISR (same core) → **disable interrupts**; never a mutex; a spinlock
    self-deadlocks.
  - across cores (SMP) → **spinlock + local IRQ disable** (`spin_lock_irqsave`).
  - hardware / DMA co-writer → **locks are useless**; use SET/CLEAR or non-RMW design.
- **Lock granularity** — per-device, not global; independent devices run in parallel.
- **Critical-section scope** — protect the true invariant (e.g. a whole transfer),
  but keep it **as short as possible**. Never hold a spinlock across a wait loop.
- **Reentrancy / thread-safety** — is the API safe to call concurrently? Beware
  non-reentrant libc, shared `errno`, and lifecycle-flag RMW races.

## 3. Interrupts & data movement (performance progression)
- **Polling → interrupt-driven → DMA** — the universal evolution as throughput rises.
  Polling wastes CPU; interrupt-per-byte drowns at high rates; DMA offloads blocks.
- **ISR discipline** — short, non-blocking, no locks held long, no blocking calls;
  defer heavy work to a bottom half (task/workqueue/tasklet/DPC).
- **Buffering** — ring buffers/FIFOs; **SPSC is lock-free** (single owner per index)
  with `volatile` indices + a barrier; double-buffering/ping-pong for continuous DMA.
- **Flow control / backpressure** — define behavior when producer outruns consumer:
  block, drop, or overwrite; handle overrun/overflow errors explicitly.

## 4. DMA & cache coherency
- **Non-coherent DMA** → CPU cache and DMA see different memory. **Clean/flush cache
  before TX** (memory→device), **invalidate cache after RX** (device→memory).
- **Alignment landmine** — cache ops act on whole lines; DMA buffers must be
  **cache-line aligned and size-padded**, or use a **non-cacheable** region.
- **Barriers before "start DMA"** so buffer writes + cache maintenance are visible
  first. `volatile` does nothing for coherency. Coherent interconnect/IOMMU removes
  the manual maintenance.

## 5. Determinism & real-time
- **Interrupt latency & priorities** — worst-case latency, NVIC preemption, priority
  inversion; priority-based masking (BASEPRI vs PRIMASK).
- **WCET thinking** — bound worst-case execution; no unbounded operations on hot paths.
- **Time sources** — tick vs tickless, monotonic vs wall-clock, counter overflow.

## 6. Robustness & fault handling
- **Every wait needs a timeout** — no infinite polls; return a hardware-error code
  on expiry.
- **Error-return contract** — consistent codes; report **partial progress** (bytes
  transferred), not just success/fail; fail-fast vs retry.
- **Mechanism vs policy** — driver moves data and reports what happened; the
  **caller/upper layer owns retry/backoff/abort policy**. A `*_all()` wrapper can
  add "transfer everything" semantics on top.
- **Defensive validation** — validate at API boundaries; for `void *` handles add a
  **magic/type tag** to catch wrong-type handles at runtime.
- **Fault/watchdog** — fault handlers (capture fault status/stack), watchdog kick
  placement (never inside a broken loop), power-loss/brown-out integrity for
  persistent state.

## 7. Lifecycle & state management
- **Standard lifecycle** — init → open → transfer → close → deinit (names vary:
  probe/remove, open/release). Enforce ordering; reject out-of-order calls.
- **Store config in the handle** — enables resume/reconfigure and re-init after
  power loss.
- **Clean teardown** — drain in-flight activity (e.g. "transmitter busy") **before**
  disabling; reset state fully.

## 8. Abstraction & portability
- **Generic ops vtable** (function-pointer table) — decouples upper layers from the
  concrete driver; same interface as implementation evolves polling→DMA. Put the
  vtable in **`const`/read-only** memory (flash, MMU/MPU RO) for RAM savings and
  control-flow-hijack protection.
- **HAL layering** — separate register access, driver logic, and protocol/policy.
  Keep the driver "dumb" (moves bytes/words); protocol meaning lives above.
- **`void *` handle tradeoff** — uniform polymorphism vs lost static type safety
  (unchecked downcast); mitigate with type tags.

## 9. Resource & runtime constraints (embedded-specific)
- **No/'limited dynamic allocation** — prefer static/pool buffers; avoid heap
  fragmentation and non-determinism. Know where every buffer lives at compile time.
- **Stack bounds** — no unbounded recursion; measure high-water mark; guard against
  overflow.
- **Flash/RAM sections** — `.data` copy + `.bss` zero at startup; `const` in flash;
  reason about footprint on constrained parts.

## 10. Power management
- **Clock gating** — disable peripheral clock when idle; **gate only after in-flight
  activity drains**, and **ungate before any register access** (gated register access
  can fault/hang).
- **State save/restore** — deep sleep may drop a power domain; reprogram from stored
  config on resume.
- **Wake sources** — configure wake interrupt; account for first-byte-lost / wake
  latency tradeoffs.

## 11. Hardware literacy
- **Datasheet/reference-manual reading** — register maps, timing diagrams, required
  init sequences and post-enable delays.
- **Errata first** — check silicon errata when hardware "misbehaves."
- **Clock trees & prescaler math** — PLL/prescaler config; integer-division error
  (e.g. baud/divisor rounding) and its tolerance limits.
- **Silicon quirks** — write-buffering, read-back-to-confirm, mandatory dummy reads,
  post-clock-enable settling.

## 12. Verification & observability
- **Testability** — register mocking / simulated register bank; HIL; loopback tests.
- **Fault injection** — force error conditions (overrun, wedged transfer/timeout
  path) that the happy path never exercises.
- **Observability without breaking timing** — no `printf` in ISRs; lightweight trace
  ring buffers, ITM/SWO, timestamped event logs.
- **Debug tooling** — JTAG/SWD, logic analyzer/scope, map files; know what the
  debugger cannot see (optimized-out non-`volatile` accesses).

## 13. Security
- **Read-only vtables / function-pointer tables** — reduce control-flow-hijack surface.
- **Validate off-chip input** — treat data from other bus devices/peripherals as
  untrusted.
- **Secure/verified boot, signed firmware, A/B update with rollback** for updatable
  images.

---

### The senior mindset (summary)
1. **Visibility** → `volatile`. 2. **Ordering** → barriers + memory type.
3. **Mutual exclusion** → the right lock for the right writer (hardware is not a lock
participant). 4. **Determinism** → bounded everything, short ISRs. 5. **Robustness**
→ timeouts, partial-progress contracts, mechanism-vs-policy. 6. **Abstraction** →
stable ops interface as the implementation evolves. Every peripheral re-skins this
same skeleton with a different register map and protocol.
