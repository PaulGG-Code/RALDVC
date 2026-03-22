# Exercises — Step-by-Step C Vulnerability Walkthroughs

These exercises complement the `/lab/vulnerabilities/` modules. Where the modules focus on end-to-end exploitation, these exercises focus on **hands-on observation**: compile, run, inspect with tools, apply the fix, verify.

Each exercise follows the same structure:
1. **Pedagogical intent** — what concept this teaches
2. **Vulnerable code** — minimal, isolated reproduction
3. **Step-by-step exploitation** — exact commands to run
4. **Tooled observation** — ASan, UBSan, Valgrind, or GDB
5. **Correction** — minimal safe fix
6. **Defensive verification** — confirm the fix removes the issue

## Exercise Index

| ID | Topic | CWE | Key Tool |
|----|-------|-----|----------|
| [A](exoA/README.md) | Stack overflow → logic corruption | CWE-121 | ASan + GDB |
| [B](exoB/README.md) | Cumulative overflow via strcat | CWE-121 | ASan |
| [C](exoC/README.md) | Off-by-one | CWE-193 | ASan |
| [D](exoD/README.md) | Integer overflow in allocation size | CWE-190 | UBSan |
| [E](exoE/README.md) | Use-after-free | CWE-416 | ASan |
| [F](exoF/README.md) | Format string misuse | CWE-134 | GCC -Wformat |
| [G](exoG/README.md) | Uninitialized memory read | CWE-457 | Valgrind |
| [H](exoH/README.md) | Unsafe API migration | CWE-676 | ASan |

## Recommended Order

A → C → B → F → D → E → G → H

Start with A (most visible impact), then build up to subtler bugs.

## Quick Build All

```bash
make -C /lab/exercises all
```

## Inside Each Exercise

```bash
cd /lab/exercises/exoA

# Build vulnerable version
make vuln

# Build with AddressSanitizer
make asan

# Build fixed version
make fix

# Run all steps of the exercise
make demo
```
