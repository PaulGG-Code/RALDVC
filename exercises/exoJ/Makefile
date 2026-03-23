CC   = gcc
BASE = -O0 -g -Wall -Wextra -Wpedantic
VULN = $(BASE) -w

# Fichier et payload pour les démonstrations
SAFE_FILE   = /etc/hostname
INJECT_ARG  = /etc/hostname; echo INJECTED_CMD

.PHONY: all vuln demo clean

all: vuln

vuln: exoJ_vuln.c
	$(CC) $(VULN) $< -o exoJ_vuln

demo: all
	@echo ""
	@echo "── Cas nominal (fichier sûr) ──"
	./exoJ_vuln "$(SAFE_FILE)" || true
	@echo ""
	@echo "── Injection via métacaractère ';' (vulnérable) ──"
	./exoJ_vuln "$(INJECT_ARG)" 2>&1 || true

clean:
	rm -f exoJ_vuln exoJ_fix
