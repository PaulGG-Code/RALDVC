# Exercice A — Stack overflow avec impact logique (CWE-121)

## Intention pédagogique

L'exploitation pédagogique consiste à démontrer qu'un dépassement de tampon peut casser une règle métier locale — ici une variable de rôle. Le but n'est pas l'exécution arbitraire de code, mais la preuve qu'un bug mémoire compromet une décision d'autorisation.

## Code vulnérable

```c
typedef struct {
    char         user[16];
    unsigned int role;      /* 0 = USER, !=0 = ADMIN */
} session_t;

session_t s;
s.role = 0;
strcpy(s.user, argv[1]);   /* pas de borne → role corruptible */
if (s.role != 0) puts("ADMIN");
else              puts("USER");
```

**Disposition mémoire (pile) :**
```
[ user[0..15] | role(4 octets) | ... ]
      ^--- strcpy écrit ici --->^
```
À partir du 17e octet, strcpy déborde dans `role`.

## Exploitation pédagogique pas à pas

**Étape 1 — Compiler sans protections (pour observer le comportement) :**
```bash
gcc -O0 -g -Wall -Wextra -Wpedantic \
    -fno-stack-protector -no-pie \
    exoA_vuln.c -o exoA_vuln
```

**Étape 2 — Comportement nominal :**
```bash
./exoA_vuln Paul
# → USER (rôle intact)
```

**Étape 3 — Déclencher le dépassement :**
```bash
./exoA_vuln AAAAAAAAAAAAAAAAAAAA   # 20 caractères
# → ADMIN (role = 0x41414141)
```
Les 4 octets 'A' (0x41) dépassent `user[16]` et écrasent `role` avec 0x41414141 ≠ 0.

**Étape 4 — Trouver le seuil exact :**
```bash
./exoA_vuln AAAAAAAAAAAAAAAA    # 16 A → USER
./exoA_vuln AAAAAAAAAAAAAAAAA   # 17 A → ADMIN (1 octet dans role)
```

## Observation outillée avec ASan

```bash
gcc -O0 -g -fsanitize=address,undefined exoA_vuln.c -o exoA_vuln_asan
./exoA_vuln_asan AAAAAAAAAAAAAAAAAAAA
```
ASan signale `stack-buffer-overflow` et localise précisément la ligne du `strcpy`.

## Observation avec GDB

```bash
gdb -q ./exoA_vuln
```

```
# Placer le breakpoint directement sur la ligne strcpy (ligne 44)
# "break main" s'arrêterait à la toute première ligne de main — il faudrait
# appuyer sur 'next' de nombreuses fois pour atteindre strcpy.
# On cible la ligne exacte du strcpy pour être précis.
(gdb) break exoA_vuln.c:44
(gdb) run AAAAAAAAAAAAAAAAAAAA

# GDB s'arrête AVANT d'exécuter strcpy
(gdb) print s.role                # affiche 0  — valeur initiale
(gdb) next                        # exécute strcpy (overflow se produit ici)

# GDB s'arrête à la ligne suivante, APRÈS le strcpy
(gdb) print s.role                # affiche 1094795585  — corrompu !
(gdb) print/x s.role              # affiche 0x41414141  (4 octets 'A')

# Dump octet par octet de la struct entière (user[16] + role[4] = 20 octets)
(gdb) x/20bx &s                   # 'b' = byte, 'x' = hexadécimal
#  → user[0..15] : 0x41 0x41 ... 0x41
#  → role[0..3]  : 0x41 0x41 0x41 0x41  ← les 'A' qui ont débordé

(gdb) quit
```

## À vous de jouer

Créez `exoA_fix.c` en corrigeant la vulnérabilité dans `exoA_vuln.c`.

**Critères de réussite :**
- Compile sans avertissement avec `-Wall -Wextra -Wpedantic`
- ASan ne signale aucune erreur avec l'entrée de 20 caractères
- Le comportement nominal est préservé (`./exoA_fix Paul` affiche `USER`)
