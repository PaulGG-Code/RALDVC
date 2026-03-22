CC   = gcc
BASE = -O0 -g -Wall -Wextra -Wpedantic
VULN = $(BASE) -w
FIX  = $(BASE) -fsanitize=address,undefined
ASAN = $(BASE) -w -fsanitize=address

.PHONY: all vuln fix asan demo check clean

all: vuln fix asan

vuln: exoC_vuln.c
	$(CC) $(VULN) $< -o exoC_vuln

fix: exoC_fix.c
	$(CC) $(FIX) $< -o exoC_fix

asan: exoC_vuln.c
	$(CC) $(ASAN) $< -o exoC_vuln_asan

demo: all
	@echo ""
	@echo "── Comportement vulnérable (peut sembler OK) ──"
	./exoC_vuln || true
	@echo ""
	@echo "── Détection ASan ──"
	./exoC_vuln_asan 2>&1 | head -20 || true
	@echo ""
	@echo "── Version corrigée ──"
	./exoC_fix

check: all
	@echo "=== Vérification automatique (exoC) ==="
	@echo "─ Test 1 : ASan doit détecter l'accès hors-bornes ─"
	@./exoC_vuln_asan 2>&1 | grep -qi "AddressSanitizer" \
		&& echo "[PASS] ASan a détecté le off-by-one." \
		|| (echo "[FAIL] ASan n'a pas signalé d'erreur." && exit 1)
	@echo "─ Test 2 : version corrigée doit se terminer sans erreur ─"
	@./exoC_fix \
		&& echo "[PASS] Version corrigée : terminaison propre." \
		|| (echo "[FAIL] Version corrigée : code de sortie non-nul." && exit 1)
	@echo "[✓] Tous les tests passent."

clean:
	rm -f exoC_vuln exoC_fix exoC_vuln_asan
