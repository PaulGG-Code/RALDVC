# Module 02 — Heap Buffer Overflow (CWE-122)

## What Is It?

A heap buffer overflow is a buffer overflow that occurs in heap-allocated memory (via `malloc`, `calloc`, etc.). Unlike stack overflows, there is no return address to overwrite directly. Instead, attackers corrupt:

- **Adjacent heap objects** — overwriting fields in nearby allocations
- **Heap metadata** — corrupting glibc's allocator internals to redirect writes
- **Function pointers stored on the heap** — hijacking control flow

## How It Happens

```
Heap layout after two malloc() calls:
  [chunk header][note_buf (32 bytes)][chunk header][note_cb struct]
                 ^-- strcpy overflows into -->^
```

When `strcpy(note_buf, user_input)` is called with input longer than 32 bytes, it walks through the heap chunk boundary and corrupts the adjacent `note_cb` structure — which happens to contain a function pointer.

## The Vulnerable Program

`vulnerable.c` is a simple note-taking application. It allocates:
1. A `char note[32]` buffer for the note content
2. A `NoteCallback` struct containing a function pointer (`on_save`)

The `save_note()` function copies user input with `strcpy()` (no length check). If the note content exceeds 32 bytes, it overflows into the callback struct and overwrites `on_save`.

## The Exploit

`exploit.py` crafts an input that:
1. Fills the 32-byte note buffer with padding
2. Overwrites heap chunk metadata (8 bytes)
3. Overwrites the `on_save` function pointer with the address of `secret_admin_fn`

Result: calling `save_note()` invokes the admin function instead of the normal save.

## The Patch

`patched.c` uses `strncpy(note_buf, input, NOTE_SIZE - 1)` with proper null termination, and adds an explicit length validation before copying.

## Key Takeaway

> **Never use `strcpy()` into a heap buffer without knowing the source length.** Use `strncpy()`, `strlcpy()`, or `snprintf()` with explicit size limits. Validate input length against buffer capacity before any copy.

## Commands

```bash
make              # build vulnerable + patched
make demo         # run exploit
make demo-patched # attempt exploit against patched
valgrind --tool=memcheck ./vulnerable  # see heap errors
```
