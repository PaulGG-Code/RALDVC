# Module 01 — Stack Buffer Overflow (CWE-121)

## What Is It?

A stack buffer overflow occurs when data written to a fixed-size stack buffer exceeds its allocated size, overwriting adjacent memory on the stack. This adjacent memory includes saved registers, the return address, and local variables of the caller.

When an attacker controls what gets written past the buffer's end, they can:
- **Overwrite the return address** — redirecting execution to attacker-controlled code
- **Bypass authentication checks** — overwriting local flag variables
- **Crash the program** — denial of service

## How It Happens

```
Stack layout (grows downward):
  [ buf[64]           ]  <- user input goes here
  [ saved_rbp         ]  <- base pointer of calling frame
  [ return address    ]  <- instruction to jump to when function returns
```

If `gets(buf)` reads more than 64 bytes, the overflow walks through saved_rbp and overwrites the return address. On `ret`, the CPU jumps to whatever value was placed there.

## The Vulnerable Program

`vulnerable.c` simulates a simple login system. It calls `gets()` to read a username — a function so dangerous it was **removed from the C11 standard** (ISO/IEC 9899:2011). The `authenticated` flag lives adjacent to the buffer; overflowing the buffer flips it without knowing the password.

## The Exploit

`exploit.py` uses **pwntools** to:
1. Send 80 bytes of padding to overflow `buf[64]` and the `authenticated` flag
2. The program prints "Access granted!" without the correct password

## The Patch

`patched.c` replaces `gets()` with `fgets(buf, sizeof(buf), stdin)`:
- `fgets` takes a **size limit** as its second argument — it never reads more than `sizeof(buf) - 1` bytes
- Combined with stack canary (`-fstack-protector-all`), even if we overflow, the canary detects it and aborts

## Key Takeaway

> **Never use `gets()`.** Use `fgets()`, `getline()`, or `scanf("%63s", buf)`. Always bound-check input before copying into a fixed-size buffer.

## Commands

```bash
make              # build vulnerable + patched
make demo         # run exploit against vulnerable
make demo-patched # attempt exploit against patched (should fail)
checksec --file=vulnerable
checksec --file=patched
gdb ./vulnerable  # inspect interactively
```
