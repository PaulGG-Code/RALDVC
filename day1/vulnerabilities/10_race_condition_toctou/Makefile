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

# Make exploit.sh executable
demo: vulnerable patched exploit.sh
	@chmod +x exploit.sh
	@echo ""
	@echo "========================================"
	@echo " Running TOCTOU race exploit"
	@echo "========================================"
	@echo "[*] Option A: Python exploit (automated)"
	python3 exploit.py
	@echo ""
	@echo "[*] Option B: Shell exploit (run manually)"
	@echo "    chmod +x exploit.sh && ./exploit.sh"

demo-shell: vulnerable exploit.sh
	chmod +x exploit.sh
	./exploit.sh

demo-patched: patched
	@echo "[*] Testing O_NOFOLLOW: creating symlink to /etc/hostname..."
	@ln -sfn /etc/hostname /tmp/test_link
	./patched /tmp/test_link || true
	@rm -f /tmp/test_link

clean:
	rm -f vulnerable patched
