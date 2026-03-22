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
	@echo "[+] Built patched (full mitigations + -Werror=format-security)"

demo: vulnerable
	@echo ""
	@echo "========================================"
	@echo " Running format string exploit"
	@echo "========================================"
	python3 exploit.py

demo-patched: patched
	@echo ""
	@echo "========================================"
	@echo " Testing format string on PATCHED binary"
	@echo "========================================"
	@echo "[*] %x and %n are treated as literal text in patched binary."
	@python3 -c "
from pwn import *
context.log_level = 'error'
p = process('./patched', stdin=PIPE, stdout=PIPE, stderr=PIPE)
out = b''
while b'Enter log message:' not in out:
    c = p.recv(timeout=1)
    if not c: break
    out += c
p.sendline(b'%x.%x.%x.%n')
result = p.recvall(timeout=2).decode(errors='replace')
p.close()
full = (out + result.encode()).decode(errors='replace')
print('[result]', result.strip())
if 'ADMIN' in result:
    print('[-] UNEXPECTED: exploit succeeded!')
else:
    print('[+] EXPECTED: %%x/%%n printed literally, no memory corruption.')
"

clean:
	rm -f vulnerable patched
