# Module 08 — Off-by-One Error (CWE-193)

## What Is It?

An off-by-one error is a logic bug where a loop or array access is one iteration or one index off from the correct value. In security contexts, this most often manifests as writing one byte *past* the end of a buffer — typically a null terminator.

Even writing a single null byte out of bounds can:
- **Overwrite an adjacent variable** — changing a flag or counter
- **Corrupt a saved frame pointer** — enabling stack pivoting
- **Enable further exploitation** — the null byte changes program flow

## How It Happens

The classic off-by-one: using `<=` instead of `<`:

```c
char buf[8];
for (int i = 0; i <= 8; i++) {   // BUG: should be i < 8
    buf[i] = input[i];            // writes buf[8] — one past the end
}
```

Also common in path concatenation:

```c
char dst[128];
size_t n = strlen(dst);
strncat(dst, src, sizeof(dst));   // BUG: doesn't account for existing content
```

## The Vulnerable Program

`vulnerable.c` is a path builder utility that constructs file paths by concatenating a base directory and a filename. The copy loop uses `<= len` instead of `< len`, writing the null terminator one byte past the buffer. The byte immediately after `path[256]` is an `int allow_write` flag — overwriting it with `\0` (zero) clears it, but the specific impact depends on the initial value.

The second example shows a more impactful version where the off-by-one null byte overwrites the LSB of a permission flag stored adjacent to the buffer, changing it from a non-zero "allowed" value to zero "denied" — or vice versa depending on endianness.

## The Exploit

`exploit.py` crafts input that is exactly `BUFFER_SIZE` bytes long (no room for null terminator), triggering the off-by-one and demonstrating the adjacent variable corruption.

## The Patch

`patched.c` corrects `<= len` to `< len` and uses `strnlen` / `snprintf` patterns that inherently avoid the fence-post error.

## Key Takeaway

> **Off-by-one is the most common "trivial" bug with non-trivial consequences.** Always use `< N` not `<= N` for array bounds. Prefer `snprintf`, `strlcpy`, and `strlcat` over manual loops. Fuzz with `ASAN` to catch these.

## Commands

```bash
make        # build
make demo   # trigger off-by-one
```
