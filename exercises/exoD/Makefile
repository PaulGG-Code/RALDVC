CC   = gcc
BASE = -O0 -g -Wall -Wextra -Wpedantic
VULN = $(BASE) -w
FIX  = $(BASE) -fsanitize=undefined
# ASan (pas UBSan) — montre le heap-buffer-overflow résultant de l'overflow entier
# Note: GCC UBSan ne supporte pas -fsanitize=unsigned-integer-overflow (option Clang).
# L'overflow uint32_t est un comportement DÉFINI (wrapping) — UBSan ne le signale pas.
# On utilise ASan pour montrer la conséquence : l'écriture hors du buffer trop petit.
ASAN = $(BASE) -w -fsanitize=address

.PHONY: all vuln fix asan demo clean

all: vuln fix asan

vuln: exoD_vuln.c
	$(CC) $(VULN) $< -o exoD_vuln

fix: exoD_fix.c
	$(CC) $(FIX) $< -o exoD_fix

asan: exoD_vuln.c
	$(CC) $(ASAN) $< -o exoD_vuln_asan

demo: all
	@echo ""
	@echo "── Étape 1 : observer l'overflow arithmétique ──"
	./exoD_vuln || true
	@echo ""
	@echo "── Étape 2 : ASan détecte le heap-buffer-overflow résultant ──"
	./exoD_vuln_asan 2>&1 | head -25 || true
	@echo ""
	@echo "── Étape 3 : version corrigée (__builtin_mul_overflow) ──"
	./exoD_fix 2>&1 || true

clean:
	rm -f exoD_vuln exoD_fix exoD_vuln_asan
