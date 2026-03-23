CC   = gcc
BASE = -O0 -g -Wall -Wextra -Wpedantic
VULN = $(BASE) -w           # masquer l'avertissement pour la démo

.PHONY: all vuln demo clean

all: vuln

vuln: exoF_vuln.c
	$(CC) $(VULN) $< -o exoF_vuln
	@echo "[+] exoF_vuln construit (sans -Wformat-security)"

demo: all
	@echo ""
	@echo "── Cas nominal ──"
	./exoF_vuln "hello world"
	@echo ""
	@echo "── Fuite de pile avec %%x ──"
	./exoF_vuln "%x %x %x %x %x %x %x %x" || true
	@echo ""
	@echo "── Fuite avec %%p (pointeurs) ──"
	./exoF_vuln "%p %p %p %p" || true
	@echo ""
	@echo "── Tentative de compiler vuln avec -Werror=format-security ──"
	$(CC) -O0 -g -Wformat -Wformat-security -Werror=format-security \
	    exoF_vuln.c -o /dev/null 2>&1 || true

clean:
	rm -f exoF_vuln exoF_fix
