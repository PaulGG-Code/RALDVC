CC      = gcc
BASE    = -O0 -g -Wall -Wextra -Wpedantic
VULN    = $(BASE) -fno-stack-protector -no-pie -w
ASAN    = $(BASE) -fno-stack-protector -no-pie -fsanitize=address,undefined -w

.PHONY: all vuln asan demo clean

all: vuln asan

vuln: exoA_vuln.c
	$(CC) $(VULN) $< -o exoA_vuln
	@echo "[+] exoA_vuln built (no mitigations)"

asan: exoA_vuln.c
	$(CC) $(ASAN) $< -o exoA_vuln_asan
	@echo "[+] exoA_vuln_asan built (vulnerable code + ASan detector)"

demo: all
	@echo ""
	@echo "── Étape 1 : comportement nominal ──"
	./exoA_vuln Paul
	@echo ""
	@echo "── Étape 2 : overflow de 20 caractères ──"
	./exoA_vuln AAAAAAAAAAAAAAAAAAAA || true
	@echo ""
	@echo "── Étape 3 : détection ASan ──"
	./exoA_vuln_asan AAAAAAAAAAAAAAAAAAAA 2>&1 | head -20 || true

clean:
	rm -f exoA_vuln exoA_vuln_asan exoA_fix
