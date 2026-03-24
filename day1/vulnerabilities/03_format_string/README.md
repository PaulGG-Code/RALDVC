# Module 03 — Format String Vulnerability (CWE-134)

## What Is It?

A format string vulnerability occurs when user-supplied input is passed directly as the *format string* argument to `printf`, `sprintf`, `fprintf`, etc. The attacker controls the format specifiers, gaining:

- **Arbitrary memory read** — `%x`, `%s` leak stack and heap contents
- **Arbitrary memory write** — `%n` writes the number of characters printed so far to a pointer argument

## How It Happens

```c
// VULNERABLE:
printf(user_input);         // user_input IS the format string

// SAFE:
printf("%s", user_input);   // user_input is treated as a plain string
```

When `printf(user_input)` runs, it reads format specifiers from `user_input`. But since no arguments were passed, `printf` reads values off the stack as if they were arguments. `%n` writes to whatever pointer value it finds on the stack.

## The Vulnerable Program

`vulnerable.c` is a logging utility. A global variable `int authorized = 0` controls access. The `log_message()` function calls `printf(msg)` directly.

**Phase 1 — Read:** Sending `%x.%x.%x.%x` leaks four 32-bit values from the stack.

**Phase 2 — Write:** Sending a crafted format string with `%n` overwrites `authorized` from 0 to non-zero, granting access without any credential check.

## The Exploit

`exploit.py` demonstrates both phases:
1. Reads `authorized`'s address from the binary (since no-pie, it's fixed)
2. Builds a `%n`-based format string payload to write 1 into `authorized`
3. Calls `log_message()` again — the authorization check now passes

## The Patch

`patched.c` uses `printf("%s", msg)` — the format string is now a hard-coded literal, so user input can only affect the `%s` argument value, never the format itself.

## Key Takeaway

> **Always use a format string literal.** Pass user input only as an argument (`%s`), never as the format string itself. Enable `-Wformat -Werror=format-security` to catch this at compile time.

## Commands

```bash
make              # build both
make demo         # run exploit (leak + write)
make demo-patched # see compiler warnings prevent the bug
```
