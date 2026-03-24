CC   = gcc
BASE = -O0 -g -Wall -Wextra -Wpedantic
VULN = $(BASE) -w
FIX  = $(BASE) -Wuninitialized -Wmaybe-uninitialized

.PHONY: all vuln fix demo check clean

all: vuln fix

vuln: exoG_vuln.c
	$(CC) $(VULN) $< -o exoG_vuln

fix: exoG_fix.c
	$(CC) $(FIX) $< -o exoG_fix

demo: all
	@echo ""
	@echo "── Lecture de mémoire non initialisée (exécution 1) ──"
	./exoG_vuln
	@echo ""
	@echo "── Lecture de mémoire non initialisée (exécution 2) ──"
	./exoG_vuln
	@echo ""
	@echo "── Version corrigée (initialisée à zéro) ──"
	./exoG_fix
	@echo ""
	@echo "── Détection Valgrind ──"
	valgrind --tool=memcheck --error-exitcode=1 ./exoG_vuln 2>&1 | \
	    grep -iE "(uninitialised|invalid read|ERROR SUMMARY)" | head -10 || true

check: all
	@echo "=== Vérification automatique (exoG) ==="
	@echo "─ Test 1 : Valgrind doit détecter la lecture de mémoire non initialisée ─"
	@valgrind --tool=memcheck --error-exitcode=1 ./exoG_vuln 2>&1 | \
		grep -qi "uninitialised\|Conditional jump\|Invalid read" \
		&& echo "[PASS] Valgrind a détecté la mémoire non initialisée." \
		|| (echo "[FAIL] Valgrind n'a pas signalé d'erreur." && exit 1)
	@echo "─ Test 2 : version corrigée doit passer Valgrind sans erreur ─"
	@valgrind --tool=memcheck --error-exitcode=1 ./exoG_fix 2>/dev/null \
		&& echo "[PASS] Version corrigée : Valgrind OK." \
		|| (echo "[FAIL] Version corrigée : Valgrind signale des erreurs." && exit 1)
	@echo "[✓] Tous les tests passent."

clean:
	rm -f exoG_vuln exoG_fix
