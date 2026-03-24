CC          = gcc
VULN_FLAGS  = -g -O0 -fno-stack-protector -z execstack -no-pie -w
PATCH_FLAGS = -g -O0 -fstack-protector-all -D_FORTIFY_SOURCE=2 -pie

.PHONY: all vulnerable patched demo demo-patched clean

all: vulnerable patched

vulnerable: vulnerable.c
	$(CC) $(VULN_FLAGS) -o $@ $<
	@echo "[+] Built vulnerable"

patched: patched.c
	$(CC) $(PATCH_FLAGS) -o $@ $<
	@echo "[+] Built patched"

demo: vulnerable patched
	@echo ""
	@echo "========================================"
	@echo " Running command injection exploit"
	@echo "========================================"
	python3 exploit.py

demo-patched: patched
	@echo "[*] Enter: a.txt; id"
	./patched

clean:
	rm -f vulnerable patched
