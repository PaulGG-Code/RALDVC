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

demo: vulnerable
	@echo ""
	@echo "========================================"
	@echo " Running integer overflow exploit"
	@echo "========================================"
	python3 exploit.py

demo-patched: patched
	@echo "See exploit.py — it also tests the patched binary in Step 3."
	python3 exploit.py

clean:
	rm -f vulnerable patched
