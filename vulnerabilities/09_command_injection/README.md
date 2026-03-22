# Module 09 — Command Injection (CWE-78)

## What Is It?

Command injection occurs when user-supplied data is incorporated into a shell command without sanitization. The attacker injects shell metacharacters (`;`, `&&`, `|`, `` ` ``, `$()`) to execute arbitrary commands.

This is often called **OS command injection** and is one of the most directly impactful vulnerabilities — an attacker can execute any command with the privileges of the running process.

## How It Happens

```c
char cmd[256];
snprintf(cmd, sizeof(cmd), "tar -czf backup.tar.gz %s", user_filename);
system(cmd);   // if user_filename = "a.txt; rm -rf /"  → catastrophe
```

`system(cmd)` passes `cmd` to `/bin/sh -c`, which interprets all shell metacharacters. Any unsanitized user input in `cmd` becomes shell code.

## The Vulnerable Program

`vulnerable.c` is a file backup utility. It takes a filename from the user and constructs a `tar` command using `snprintf` + `system()`. There is no validation of the filename — the user can inject arbitrary shell commands.

## The Exploit

`exploit.py` sends a filename of the form `file.txt; id; whoami` which causes the shell to:
1. Run `tar -czf backup.tar.gz file.txt`
2. Run `id`
3. Run `whoami`

This demonstrates arbitrary command execution.

## The Patch

`patched.c` replaces `system()` with `execve()` using an explicit argument array. `execve()` does **not** invoke a shell — it runs the binary directly with exact arguments. No shell metacharacter interpretation occurs.

## Key Takeaway

> **Avoid `system()`, `popen()`, and `exec[lv]p()` with user input.** Use `execve()` with explicit argument arrays — no shell, no metacharacter interpretation. If `system()` is unavoidable, strictly whitelist the input characters (only `[a-zA-Z0-9._-]` for filenames).

## Commands

```bash
make        # build
make demo   # trigger command injection: id, whoami, date
```
