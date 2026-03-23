# Exercice J — Injection de commande (CWE-78)

**Difficulté :** Intermédiaire

## Intention pédagogique

Montrer comment `system()` délègue l'exécution à un shell intermédiaire, ce qui permet à un attaquant d'injecter des commandes arbitraires via des métacaractères shell. Démontrer pourquoi `execvp()` + `fork()` est la correction canonique : les arguments sont transmis directement au programme, sans shell.

## Code vulnérable

```c
void count_words(const char *filename) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "wc -w %s", filename);
    system(cmd);  /* /bin/sh -c "wc -w <filename>" */
}
```

**Ce que system() fait en interne :**
```
system(cmd)
  → fork()
  → execve("/bin/sh", ["/bin/sh", "-c", cmd], envp)
  → /bin/sh interprète les métacaractères dans cmd
```

**Injection avec `;` :**
```
filename = "notes.txt; echo INJECTED"
cmd      = "wc -w notes.txt; echo INJECTED"
/bin/sh exécute :
  1. wc -w notes.txt
  2. echo INJECTED   ← commande arbitraire
```

## Exploitation pédagogique pas à pas

**Étape 1 — Compiler et tester le cas nominal :**
```bash
gcc -O0 -g -Wall -Wextra -Wpedantic exoJ_vuln.c -o exoJ_vuln
./exoJ_vuln /etc/hostname
```
Sortie attendue :
```
=== Comptage de mots ===
[*] Commande : wc -w /etc/hostname

1 /etc/hostname
```

**Étape 2 — Injecter une commande avec `;` :**
```bash
./exoJ_vuln "/etc/hostname; echo INJECTED_CMD"
```
Sortie attendue :
```
=== Comptage de mots ===
[*] Commande : wc -w /etc/hostname; echo INJECTED_CMD

1 /etc/hostname
INJECTED_CMD
```
`echo INJECTED_CMD` s'exécute car `/bin/sh` interprète le `;` comme un séparateur de commandes.

**Étape 3 — Autres vecteurs d'injection :**
```bash
# Pipe : envoyer la sortie de wc vers une autre commande
./exoJ_vuln "/etc/hostname | cat"

# Substitution de commande
./exoJ_vuln '/etc/hostname; id'

# Redirection (écriture de fichier)
./exoJ_vuln '/etc/hostname; echo pwned > /tmp/test_injection.txt'
cat /tmp/test_injection.txt
```

**Étape 4 — Inspecter ce que le shell reçoit :**
```bash
# Remplacer system(cmd) par strace pour voir execve
strace -e execve ./exoJ_vuln "/etc/hostname; id" 2>&1 | grep execve
```
On voit `/bin/sh` invoqué avec la chaîne entière — le shell interprète tout.

## À vous de jouer

Créez `exoJ_fix.c` en corrigeant la vulnérabilité dans `exoJ_vuln.c`.

**Critères de réussite :**
- Compile sans avertissement avec `-Wall -Wextra -Wpedantic`
- `./exoJ_fix "/etc/hostname; echo INJECTED_CMD"` n'exécute **pas** `echo INJECTED_CMD`
- Le cas nominal (`./exoJ_fix /etc/hostname`) fonctionne toujours correctement

**Indice :** comment exécuter `wc` sans passer par un shell intermédiaire ?
