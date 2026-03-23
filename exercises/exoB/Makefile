CC   = gcc
BASE = -O0 -g -Wall -Wextra -Wpedantic
VULN = $(BASE) -w
ASAN = $(BASE) -w -fsanitize=address

SHORT_U = Alice
SHORT_M = Hi
LONG_U  = AAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
LONG_M  = BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB

.PHONY: all vuln asan demo clean

all: vuln asan

vuln: exoB_vuln.c
	$(CC) $(VULN) $< -o exoB_vuln

asan: exoB_vuln.c
	$(CC) $(ASAN) $< -o exoB_vuln_asan

demo: all
	@echo ""
	@echo "── Cas nominal ──"
	./exoB_vuln "$(SHORT_U)" "$(SHORT_M)"
	@echo ""
	@echo "── Overflow cumulatif (vulnérable) ──"
	./exoB_vuln "$(LONG_U)" "$(LONG_M)" || true
	@echo ""
	@echo "── Détection ASan ──"
	./exoB_vuln_asan "$(LONG_U)" "$(LONG_M)" 2>&1 | head -20 || true

clean:
	rm -f exoB_vuln exoB_vuln_asan exoB_fix
