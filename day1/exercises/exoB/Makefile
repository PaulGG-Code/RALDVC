CC   = gcc
BASE = -O0 -g -Wall -Wextra -Wpedantic
VULN = $(BASE) -w
FIX  = $(BASE) -fsanitize=address,undefined
ASAN = $(BASE) -w -fsanitize=address

SHORT_U = Alice
SHORT_M = Hi
LONG_U  = AAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
LONG_M  = BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB

.PHONY: all vuln fix asan demo check clean

all: vuln fix asan

vuln: exoB_vuln.c
	$(CC) $(VULN) $< -o exoB_vuln

fix: exoB_fix.c
	$(CC) $(FIX) $< -o exoB_fix

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
	@echo ""
	@echo "── Version corrigée ──"
	./exoB_fix "$(SHORT_U)" "$(SHORT_M)"
	./exoB_fix "$(LONG_U)" "$(LONG_M)"

check: all
	@echo "=== Vérification automatique (exoB) ==="
	@echo "─ Test 1 : ASan doit détecter le débordement cumulatif ─"
	@./exoB_vuln_asan "$(LONG_U)" "$(LONG_M)" 2>&1 | grep -qi "AddressSanitizer" \
		&& echo "[PASS] ASan a détecté le débordement." \
		|| (echo "[FAIL] ASan n'a pas signalé d'erreur." && exit 1)
	@echo "─ Test 2 : version corrigée doit se terminer sans erreur ─"
	@./exoB_fix "$(LONG_U)" "$(LONG_M)" \
		&& echo "[PASS] Version corrigée : terminaison propre." \
		|| (echo "[FAIL] Version corrigée : code de sortie non-nul." && exit 1)
	@echo "[✓] Tous les tests passent."

clean:
	rm -f exoB_vuln exoB_fix exoB_vuln_asan
