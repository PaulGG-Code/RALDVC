# Exercice D — Integer overflow de taille (CWE-190)

## Intention pédagogique

L'exploitation pédagogique consiste à montrer qu'une erreur arithmétique peut invalider une allocation sans crash immédiat, préparant une corruption ultérieure. L'overflow est silencieux — aucune exception, aucun signal.

## Code vulnérable

```c
uint32_t n     = 1U << 30;    /* 1 073 741 824          */
uint32_t total = n * 8;       /* 8 589 934 592 → OVERFLOW → 0 */
char *p = malloc(total);      /* malloc(0) — buffer minuscule  */
/* suite : écriture de n*8 octets → débordement de tas         */
```

**Arithmétique modulo 2³² :**
```
n     = 0x40000000
n * 8 = 0x200000000  → tronqué à 32 bits → 0x00000000 = 0
```

## Exploitation pédagogique pas à pas

**Étape 1 — Compiler et exécuter :**
```bash
gcc -O0 -g -Wall -Wextra -Wpedantic exoD_vuln.c -o exoD_vuln
./exoD_vuln
```
Observer que `total = 0` et que `malloc(0)` peut réussir (retourne un pointeur valide mais inutilisable).

**Étape 2 — Observer la différence entre valeur attendue et valeur calculée :**
```
n * 8 attendu  : 8 589 934 592 octets  (8 Go)
n * 8 obtenu   : 0 octets  (après overflow uint32_t)
```

**Étape 3 — Varier n pour trouver d'autres valeurs d'overflow :**
```
n = 0x20000001 → n * 8 = 0x100000008 → tronqué à 0x00000008 = 8 octets
Allocation de 8 octets, écriture de 0x20000001 * 8 = 4 Go → débordement massif
```

## Pourquoi UBSan ne détecte PAS cet overflow ?

C'est un point important à comprendre :

```
uint32_t n = 1U << 30;
uint32_t total = n * 8;   ← overflow DÉFINI en C, pas UB
```

L'overflow d'entiers **non signés** (`uint32_t`, `unsigned int`, etc.) est un **comportement DÉFINI** par la norme C : le résultat est simplement tronqué modulo 2^32. Ce n'est pas du comportement indéfini (UB).

UBSan ne signale que les comportements **indéfinis**. L'overflow non signé n'en est pas un → UBSan reste silencieux.

> **GCC UBSan ne supporte pas `-fsanitize=unsigned-integer-overflow`** (c'est une option Clang uniquement).
> Seul l'overflow **signé** (`int`, `long`) est UB et capturé par GCC UBSan.

## Observation outillée avec ASan — conséquence du heap overflow

À la place, on utilise ASan pour détecter la **conséquence** de l'overflow : l'écriture hors du buffer trop petit.

```bash
gcc -O0 -g -fsanitize=address exoD_vuln.c -o exoD_vuln_asan
./exoD_vuln_asan
```
ASan signale `heap-buffer-overflow` lors du `memset(buf, 'A', 32)` dans un buffer de 0 ou 1 octet — la preuve que l'overflow arithmétique a produit une allocation insuffisante.

```
ERROR: AddressSanitizer: heap-buffer-overflow on address ...
WRITE of size 32 at ...
0x... is located 0 bytes after 1-byte region [...]
```

## À vous de jouer

Créez `exoD_fix.c` en corrigeant la vulnérabilité dans `exoD_vuln.c`.

**Critères de réussite :**
- Compile sans avertissement avec `-Wall -Wextra -Wpedantic`
- Le programme détecte et rejette l'allocation invalide avant qu'elle ne se produise
- ASan ne signale aucune erreur de heap-buffer-overflow

**Indice :** pensez au type utilisé pour les calculs de taille, et à la façon de détecter l'overflow avant d'appeler `malloc`.
