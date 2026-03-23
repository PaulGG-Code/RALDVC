# Exercice H — Migration API dangereuses → API sûres (CWE-676)

## Intention pédagogique

Exploiter pédagogiquement une dette technique classique : composition de chaînes via API non bornées. Chaque API individuelle semble « normale », mais leur composition accumule les risques jusqu'au débordement. L'exercice montre la migration complète vers des API sûres.

## Code vulnérable

```c
void make_msg(char *dst, const char *user, const char *msg) {
    char tmp[64];
    strcpy(tmp, user);      /* pas de borne */
    strcat(tmp, ": ");      /* taille cumulée non contrôlée */
    strcat(tmp, msg);       /* idem */
    sprintf(dst, "%s", tmp);/* dst peut être plus petit que tmp */
}
```

**Anatomie du risque cumulatif :**
```
tmp[64] :
  strcpy(tmp, user)     → si strlen(user) >= 64 → overflow immédiat
  strcat(tmp, ": ")     → si strlen(user) >= 62 → overflow
  strcat(tmp, msg)      → si strlen(user)+strlen(msg) >= 62 → overflow
  sprintf(dst, "%s", tmp) → si sizeof(dst) < strlen(tmp)+1 → overflow
```

## Exploitation pédagogique pas à pas

**Étape 1 — Compiler et cas nominal :**
```bash
gcc -O0 -g -Wall -Wextra -Wpedantic exoH_vuln.c -o exoH_vuln
./exoH_vuln ALICE "hello"
# → ALICE: hello
```

**Étape 2 — Déclencher le débordement cumulatif :**
```bash
./exoH_vuln AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
# A(33) + ": "(2) + B(33) = 68 octets > 64 → overflow de tmp
```

**Étape 3 — Identifier chaque API dangereuse :**

| API | Dangereuse parce que |
|-----|---------------------|
| `strcpy(dst, src)` | Pas de taille de destination |
| `strcat(dst, src)` | Pas de taille de destination |
| `sprintf(dst, fmt, ...)` | Pas de taille de destination |
| `gets(buf)` | Pas de taille (supprimée de C11) |

**Équivalents sûrs :**

| Dangereuse | Sûre |
|-----------|------|
| `strcpy` | `strncpy` + null-term, ou `snprintf(dst, n, "%s", src)` |
| `strcat` | `strncat` (taille relative !), ou `snprintf` global |
| `sprintf` | `snprintf(dst, dst_sz, ...)` |
| `gets` | `fgets(buf, sizeof(buf), stdin)` |

## Observation outillée avec ASan

```bash
gcc -O0 -g -fsanitize=address exoH_vuln.c -o exoH_vuln_asan
./exoH_vuln_asan AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
```
ASan pointe précisément le `strcat` ou le `strcpy` responsable.

## À vous de jouer

Créez `exoH_fix.c` en corrigeant la vulnérabilité dans `exoH_vuln.c`.

**Critères de réussite :**
- Compile sans avertissement avec `-Wall -Wextra -Wpedantic`
- ASan ne signale aucune erreur sur les entrées longues
- Le cas nominal (`ALICE hello`) produit la même sortie
- Les entrées qui dépassent la capacité sont gérées proprement (troncature signalée ou rejet)

**Indice :** une fonction qui écrit dans un buffer devrait recevoir la taille de ce buffer comme paramètre.
