# Module 04 — Integer Overflow (CWE-190)

## What Is It?

An integer overflow occurs when an arithmetic operation produces a value too large (or too small) to be represented in the integer type used. In C, unsigned integer overflow wraps around modulo 2^N; signed overflow is undefined behavior.

In security contexts, integer overflows most commonly lead to:
- **Under-allocation** — `malloc(size)` allocates far less memory than expected
- **Buffer overflow** — subsequent writes exceed the tiny allocation
- **Logic bypass** — length checks compare against a wrapped value

## How It Happens

```c
uint32_t count = user_input;          // attacker controls this
size_t alloc   = count * sizeof(Ticket);  // wraps if count is large enough
void  *buf     = malloc(alloc);           // allocates almost nothing
memcpy(buf, data, count * sizeof(Ticket)); // writes huge amount — heap overflow
```

With `sizeof(Ticket) == 8` and `count == 0x20000001`, the multiplication is:
`0x20000001 * 8 = 0x100000008` which truncates to `0x8` in a 32-bit multiplication, so only 8 bytes are allocated — but the copy writes gigabytes.

## The Vulnerable Program

`vulnerable.c` is a ticket reservation system. The user specifies how many tickets to reserve. The allocation size is computed as `count * sizeof(Ticket)` using a `uint32_t` count. With a large enough count, the multiplication wraps, `malloc()` gets a tiny size, and the subsequent data initialization writes far past the allocation.

## The Exploit

`exploit.py` sends `count = 0x20000001` (536870913). This causes `count * 8` to overflow to `0x8` — only 8 bytes allocated — but then the program tries to initialize 4GB worth of tickets, crashing or corrupting the heap.

## The Patch

`patched.c` uses `__builtin_mul_overflow()` to detect the overflow before allocation, and additionally checks that the allocation size is within a sane upper bound.

## Key Takeaway

> **Validate integer arithmetic before using results for memory allocation.** Use `__builtin_mul_overflow`, `__builtin_add_overflow`, or explicit pre-multiply bounds checks. Never assume that `count * sizeof(T)` is safe without validation.

## Commands

```bash
make        # build
make demo   # trigger integer overflow → heap corruption / crash
```
