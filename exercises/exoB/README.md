# Exercice B — Overflow cumulatif par concaténation (CWE-121)

## Intention pédagogique

La démonstration montre qu'une faille peut émerger d'une accumulation d'écritures apparemment « raisonnables » si la taille finale n'est jamais contrôlée. Chaque `strcat` semble anodin pris isolément ; c'est leur composition qui déborde.

## Code vulnérable

```c
char out[64] = "User: ";
strcat(out, argv[1]);        /* vérifie-t-il la capacité restante ? NON */
strcat(out, " | Msg: ");
strcat(out, argv[2]);        /* débordement possible si argv[1]+argv[2] > 50 chars */
puts(out);
```

**Anatomie du buffer :**
```
[ "User: " (6) | argv[1] | " | Msg: " (8) | argv[2] | '\0' ]
  ←─────────────────── 64 octets max ────────────────────────→
```
Si `strlen(argv[1]) + strlen(argv[2]) > 50`, le buffer déborde.

## Exploitation pédagogique pas à pas

**Étape 1 — Compiler :**
```bash
gcc -O0 -g -Wall -Wextra -Wpedantic exoB_vuln.c -o exoB_vuln
```

**Étape 2 — Cas nominal :**
```bash
./exoB_vuln Alice Hi
# → User: Alice | Msg: Hi
```

**Étape 3 — Overflow cumulatif :**
```bash
./exoB_vuln AAAAAAAAAAAAAAAAAAAAAAAAAAAAAA BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
# A(30) + " | Msg: "(8) + B(40) = 78 octets > 58 restants → débordement
```
Le comportement est indéfini : crash, corruption silencieuse, ou sortie apparemment normale selon la pile.

## Observation outillée avec ASan

```bash
gcc -O0 -g -fsanitize=address exoB_vuln.c -o exoB_vuln_asan
./exoB_vuln_asan AAAAAAAAAAAAAAAAAAAAAAAAAAAAAA BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
```
ASan signale `stack-buffer-overflow` et pointe exactement le `strcat` responsable.

## À vous de jouer

Créez `exoB_fix.c` en corrigeant la vulnérabilité dans `exoB_vuln.c`.

**Critères de réussite :**
- Compile sans avertissement avec `-Wall -Wextra -Wpedantic`
- ASan ne signale aucune erreur sur les entrées longues
- Le cas nominal (`Alice Hi`) produit toujours la même sortie
- Les entrées qui dépassent la capacité du buffer sont gérées proprement (troncature ou rejet)
