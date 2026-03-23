CC   = gcc
BASE = -O0 -g -Wall -Wextra -Wpedantic
VULN = $(BASE) -w
ASAN = $(BASE) -w -fsanitize=address

.PHONY: all vuln asan demo clean

all: vuln asan

vuln: exoE_vuln.c
	$(CC) $(VULN) $< -o exoE_vuln

asan: exoE_vuln.c
	$(CC) $(ASAN) $< -o exoE_vuln_asan

demo: all
	@echo ""
	@echo "── Use-after-free (exécution 1) ──"
	./exoE_vuln || true
	@echo ""
	@echo "── Use-after-free (exécution 2 — valeur peut différer) ──"
	./exoE_vuln || true
	@echo ""
	@echo "── Détection ASan ──"
	./exoE_vuln_asan 2>&1 | head -25 || true

clean:
	rm -f exoE_vuln exoE_vuln_asan exoE_fix
