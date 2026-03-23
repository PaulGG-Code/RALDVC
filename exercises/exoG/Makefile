CC   = gcc
BASE = -O0 -g -Wall -Wextra -Wpedantic
VULN = $(BASE) -w

.PHONY: all vuln demo clean

all: vuln

vuln: exoG_vuln.c
	$(CC) $(VULN) $< -o exoG_vuln

demo: all
	@echo ""
	@echo "── Lecture de mémoire non initialisée (exécution 1) ──"
	./exoG_vuln
	@echo ""
	@echo "── Lecture de mémoire non initialisée (exécution 2) ──"
	./exoG_vuln
	@echo ""
	@echo "── Détection Valgrind ──"
	valgrind --tool=memcheck --error-exitcode=1 ./exoG_vuln 2>&1 | \
	    grep -iE "(uninitialised|invalid read|ERROR SUMMARY)" | head -10 || true

clean:
	rm -f exoG_vuln exoG_fix
