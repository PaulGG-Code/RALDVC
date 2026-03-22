# Module 05 — Use-After-Free (CWE-416)

## What Is It?

Use-After-Free (UAF) occurs when a program continues to use a pointer after the memory it points to has been freed. This is dangerous because:

1. The freed memory may be **reallocated** for a different purpose
2. Writing through the old pointer corrupts the new object's data
3. Reading through the old pointer leaks sensitive data from the new object
4. If the new object contains a function pointer, UAF can **hijack control flow**

## How It Happens

```c
Session *s = malloc(sizeof(Session));
s->uid = 1000;
free(s);
// ... allocator reallocates s's memory for something else ...
s->uid = 0;   // UAF write — corrupts the new object
```

The key insight: `free()` does NOT zero memory or invalidate the pointer. The pointer value remains valid-looking but the memory it points to is now managed by the allocator.

## The Vulnerable Program

`vulnerable.c` is a user session manager. It:
1. Allocates a `Session` struct and logs the user in
2. Calls `logout()` which frees the Session — but does NOT null the pointer
3. Allocates an `AdminToken` struct (same size — allocator reuses the freed block)
4. The old session pointer now overlaps with the AdminToken
5. Accessing the old `session->is_admin` field reads/writes the AdminToken's first field (`privilege_level`)

## The Exploit

`exploit.py` demonstrates that accessing session data after logout actually reads/writes the AdminToken's fields — type confusion via UAF.

## The Patch

`patched.c` sets `session = NULL` immediately after `free(session)`. Any subsequent dereference of a NULL pointer causes an immediate, detectable crash (SIGSEGV) rather than silent memory corruption.

## Key Takeaway

> **Always set pointers to NULL after freeing.** Use smart pointer patterns (reference counting, arena allocation) when ownership is complex. Tools: `valgrind --tool=memcheck`, AddressSanitizer (`-fsanitize=address`).

## Commands

```bash
make                          # build both
make demo                     # UAF demonstration
valgrind --tool=memcheck ./vulnerable  # detect UAF with valgrind
gcc -fsanitize=address -g -O0 vulnerable.c -o vulnerable_asan && ./vulnerable_asan
```
