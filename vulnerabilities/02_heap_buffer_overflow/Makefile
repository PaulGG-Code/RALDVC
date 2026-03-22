CC          = gcc
VULN_FLAGS  = -g -O0 -fno-stack-protector -z execstack -no-pie -w
PATCH_FLAGS = -g -O0 -fstack-protector-all -D_FORTIFY_SOURCE=2 -pie \
              -Wformat -Werror=format-security

.PHONY: all vulnerable patched demo demo-patched clean

all: vulnerable patched

vulnerable: vulnerable.c
	$(CC) $(VULN_FLAGS) -o $@ $<
	@echo "[+] Built vulnerable (no mitigations)"

patched: patched.c
	$(CC) $(PATCH_FLAGS) -o $@ $<
	@echo "[+] Built patched (full mitigations)"

demo: vulnerable
	@echo ""
	@echo "========================================"
	@echo " Running exploit against VULNERABLE binary"
	@echo "========================================"
	python3 exploit.py

demo-patched: patched
	@echo ""
	@echo "========================================"
	@echo " Attempting exploit against PATCHED binary"
	@echo "========================================"
	@echo "[*] Patched binary validates input length. Overflow rejected."
	@python3 -c "
from pwn import *
import os
context.log_level = 'error'
p = process('./patched', stdin=PIPE, stdout=PIPE, stderr=PIPE)
out_before = b''
while b'Enter note' not in out_before:
    c = p.recv(timeout=1)
    if not c: break
    out_before += c
p.sendline(b'A' * 200)
out = p.recvall(timeout=2).decode(errors='replace')
p.close()
print('[result]', out.strip())
if 'SECRET' in out:
    print('[-] UNEXPECTED: exploit succeeded on patched binary!')
else:
    print('[+] EXPECTED: patched binary correctly rejected oversized input.')
"

clean:
	rm -f vulnerable patched
