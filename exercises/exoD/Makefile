CC   = gcc
BASE = -O0 -g -Wall -Wextra -Wpedantic
VULN = $(BASE) -w
# ASan (pas UBSan) — montre le heap-buffer-overflow résultant de l'overflow entier
# Note: GCC UBSan ne supporte pas -fsanitize=unsigned-integer-overflow (option Clang).
# L'overflow uint32_t est un comportement DÉFINI (wrapping) — UBSan ne le signale pas.
# On utilise ASan pour montrer la conséquence : l'écriture hors du buffer trop petit.
ASAN = $(BASE) -w -fsanitize=address

.PHONY: all vuln asan demo clean

all: vuln asan

vuln: exoD_vuln.c
	$(CC) $(VULN) $< -o exoD_vuln

asan: exoD_vuln.c
	$(CC) $(ASAN) $< -o exoD_vuln_asan

demo: all
	@echo ""
	@echo "── Étape 1 : observer l'overflow arithmétique ──"
	./exoD_vuln || true
	@echo ""
	@echo "── Étape 2 : ASan détecte le heap-buffer-overflow résultant ──"
	./exoD_vuln_asan 2>&1 | head -25 || true

clean:
	rm -f exoD_vuln exoD_vuln_asan exoD_fix
