# C Vulnerability Learning Lab

An educational Docker-based lab for learning common C security vulnerabilities hands-on. Each module isolates one vulnerability class, showing:

1. **How it happens** — a realistic vulnerable C program
2. **The exploit** — a working demonstration of the bug's impact
3. **The patch** — a corrected version with explanations of why the fix works

---

## Prerequisites

- [Docker](https://docs.docker.com/get-docker/) 20.10+
- [Docker Compose](https://docs.docker.com/compose/) v2+

---

## Quick Start

```bash
# Build the lab image
make build

# Drop into an interactive shell inside the lab container
make run
```

Inside the container, ASLR is disabled and all tools (gcc, gdb, python3, pwntools, valgrind, checksec) are pre-installed.

---

## Lab Structure

```
vulnerabilities/
├── 01_stack_buffer_overflow/   Stack smashing via gets()
├── 02_heap_buffer_overflow/    Heap corruption via strcpy
├── 03_format_string/           Format string read/write primitives
├── 04_integer_overflow/        Allocation size integer wrap-around
├── 05_use_after_free/          Dangling pointer reuse
├── 06_double_free/             Double free heap corruption
├── 07_null_pointer_deref/      Unchecked NULL dereference
├── 08_off_by_one/              Off-by-one null-byte overflow
├── 09_command_injection/       Unsafe system() call
└── 10_race_condition_toctou/   TOCTOU race in file access checks
```

---

## Working Through a Module

```bash
# Inside the container
cd /lab/vulnerabilities/01_stack_buffer_overflow

# Read the module README
cat README.md

# Build the vulnerable and patched binaries
make

# Inspect protections (vulnerable should have none, patched should have all)
checksec --file=vulnerable
checksec --file=patched

# Run the exploit demonstration
make demo

# See that the patched binary is not exploitable
make demo-patched
```

---

## Compiler Flags

| Binary      | Flags |
|-------------|-------|
| `vulnerable` | `-g -O0 -fno-stack-protector -z execstack -no-pie` |
| `patched`    | `-g -O0 -fstack-protector-all -D_FORTIFY_SOURCE=2 -pie -Wformat -Werror=format-security` |

The vulnerable binary deliberately disables all mitigations for demonstration. **Never compile production code this way.**

---

## Safety Notice

These programs are **intentionally vulnerable**. Always run them inside the provided Docker container. Do not run the vulnerable binaries on production or shared systems.

---

## Vulnerability Index

| # | Vulnerability | CWE |
|---|--------------|-----|
| 01 | Stack Buffer Overflow | CWE-121 |
| 02 | Heap Buffer Overflow | CWE-122 |
| 03 | Format String | CWE-134 |
| 04 | Integer Overflow | CWE-190 |
| 05 | Use-After-Free | CWE-416 |
| 06 | Double Free | CWE-415 |
| 07 | Null Pointer Dereference | CWE-476 |
| 08 | Off-by-One | CWE-193 |
| 09 | Command Injection | CWE-78 |
| 10 | Race Condition (TOCTOU) | CWE-367 |

---

## References

- [OWASP Top Ten](https://owasp.org/www-project-top-ten/)
- [CWE/SANS Top 25](https://cwe.mitre.org/top25/)
- [Pwntools Documentation](https://docs.pwntools.com/)
- [GDB Cheat Sheet](https://darkdust.net/files/GDB%20Cheat%20Sheet.pdf)
