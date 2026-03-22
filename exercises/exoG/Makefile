CC   = gcc
BASE = -O0 -g -Wall -Wextra -Wpedantic
VULN = $(BASE) -w
FIX  = $(BASE) -Wuninitialized -Wmaybe-uninitialized

.PHONY: all vuln fix demo clean

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

clean:
	rm -f exoG_vuln exoG_fix
