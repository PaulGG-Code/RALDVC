CC   = gcc
BASE = -O0 -g -Wall -Wextra -Wpedantic
VULN = $(BASE) -w
FIX  = $(BASE) -fsanitize=address
ASAN = $(BASE) -w -fsanitize=address

.PHONY: all vuln fix asan demo check clean

all: vuln fix asan

vuln: exoI_vuln.c
	$(CC) $(VULN) $< -o exoI_vuln

fix: exoI_fix.c
	$(CC) $(FIX) $< -o exoI_fix

asan: exoI_vuln.c
	$(CC) $(ASAN) $< -o exoI_vuln_asan

demo: all
	@echo ""
	@echo "── Exécution vulnérable (crash ou corruption silencieuse) ──"
	./exoI_vuln 2>&1 || true
	@echo ""
	@echo "── Détection ASan (double-free) ──"
	./exoI_vuln_asan 2>&1 | head -25 || true
	@echo ""
	@echo "── Version corrigée (double-libération inoffensive) ──"
	./exoI_fix

check: all
	@echo "=== Vérification automatique (exoI) ==="
	@echo "─ Test 1 : ASan doit détecter la double-libération ─"
	@./exoI_vuln_asan 2>&1 | grep -qi "AddressSanitizer\|double.free" \
		&& echo "[PASS] ASan a détecté la double-libération." \
		|| (echo "[FAIL] ASan n'a pas signalé d'erreur." && exit 1)
	@echo "─ Test 2 : version corrigée doit se terminer sans erreur ASan ─"
	@./exoI_fix \
		&& echo "[PASS] Version corrigée : terminaison propre." \
		|| (echo "[FAIL] Version corrigée : code de sortie non-nul." && exit 1)
	@echo "[✓] Tous les tests passent."

clean:
	rm -f exoI_vuln exoI_fix exoI_vuln_asan
