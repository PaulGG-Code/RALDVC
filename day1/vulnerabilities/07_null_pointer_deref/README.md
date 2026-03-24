# Module 07 — Null Pointer Dereference (CWE-476)

## What Is It?

A null pointer dereference occurs when a program attempts to read from or write to memory at address 0x0 (NULL). In most operating systems, address 0 is not mapped, so this causes an immediate SIGSEGV crash.

While primarily a reliability issue, null pointer dereferences can become security vulnerabilities when:
- **NULL is not zero on the target platform** (embedded systems)
- **mmap(0, ...) trick** — historically, on Linux without address space restrictions, an attacker could map page 0 and place shellcode there, then trigger the null dereference to execute it
- **Logic bypasses** — when a NULL check that should reject is missing, a NULL value flows into a permission check that evaluates to "granted"

## How It Happens

```c
User *u = find_user(username);  // returns NULL if not found
printf("Logged in as: %s\n", u->name);  // crash if u is NULL
```

Common causes:
- Missing NULL check on `malloc()` return
- Missing NULL check on `find_*()` / `get_*()` return values
- Early return paths that don't set a pointer before it's used

## The Vulnerable Program

`vulnerable.c` is a configuration parser. `get_config_value()` returns NULL when a key is not found. The caller dereferences the return value without checking for NULL.

## The Exploit

`exploit.py` passes an unknown configuration key, causing a NULL return that flows into a dereference — SIGSEGV.

## The Patch

`patched.c` checks every pointer return value before dereferencing. Uses `assert()` in development mode and explicit error returns in production code.

## Key Takeaway

> **Always check pointer return values before dereferencing.** Functions that can fail should return NULL on failure; callers must handle it. Use `-fsanitize=null` or `-fsanitize=address` to detect this class of bug.

## Commands

```bash
make        # build
make demo   # trigger null deref crash
```
