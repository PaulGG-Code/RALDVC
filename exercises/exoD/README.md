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

## Correction

```c
/* FIX 1 : size_t au lieu de uint32_t pour les calculs de taille */
size_t n = (size_t)1 << 30;

/* FIX 2 : __builtin_mul_overflow détecte l'overflow avant qu'il se produise */
size_t total;
if (__builtin_mul_overflow(n, (size_t)8, &total)) {
    fprintf(stderr, "Overflow — allocation refusée.\n");
    return 1;
}

/* FIX 3 : borne supérieure applicative */
if (total > MAX_ALLOC) { return 1; }
```

**Pourquoi `size_t` et pas `uint64_t` ?**
`size_t` est le type garanti par la norme C pour représenter toute taille d'objet allouable. Sur x86-64, il est 64 bits, ce qui rend l'overflow dans ce calcul impossible en pratique.

## Vérification défensive

```bash
gcc -O0 -g -Wall -Wextra -Wpedantic -fsanitize=undefined \
    exoD_fix.c -o exoD_fix
./exoD_fix
# → Overflow détecté dans n * 8 — allocation refusée.
```

Tester plusieurs valeurs frontières :
```bash
# n = 1 → total = 8 → OK
# n = 2^30 → overflow → refusé
# n = MAX_ALLOC/8 + 1 → total > MAX_ALLOC → refusé
```

## Points clés

- Les overflows entiers en C sont silencieux — il n'y a pas d'exception
- Toujours utiliser `size_t` pour les calculs de taille mémoire
- `__builtin_mul_overflow` (GCC/Clang) est le moyen le plus lisible de vérifier
- Les multiplications `count * sizeof(T)` pour `malloc` sont un vecteur classique de CVE (OpenSSH, Samba, etc.)
