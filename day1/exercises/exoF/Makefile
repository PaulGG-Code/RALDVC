CC   = gcc
BASE = -O0 -g -Wall -Wextra -Wpedantic
VULN = $(BASE) -w           # masquer l'avertissement pour la démo
FIX  = $(BASE) -Wformat -Wformat-security -Werror=format-security

.PHONY: all vuln fix demo check clean

all: vuln fix

vuln: exoF_vuln.c
	$(CC) $(VULN) $< -o exoF_vuln
	@echo "[+] exoF_vuln construit (sans -Wformat-security)"

fix: exoF_fix.c
	$(CC) $(FIX) $< -o exoF_fix
	@echo "[+] exoF_fix construit (avec -Werror=format-security)"

demo: all
	@echo ""
	@echo "── Cas nominal ──"
	./exoF_vuln "hello world"
	@echo ""
	@echo "── Fuite de pile avec %%x ──"
	./exoF_vuln "%x %x %x %x %x %x %x %x" || true
	@echo ""
	@echo "── Fuite avec %%p (pointeurs) ──"
	./exoF_vuln "%p %p %p %p" || true
	@echo ""
	@echo "── Version corrigée (%%x traité littéralement) ──"
	./exoF_fix "%x %x %x %x"
	@echo ""
	@echo "── Tentative de compiler vuln avec -Werror=format-security ──"
	$(CC) -O0 -g -Wformat -Wformat-security -Werror=format-security \
	    exoF_vuln.c -o /dev/null 2>&1 || true

check: all
	@echo "=== Vérification automatique (exoF) ==="
	@echo "─ Test 1 : compiler le code vulnérable avec -Werror=format-security doit échouer ─"
	@$(CC) -O0 -g -Wformat -Wformat-security -Werror=format-security \
		exoF_vuln.c -o /dev/null 2>/dev/null \
		&& (echo "[FAIL] La compilation a réussi — code vulnérable non détecté." && exit 1) \
		|| echo "[PASS] Compilation refusée avec -Werror=format-security."
	@echo "─ Test 2 : version corrigée doit se terminer sans erreur ─"
	@./exoF_fix "%x %x %x" \
		&& echo "[PASS] Version corrigée : terminaison propre." \
		|| (echo "[FAIL] Version corrigée : code de sortie non-nul." && exit 1)
	@echo "[✓] Tous les tests passent."

clean:
	rm -f exoF_vuln exoF_fix
