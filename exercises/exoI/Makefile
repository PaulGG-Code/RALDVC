CC   = gcc
BASE = -O0 -g -Wall -Wextra -Wpedantic
VULN = $(BASE) -w
ASAN = $(BASE) -w -fsanitize=address

.PHONY: all vuln asan demo clean

all: vuln asan

vuln: exoI_vuln.c
	$(CC) $(VULN) $< -o exoI_vuln

asan: exoI_vuln.c
	$(CC) $(ASAN) $< -o exoI_vuln_asan

demo: all
	@echo ""
	@echo "── Exécution vulnérable (crash ou corruption silencieuse) ──"
	./exoI_vuln 2>&1 || true
	@echo ""
	@echo "── Détection ASan (double-free) ──"
	./exoI_vuln_asan 2>&1 | head -25 || true

clean:
	rm -f exoI_vuln exoI_vuln_asan exoI_fix
