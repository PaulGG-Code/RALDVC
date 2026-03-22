#!/bin/bash
# Entrypoint for the C Vulnerability Learning Lab.
#
# Tries to disable ASLR (Address Space Layout Randomization) so that
# exploit addresses are reproducible across runs. This succeeds on Linux
# hosts with sufficient privileges. On Mac and Windows (Docker Desktop),
# the kernel write may be denied — we warn and continue rather than abort.

if echo 0 > /proc/sys/kernel/randomize_va_space 2>/dev/null; then
    echo "[*] ASLR disabled (kernel.randomize_va_space = 0)"
else
    echo "[!] ASLR could not be disabled on this host."
    echo "    Exploit addresses in vulnerability modules may vary between runs."
    echo "    To disable ASLR for a single binary, prefix with:"
    echo "      setarch x86_64 -R ./vulnerable"
    echo ""
fi

exec "$@"
