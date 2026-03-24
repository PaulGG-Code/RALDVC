# Module 06 — Double Free (CWE-415)

## What Is It?

A double free occurs when `free()` is called twice on the same pointer. This corrupts the heap allocator's internal metadata (free lists), potentially allowing:

- **Arbitrary write** — corrupting allocator metadata to write to attacker-controlled addresses on the next `malloc()`
- **Crash (SIGSEGV)** — detectable but not exploitable in hardened systems
- **Heap information leak** — in some glibc versions, double-free corrupts libc pointers visible to the attacker

Modern glibc (2.29+) has partial mitigations but double-free remains a critical vulnerability class, heavily exploited in browser and kernel exploits.

## How It Happens

The most common cause is **multiple code paths** that both free the same resource:

```c
void process(Data *d) {
    if (error_condition) {
        free(d);   // first free on error path
        return;
    }
    use(d);
    free(d);       // second free on normal exit path
}
```

Also common: `goto` error handling that frees already-freed resources.

## The Vulnerable Program

`vulnerable.c` is an error-handling routine for a data processing pipeline. It uses `goto` for cleanup — a pattern that often introduces double-free bugs when intermediate steps fail. The same `buffer` pointer gets freed twice when a certain error path is taken.

## The Exploit

`exploit.py` triggers the error path to cause a double-free. On older glibc, this corrupts the free list chunk headers, which can be observed with valgrind. On modern glibc, it triggers `malloc(): unaligned tcache chunk detected` or a similar abort.

## The Patch

`patched.c` uses the `safe_free()` idiom (set to NULL after free) and restructures cleanup to avoid double-free regardless of error path.

## Key Takeaway

> **Null the pointer after every `free()`**. If a pointer might be freed in multiple places, use a `safe_free()` macro. Tools: valgrind, AddressSanitizer. Note that `free(NULL)` is a no-op and always safe.

## Commands

```bash
make              # build
make demo         # trigger double-free
valgrind --tool=memcheck ./vulnerable   # heap error detection
```
