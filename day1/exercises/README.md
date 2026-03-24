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

| ID | Topic | CWE | Key Tool | Difficulty |
|----|-------|-----|----------|------------|
| [A](exoA/README.md) | Stack overflow → logic corruption | CWE-121 | ASan + GDB | Débutant |
| [B](exoB/README.md) | Cumulative overflow via strcat | CWE-121 | ASan | Débutant |
| [C](exoC/README.md) | Off-by-one | CWE-193 | ASan | Débutant |
| [D](exoD/README.md) | Integer overflow in allocation size | CWE-190 | UBSan / ASan | Intermédiaire |
| [E](exoE/README.md) | Use-after-free | CWE-416 | ASan | Intermédiaire |
| [F](exoF/README.md) | Format string misuse | CWE-134 | GCC -Wformat | Intermédiaire |
| [G](exoG/README.md) | Uninitialized memory read | CWE-457 | Valgrind | Intermédiaire |
| [H](exoH/README.md) | Unsafe API migration | CWE-676 | ASan | Intermédiaire |
| [I](exoI/README.md) | Double free | CWE-415 | ASan | Intermédiaire |
| [J](exoJ/README.md) | Command injection | CWE-78 | Manuel | Intermédiaire |

## Recommended Order

A → C → B → F → D → E → G → H → I → J

Start with A (most visible impact), then build up to subtler bugs.
I and J can be done at any point after E.

## Quick Build All

```bash
make -C /lab/exercises all
```

## Run Automated Checks

Each exercise includes a `make check` target that verifies:
1. The vulnerable version is caught by the relevant tool (ASan / Valgrind / compiler)
2. The fixed version passes cleanly

```bash
# Check a single exercise
make -C /lab/exercises/exoA check

# Check all exercises at once
make -C /lab/exercises check
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

# Run automated pass/fail verification
make check
```
