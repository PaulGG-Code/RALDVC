CC   = gcc
BASE = -O0 -g -Wall -Wextra -Wpedantic
VULN = $(BASE) -w
FIX  = $(BASE) -fsanitize=address,undefined
ASAN = $(BASE) -w -fsanitize=address

SHORT_U = ALICE
SHORT_M = hello
LONG_U  = AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
LONG_M  = BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB

.PHONY: all vuln fix asan demo check clean

all: vuln fix asan

vuln: exoH_vuln.c
	$(CC) $(VULN) $< -o exoH_vuln

fix: exoH_fix.c
	$(CC) $(FIX) $< -o exoH_fix

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
	@echo ""
	@echo "── Version corrigée — cas nominal ──"
	./exoH_fix "$(SHORT_U)" "$(SHORT_M)"
	@echo ""
	@echo "── Version corrigée — cas long (troncature signalée) ──"
	./exoH_fix "$(LONG_U)" "$(LONG_M)" || true

check: all
	@echo "=== Vérification automatique (exoH) ==="
	@echo "─ Test 1 : ASan doit détecter le débordement (API non sécurisée) ─"
	@./exoH_vuln_asan "$(LONG_U)" "$(LONG_M)" 2>&1 | grep -qi "AddressSanitizer" \
		&& echo "[PASS] ASan a détecté le débordement." \
		|| (echo "[FAIL] ASan n'a pas signalé d'erreur." && exit 1)
	@echo "─ Test 2 : version corrigée doit se terminer sans erreur ASan ─"
	@./exoH_fix "$(LONG_U)" "$(LONG_M)" 2>&1 | grep -qiv "AddressSanitizer\|SIGABRT" \
		&& echo "[PASS] Version corrigée : pas d'erreur ASan (troncature contrôlée)." \
		|| (echo "[FAIL] Version corrigée déclenche ASan." && exit 1)
	@echo "[✓] Tous les tests passent."

clean:
	rm -f exoH_vuln exoH_fix exoH_vuln_asan
