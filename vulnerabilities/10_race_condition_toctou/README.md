# Module 10 — Race Condition / TOCTOU (CWE-367)

## What Is It?

TOCTOU stands for **Time-of-Check to Time-of-Use**. It is a race condition vulnerability where the state of a resource is checked at one point in time, but used at a different (later) point, with the possibility that the state changes between the check and the use.

In file access security:
```
access(path, R_OK)   ← TIME OF CHECK (path → /home/user/safe_file)
[attacker swaps symlink here]
open(path, O_RDONLY) ← TIME OF USE   (path → /etc/shadow!)
```

This race is critical in **setuid programs** where the program runs as root but checks if the *calling user* has access. An attacker can win the race to make the program open a privileged file on their behalf.

## How It Happens

The OS provides no atomicity guarantee between `access()` and `open()`. Between the two syscalls, the kernel can be preempted, allowing another thread/process to rename the file, swap a symlink, or otherwise change what `path` resolves to.

## The Vulnerable Program

`vulnerable.c` is a privileged file reader (simulates a setuid utility). It:
1. Calls `access(path, R_OK)` to check if the calling user can read the file
2. Calls `open(path, O_RDONLY)` to actually open it
3. Reads and prints the file content

The gap between steps 1 and 2 is the TOCTOU window.

## The Exploit

`exploit.sh` is a race script that:
1. Creates a readable decoy file (`/tmp/safe_file`)
2. Creates a sensitive target (`/tmp/secret` with restricted permissions, simulating `/etc/shadow`)
3. Rapidly alternates the symlink between decoy and target while running the vulnerable program
4. Wins the race: access() sees the decoy, open() sees the secret

## The Patch

`patched.c` uses `O_NOFOLLOW` to refuse symlinks, and opens the file first (getting an fd), then uses `fstat()` to check permissions on the already-opened fd — eliminating the TOCTOU gap entirely.

## Key Takeaway

> **Never use `access()` followed by `open()`**. Check-then-act patterns are inherently racy. Instead: open the file first (getting an fd), then check permissions on the fd using `fstat()`. Use `O_NOFOLLOW` to prevent symlink attacks.

## Commands

```bash
make           # build
make demo      # run race condition exploit script
./exploit.sh   # run the race manually
```
