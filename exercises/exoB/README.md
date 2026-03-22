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

## Correction

```c
/* Un seul appel borné remplace trois strcat */
int written = snprintf(out, sizeof(out), "User: %s | Msg: %s", argv[1], argv[2]);
if ((size_t)written >= sizeof(out)) {
    /* message tronqué — décider : rejeter ou accepter la troncature */
}
```

**Pourquoi `snprintf` et pas `strncat` en chaîne ?**
`strncat(dst, src, n)` ajoute au plus `n` octets de `src`, mais `n` est *relatif à src*, pas à la capacité restante de `dst`. Il faudrait calculer la capacité restante à chaque appel, ce qui est fragile. `snprintf` construit tout en une seule opération avec une borne absolue.

## Vérification défensive

```bash
gcc -O0 -g -Wall -Wextra -Wpedantic -fsanitize=address,undefined \
    exoB_fix.c -o exoB_fix

# Cas long : troncature signalée proprement, pas de dépassement
./exoB_fix AAAAAAAAAAAAAAAAAAAAAAAAAAAAAA BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB

# Cas court : sortie normale
./exoB_fix Alice Hi
```

## Points clés

- La composition de plusieurs API sûres individuellement peut être globalement non-sûre
- `strcat` ne connaît pas la taille de la destination — elle lit jusqu'au `'\0'` initial
- Toujours raisonner sur la **taille totale finale**, pas sur chaque copie isolément
- `snprintf` est l'outil de construction de chaînes le plus sûr en C standard
