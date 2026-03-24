CC          = gcc
VULN_FLAGS  = -g -O0 -fno-stack-protector -z execstack -no-pie -w
PATCH_FLAGS = -g -O0 -fstack-protector-all -D_FORTIFY_SOURCE=2 -pie
ASAN_FLAGS  = -g -O0 -fsanitize=address -fno-omit-frame-pointer -w

.PHONY: all vulnerable patched demo demo-patched asan clean

all: vulnerable patched

vulnerable: vulnerable.c
	$(CC) $(VULN_FLAGS) -o $@ $<
	@echo "[+] Built vulnerable"

patched: patched.c
	$(CC) $(PATCH_FLAGS) -o $@ $<
	@echo "[+] Built patched"

asan: vulnerable.c
	$(CC) $(ASAN_FLAGS) -o vulnerable_asan $<
	@echo "[+] Built vulnerable_asan (AddressSanitizer enabled)"
	@echo "[*] Run: ./vulnerable_asan to see ASAN catch the UAF"

demo: vulnerable
	@echo ""
	@echo "========================================"
	@echo " Running UAF demonstration"
	@echo "========================================"
	python3 exploit.py

demo-patched: patched
	@echo ""
	@echo "========================================"
	@echo " Running patched version"
	@echo "========================================"
	./patched

clean:
	rm -f vulnerable patched vulnerable_asan
