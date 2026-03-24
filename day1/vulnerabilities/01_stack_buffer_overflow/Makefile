CC      = gcc
# Vulnerable: no mitigations, stack is executable, no PIE
VULN_FLAGS  = -g -O0 -fno-stack-protector -z execstack -no-pie -w
# Patched: full mitigations enabled
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
	@echo "[*] Note: fgets() limits input to sizeof(buf). Overflow impossible."
	python3 demo_patched.py

clean:
	rm -f vulnerable patched
