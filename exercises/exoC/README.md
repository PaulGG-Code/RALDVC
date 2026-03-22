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

## Correction

```c
/* Strict < au lieu de <= */
for (size_t i = 0; i < sizeof(buf); i++) {
    buf[i] = 'A';
}
```

**Règle invariante :** pour un tableau `T[N]`, les indices valides sont `0..N-1`. La condition de boucle doit être `i < N` (jamais `i <= N`).

Utiliser `sizeof(buf)` plutôt qu'un littéral `8` est une bonne pratique : si la taille du buffer change, la condition reste automatiquement correcte.

## Vérification défensive

```bash
gcc -O0 -g -Wall -Wextra -Wpedantic \
    -fsanitize=address,undefined \
    exoC_fix.c -o exoC_fix

./exoC_fix
# → done — 8 octets écrits, tous dans les limites.
```

Aucune alerte sanitizer, sortie propre.

## Points clés

- L'erreur `<=` vs `<` est l'un des bugs les plus fréquents en C
- Un comportement non défini silencieux est souvent plus dangereux qu'un crash : il peut corrompre une variable adjacente sans signal visible
- Les sanitizers (ASan) détectent ce bug à l'exécution ; les analyseurs statiques (cppcheck, clang-tidy) peuvent le détecter sans exécuter le code
- Sur le tas, un off-by-one peut corrompre les métadonnées du chunk glibc suivant
