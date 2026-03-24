CC   = gcc
BASE = -O0 -g -Wall -Wextra -Wpedantic
VULN = $(BASE) -w
FIX  = $(BASE)

# Fichier et payload pour les démonstrations
SAFE_FILE   = /etc/hostname
INJECT_ARG  = /etc/hostname; echo INJECTED_CMD

.PHONY: all vuln fix demo check clean

all: vuln fix

vuln: exoJ_vuln.c
	$(CC) $(VULN) $< -o exoJ_vuln

fix: exoJ_fix.c
	$(CC) $(FIX) $< -o exoJ_fix

demo: all
	@echo ""
	@echo "── Cas nominal (fichier sûr) ──"
	./exoJ_vuln "$(SAFE_FILE)" || true
	@echo ""
	@echo "── Injection via métacaractère ';' (vulnérable) ──"
	./exoJ_vuln "$(INJECT_ARG)" 2>&1 || true
	@echo ""
	@echo "── Cas nominal corrigé ──"
	./exoJ_fix "$(SAFE_FILE)" || true
	@echo ""
	@echo "── Même injection — version corrigée (echo ne s'exécute pas) ──"
	./exoJ_fix "$(INJECT_ARG)" 2>&1 || true

check: all
	@echo "=== Vérification automatique (exoJ) ==="
	@echo "─ Test 1 : la version vulnérable doit exécuter la commande injectée ─"
	@./exoJ_vuln "/etc/hostname; echo INJECTED_CMD" 2>&1 | grep -q "INJECTED_CMD" \
		&& echo "[PASS] Injection démontrée dans la version vulnérable." \
		|| (echo "[FAIL] Injection non détectée — vérifier le code vulnérable." && exit 1)
	@echo "─ Test 2 : la version corrigée doit bloquer l'injection ─"
	@./exoJ_fix "/etc/hostname; echo INJECTED_CMD" 2>&1 | grep -q "INJECTED_CMD" \
		&& (echo "[FAIL] Injection réussie dans la version corrigée !" && exit 1) \
		|| echo "[PASS] Injection bloquée dans la version corrigée."
	@echo "[✓] Tous les tests passent."

clean:
	rm -f exoJ_vuln exoJ_fix
