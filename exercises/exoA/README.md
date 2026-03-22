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
(gdb) break main
(gdb) run AAAAAAAAAAAAAAAAAAAA
(gdb) next                        # avancer jusqu'après strcpy
(gdb) print s.role                # observer la valeur corrompue
(gdb) print/x s.role              # en hexadécimal : 0x41414141
(gdb) x/20x &s                    # dump mémoire de la struct
(gdb) quit
```

## Correction

```c
/* strncpy borné + null-termination explicite */
strncpy(s.user, argv[1], sizeof(s.user) - 1);
s.user[sizeof(s.user) - 1] = '\0';
```

**Pourquoi `sizeof(s.user) - 1` et pas juste `sizeof(s.user)` ?**
`strncpy(dst, src, n)` copie *exactement* n octets : si `strlen(src) >= n`, il n'ajoute **pas** de `'\0'`. Réserver le dernier octet et l'écrire explicitement garantit que la chaîne est toujours terminée.

## Vérification défensive post-correction

```bash
gcc -O0 -g -Wall -Wextra -Wpedantic \
    -fsanitize=address,undefined \
    -fstack-protector-all \
    exoA_fix.c -o exoA_fix

./exoA_fix AAAAAAAAAAAAAAAAAAAA
# → USER — rôle intact, overflow bloqué.
```

Le binaire corrigé ne produit ni alerte sanitizer ni bascule logique.

## Points clés

- `strcpy` est dangereuse car elle n'a aucun argument de taille
- Un overflow de 1 octet suffit à corrompre la variable adjacente
- La disposition mémoire des structs est déterministe (sans padding excessif avec `-O0`)
- `strncpy` + null-termination explicite est le patron minimal de correction
- Pour du code production, préférer `strlcpy` (BSD/glibc ≥ 2.38) ou `snprintf`
