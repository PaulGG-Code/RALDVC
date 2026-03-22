CC   = gcc
BASE = -O0 -g -Wall -Wextra -Wpedantic
VULN = $(BASE) -w
FIX  = $(BASE) -fsanitize=address,undefined
ASAN = $(BASE) -w -fsanitize=address

.PHONY: all vuln fix asan demo clean

all: vuln fix asan

vuln: exoC_vuln.c
	$(CC) $(VULN) $< -o exoC_vuln

fix: exoC_fix.c
	$(CC) $(FIX) $< -o exoC_fix

asan: exoC_vuln.c
	$(CC) $(ASAN) $< -o exoC_vuln_asan

demo: all
	@echo ""
	@echo "── Comportement vulnérable (peut sembler OK) ──"
	./exoC_vuln || true
	@echo ""
	@echo "── Détection ASan ──"
	./exoC_vuln_asan 2>&1 | head -20 || true
	@echo ""
	@echo "── Version corrigée ──"
	./exoC_fix

clean:
	rm -f exoC_vuln exoC_fix exoC_vuln_asan
