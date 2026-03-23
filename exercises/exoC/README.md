# Exercice C — Off-by-one (CWE-193)

## Intention pédagogique

Montrer qu'une erreur de borne d'un seul octet suffit à violer la sûreté mémoire. Le programme *semble* fonctionner sans signal visible — ce qui illustre le danger des comportements non définis silencieux.

## Code vulnérable

```c
char buf[8] = {0};

for (int i = 0; i <= 8; i++) {   /* BUG : <= au lieu de < */
    buf[i] = 'A';                 /* buf[8] est hors limites */
}
```

**Indices valides : 0, 1, 2, 3, 4, 5, 6, 7** — soit 8 indices (0..N-1).
La condition `i <= 8` itère jusqu'à `i = 8` → 9 itérations → 1 écriture hors limites.

## Exploitation pédagogique pas à pas

**Étape 1 — Compiler :**
```bash
gcc -O0 -g -Wall -Wextra -Wpedantic exoC_vuln.c -o exoC_vuln
```

**Étape 2 — Exécuter :**
```bash
./exoC_vuln
```
Le programme peut sembler fonctionner sans signal visible. C'est précisément le problème : le comportement est **non défini**, pas forcément un crash immédiat.

**Étape 3 — Chercher l'avertissement du compilateur :**
```bash
gcc -O0 -g -Wall -Wextra -Wpedantic exoC_vuln.c -o exoC_vuln 2>&1
```
GCC peut émettre `-Waggressive-loop-optimizations` ou un avertissement similaire sur certaines versions.

## Observation outillée avec ASan

```bash
gcc -O0 -g -fsanitize=address exoC_vuln.c -o exoC_vuln_asan
./exoC_vuln_asan
```
ASan signale immédiatement :
```
ERROR: AddressSanitizer: stack-buffer-overflow
WRITE of size 1 at 0x... shadow bytes around the buggy address
```

## À vous de jouer

Créez `exoC_fix.c` en corrigeant la vulnérabilité dans `exoC_vuln.c`.

**Critères de réussite :**
- Compile sans avertissement avec `-Wall -Wextra -Wpedantic`
- ASan ne signale aucune erreur
- Le programme écrit exactement 8 octets, tous dans les limites du tableau
