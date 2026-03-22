# Exercice G — Mémoire non initialisée (CWE-457)

## Intention pédagogique

Montrer une fuite d'information locale due à un état non initialisé. La sortie du programme n'est pas fiable et peut contenir des résidus de pile d'exécutions précédentes ou du loader.

## Code vulnérable

```c
typedef struct {
    int  id;
    char token[16];
} record_t;

record_t r;       /* non initialisée — token contient des résidus */
r.id = 1;
printf("%d %s\n", r.id, r.token);   /* r.token : lecture d'état indéfini */
```

**Ce que contient `r.token` sans initialisation :**
```
Adresse mémoire sur la pile, contenu provenant de :
  → cadres de fonctions précédentes (données résiduelles)
  → variables d'environnement (copie par le loader au démarrage)
  → tout ce qui occupait cette zone avant main()
```

## Exploitation pédagogique pas à pas

**Étape 1 — Compiler :**
```bash
gcc -O0 -g -Wall -Wextra -Wpedantic exoG_vuln.c -o exoG_vuln
# Note : GCC peut émettre -Wuninitialized ou -Wmaybe-uninitialized
```

**Étape 2 — Exécuter plusieurs fois :**
```bash
./exoG_vuln
./exoG_vuln
./exoG_vuln
```
Observer que la sortie de `r.token` (hexadécimale) peut varier ou être constante selon le niveau d'isolation du processus.

**Étape 3 — Forcer un résidu en pile :**
```bash
# Modifier exoG_vuln.c temporairement pour appeler une fonction avant main :
# void pollute(void) { char buf[16] = "SECRET_KEY_12345"; (void)buf; }
# et constater que token peut contenir "SECRET_KEY_12345"
```

**Étape 4 — Comparer entre exécutions et environnements :**
Sur certains systèmes/compilateurs, `r.token` affiche toujours 0x00 (le BSS ou la pile sont zeroed) — mais c'est un comportement *non garanti* et *non portable*.

## Observation avec Valgrind

```bash
valgrind --tool=memcheck ./exoG_vuln
```
Valgrind signale (noter la casse minuscule dans les messages réels) :
```
==PID== Conditional jump or move depends on uninitialised value(s)
==PID== Use of uninitialised value of size 8
```
Ces deux messages pointent vers la lecture du champ `token` non initialisé dans `printf`.

## Correction

```c
/* Initialisation à zéro de toute la struct en une ligne */
record_t r = {0};
r.id = 1;
```

**Pourquoi `= {0}` initialise-t-il tous les membres ?**
Norme C99 §6.7.8 : si un initialiseur est fourni pour le premier membre, tous les membres restants sont initialisés implicitement à leur zéro (0, NULL, 0.0, ou équivalent selon le type).

`= {0}` est donc équivalent à `memset(&r, 0, sizeof(r))` mais exprimé au niveau sémantique, sans magic numbers.

## Vérification défensive

```bash
gcc -O0 -g -Wall -Wextra -Wpedantic \
    -Wuninitialized -Wmaybe-uninitialized \
    exoG_fix.c -o exoG_fix

./exoG_fix
# → r.token hex : 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  (tous zéros)
```

Comparer plusieurs exécutions — la sortie doit être identique et prévisible.

## Points clés

- En C, les variables locales ne sont **pas** initialisées à zéro par défaut (contrairement à Java ou Python)
- Un token, un mot de passe ou une clé non initialisé peut être accidentellement exposé dans des logs ou des réponses réseau
- `= {0}` est la façon la plus concise et correcte d'initialiser n'importe quelle struct/tableau à zéro
- `calloc()` (au lieu de `malloc()`) retourne de la mémoire zeroed — utile pour les allocations sur le tas
