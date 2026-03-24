# Exercice E — Use-After-Free (CWE-416)

## Intention pédagogique

Montrer le non-déterminisme d'un accès après libération. Ce n'est pas un comportement « parfois acceptable », c'est une erreur systémique. La valeur lue peut varier d'une exécution à l'autre, selon ce que l'allocateur a fait de la mémoire libérée.

## Code vulnérable

```c
int *p = malloc(sizeof(int));
*p = 42;
free(p);
printf("%d\n", *p);   /* accès après free — comportement non défini */
```

**Ce qui peut se passer après `free(p)` :**
```
glibc tcache : écrit un pointeur (fd) dans les premiers octets du bloc libéré
               → *p ne contient plus 42, mais une adresse glibc interne
               → ou 42 si le bloc n'a pas encore été touché (non fiable)
```

## Exploitation pédagogique pas à pas

**Étape 1 — Compiler et observer le non-déterminisme :**
```bash
gcc -O0 -g -Wall -Wextra -Wpedantic exoE_vuln.c -o exoE_vuln
./exoE_vuln
./exoE_vuln   # relancer plusieurs fois
```
La valeur affichée pour `*p` après `free` peut varier ou rester stable selon les exécutions.

**Étape 2 — Observer ce que glibc écrit dans le bloc libéré avec GDB :**
```bash
gdb -q ./exoE_vuln
```

```
# On cible directement la ligne du free() (ligne 23 dans exoE_vuln.c)
# pour s'arrêter juste avant et juste après.
(gdb) break exoE_vuln.c:23
(gdb) run

# GDB s'arrête AVANT free(p)
(gdb) print *p               # affiche 42  — valeur initiale correcte
(gdb) x/2gx p                # dump 2 mots de 64 bits : [ 0x2a | ... ]
                              #   0x2a = 42 en hexadécimal

(gdb) next                   # exécute free(p)

# GDB s'arrête APRÈS free(p)
(gdb) print *p               # valeur différente ! glibc a écrit ses métadonnées
                              # tcache dans les premiers octets du bloc libéré
(gdb) x/2gx p                # visualiser les 16 octets bruts — plus 0x2a (42)
                              # mais un pointeur de la freelist glibc tcache

(gdb) quit
```

> **Note :** `break main` puis plusieurs `next` aurait fonctionné mais de manière
> fastidieuse. Cibler `break exoE_vuln.c:23` est précis et reproductible.

**Étape 3 — Visualiser le recyclage mémoire :**
```c
/* Exemple illustratif — que se passe-t-il si on alloue entre free et accès ? */
int *p = malloc(sizeof(int));
*p = 42;
free(p);
int *q = malloc(sizeof(int));   /* q == p sur glibc (tcache reuse) */
*q = 99;
printf("%d\n", *p);             /* affiche 99 — le « 42 » a été écrasé */
free(q);
```

## Observation outillée avec ASan

```bash
gcc -O0 -g -fsanitize=address exoE_vuln.c -o exoE_vuln_asan
./exoE_vuln_asan
```
ASan signale :
```
ERROR: AddressSanitizer: heap-use-after-free
READ of size 4 at 0x...
previously freed here
```

## Observation avec Valgrind

```bash
valgrind --tool=memcheck ./exoE_vuln
```
Valgrind signale `Invalid read of size 4` et montre la pile d'appel du `free` et de l'accès invalide.

## Correction

```c
free(p);
p = NULL;   /* invalider immédiatement */

/* Plus tard : */
if (p != NULL) {
    /* seul contexte où on peut déréférencer */
}
```

**Pourquoi `p = NULL` après `free` ?**
- `free(NULL)` est un no-op garanti par la norme C → double-free accidentel inoffensif
- Tout déréférencement ultérieur de `p` provoque un SIGSEGV immédiat et identifiable
- Les sanitizers et Valgrind confirment que le code corrigé est propre

## Vérification défensive

```bash
gcc -O0 -g -Wall -Wextra -Wpedantic -fsanitize=address \
    exoE_fix.c -o exoE_fix
./exoE_fix
# → p est NULL — accès bloqué, pas d'use-after-free.
```

## Points clés

- `free()` ne met pas le pointeur à NULL — c'est au programmeur de le faire
- La mémoire libérée peut être réallouée à tout moment pour un autre objet
- UAF est l'une des classes de vulnérabilités les plus exploitées (navigateurs, noyaux Linux)
- Macro recommandée : `#define safe_free(p) do { free(p); (p) = NULL; } while(0)`
