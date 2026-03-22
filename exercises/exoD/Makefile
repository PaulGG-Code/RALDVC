CC   = gcc
BASE = -O0 -g -Wall -Wextra -Wpedantic
VULN = $(BASE) -w
FIX  = $(BASE) -fsanitize=undefined
USAN = $(BASE) -w -fsanitize=undefined

.PHONY: all vuln fix asan demo clean

all: vuln fix asan

vuln: exoD_vuln.c
	$(CC) $(VULN) $< -o exoD_vuln

fix: exoD_fix.c
	$(CC) $(FIX) $< -o exoD_fix

asan: exoD_vuln.c
	$(CC) $(USAN) $< -o exoD_vuln_ubsan

demo: all
	@echo ""
	@echo "── Overflow de taille (vulnérable) ──"
	./exoD_vuln || true
	@echo ""
	@echo "── Détection UBSan ──"
	./exoD_vuln_ubsan 2>&1 || true
	@echo ""
	@echo "── Version corrigée ──"
	./exoD_fix 2>&1 || true

clean:
	rm -f exoD_vuln exoD_fix exoD_vuln_ubsan
