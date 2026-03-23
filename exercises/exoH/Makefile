CC   = gcc
BASE = -O0 -g -Wall -Wextra -Wpedantic
VULN = $(BASE) -w
ASAN = $(BASE) -w -fsanitize=address

SHORT_U = ALICE
SHORT_M = hello
LONG_U  = AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
LONG_M  = BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB

.PHONY: all vuln asan demo clean

all: vuln asan

vuln: exoH_vuln.c
	$(CC) $(VULN) $< -o exoH_vuln

asan: exoH_vuln.c
	$(CC) $(ASAN) $< -o exoH_vuln_asan

demo: all
	@echo ""
	@echo "── Cas nominal (vulnérable) ──"
	./exoH_vuln "$(SHORT_U)" "$(SHORT_M)"
	@echo ""
	@echo "── Overflow cumulatif (vulnérable) ──"
	./exoH_vuln "$(LONG_U)" "$(LONG_M)" || true
	@echo ""
	@echo "── Détection ASan ──"
	./exoH_vuln_asan "$(LONG_U)" "$(LONG_M)" 2>&1 | head -20 || true

clean:
	rm -f exoH_vuln exoH_vuln_asan exoH_fix
