# Exercice F — Format String Misuse (CWE-134)

## Intention pédagogique

Expliquer qu'une entrée utilisateur ne doit jamais être interprétée comme chaîne de format. Le compilateur lui-même peut détecter ce bug avec le bon flag.

## Code vulnérable

```c
printf(argv[1]);   /* argv[1] EST la chaîne de format */
```

`printf` interprète son premier argument comme un motif de format. Si `argv[1]` contient `%x`, `printf` lira un entier sur la pile. Si `argv[1]` contient `%n`, `printf` **écrira** à l'adresse pointée par le prochain argument de la pile.

## Exploitation pédagogique pas à pas

**Étape 1 — Compiler :**
```bash
gcc -O0 -g -Wall -Wextra -Wpedantic exoF_vuln.c -o exoF_vuln
# GCC émet : warning: format not a string literal and no format arguments
```

**Étape 2 — Cas nominal (entrée sans directives) :**
```bash
./exoF_vuln "hello world"
# → Message reçu : hello world
```

**Étape 3 — Fuite de la pile avec %x :**
```bash
./exoF_vuln "%x %x %x %x %x %x %x %x"
# → des valeurs hexadécimales de la pile s'affichent
#   Ces valeurs peuvent contenir : adresses de retour, variables locales,
#   données sensibles résidant dans le cadre d'appel de printf
```

**Étape 4 — Fuite avec %p (pointeurs) :**
```bash
./exoF_vuln "%p %p %p %p %p %p"
# → adresses en notation 0x...
```

**Étape 5 — Tentative de lecture arbitraire :**
```bash
./exoF_vuln "%s"
# → peut crasher (déréférencement d'une valeur arbitraire de la pile comme pointeur)
# → ou afficher des données erratiques
```

## Outil défensif compilateur

```bash
gcc -O0 -g -Wformat -Wformat-security -Werror=format-security exoF_vuln.c -o exoF_vuln
# Erreur : error: format not a string literal and no format arguments
#          (avec -Werror, cela devient une erreur de compilation)
```
`-Wformat-security` transforme ce pattern en avertissement ; `-Werror=format-security` le transforme en erreur qui interrompt la compilation.

## Correction

```c
printf("%s", argv[1]);   /* SAFE : "%s" est un littéral de format */
```

**Changement minimal : `printf(x)` → `printf("%s", x)`.**

## Vérification

```bash
gcc -O0 -g -Wall -Wextra -Wpedantic \
    -Wformat -Wformat-security -Werror=format-security \
    exoF_fix.c -o exoF_fix

./exoF_fix "hello"
./exoF_fix "%x %x %x %x"
# → affiche "%x %x %x %x" littéralement, sans interprétation
```

Recompiler `exoF_vuln.c` avec `-Werror=format-security` pour vérifier que le compilateur aurait pu bloquer ce bug dès la compilation.

## Points clés

- La chaîne de format de `printf` doit **toujours** être un littéral sous contrôle du développeur
- `%n` permet une écriture mémoire arbitraire — c'est la primitive d'exploitation la plus puissante des vulnérabilités de format string
- `-Wformat-security` dans le Makefile de tout projet C est une mesure préventive à coût nul
- Les linters (clang-tidy, cppcheck) détectent ce pattern statiquement
