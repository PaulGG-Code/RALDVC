CC   = gcc
BASE = -O0 -g -Wall -Wextra -Wpedantic
VULN = $(BASE) -w
FIX  = $(BASE) -fsanitize=address
ASAN = $(BASE) -w -fsanitize=address

.PHONY: all vuln fix asan demo clean

all: vuln fix asan

vuln: exoE_vuln.c
	$(CC) $(VULN) $< -o exoE_vuln

fix: exoE_fix.c
	$(CC) $(FIX) $< -o exoE_fix

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
	@echo ""
	@echo "── Version corrigée ──"
	./exoE_fix

clean:
	rm -f exoE_vuln exoE_fix exoE_vuln_asan
