CC   = gcc
BASE = -O0 -g -Wall -Wextra -Wpedantic
VULN = $(BASE) -w
ASAN = $(BASE) -w -fsanitize=address

.PHONY: all vuln asan demo clean

all: vuln asan

vuln: exoC_vuln.c
	$(CC) $(VULN) $< -o exoC_vuln

asan: exoC_vuln.c
	$(CC) $(ASAN) $< -o exoC_vuln_asan

demo: all
	@echo ""
	@echo "── Comportement vulnérable (peut sembler OK) ──"
	./exoC_vuln || true
	@echo ""
	@echo "── Détection ASan ──"
	./exoC_vuln_asan 2>&1 | head -20 || true

clean:
	rm -f exoC_vuln exoC_vuln_asan exoC_fix
