# Exercice I — Double Free (CWE-415)

**Difficulté :** Intermédiaire

## Intention pédagogique

Montrer comment une double libération survient lorsque deux chemins d'exécution différents appellent `free()` sur le même pointeur — sans que l'un sache que l'autre l'a déjà fait. C'est un scénario classique dans la gestion d'erreurs avec `goto` ou avec des fonctions d'assistance qui libèrent leurs arguments.

## Code vulnérable

```c
void node_destroy(Node *n) {
    free(n->data);
    free(n);
}

int node_process(Node *n, int error) {
    if (error) {
        node_destroy(n);   /* PREMIÈRE LIBÉRATION */
        return -1;
    }
    return 0;
}

int main(void) {
    Node *n = node_create(1, "hello");
    int ret = node_process(n, 1);

    if (ret != 0) {
        node_destroy(n);   /* DEUXIÈME LIBÉRATION — double-free */
    }
}
```

**Ce qui se passe lors du double-free :**
```
glibc détecte que le bloc n est déjà dans la freelist tcache
→ SIGABRT (assertion interne de l'allocateur)
→ ou corruption silencieuse des métadonnées du tas
→ en exploit réel : contrôle du pointeur fd de tcache → write-what-where primitif
```

## Exploitation pédagogique pas à pas

**Étape 1 — Compiler et observer le crash :**
```bash
gcc -O0 -g -Wall -Wextra -Wpedantic exoI_vuln.c -o exoI_vuln
./exoI_vuln
```
Le comportement varie : crash immédiat avec `SIGABRT`, corruption silencieuse, ou dans certains builds double-free détecté par glibc avec un message `double free or corruption`.

**Étape 2 — Détecter avec ASan :**
```bash
gcc -O0 -g -fsanitize=address exoI_vuln.c -o exoI_vuln_asan
./exoI_vuln_asan
```
ASan signale immédiatement :
```
ERROR: AddressSanitizer: attempting double-free on 0x...
    #0 free (...)
    #1 node_destroy (exoI_vuln.c:41)
    #2 main (exoI_vuln.c:66)
previously freed by thread T0 here:
    #0 free (...)
    #1 node_destroy (exoI_vuln.c:41)
    #2 node_process (exoI_vuln.c:53)
```
ASan montre les **deux sites de libération** — c'est une information impossible à obtenir autrement au runtime.

**Étape 3 — Inspecter avec GDB :**
```bash
gdb -q ./exoI_vuln
```
```
(gdb) break node_destroy
(gdb) run
# Premier arrêt : libération légitime dans node_process
(gdb) print n          # adresse valide
(gdb) continue
# Deuxième arrêt : même adresse — déjà libérée
(gdb) print n          # même valeur → problème
(gdb) bt               # backtrace : on voit main appelant node_destroy directement
```

## Observation outillée avec ASan

```bash
gcc -O0 -g -fsanitize=address exoI_vuln.c -o exoI_vuln_asan
./exoI_vuln_asan 2>&1 | head -30
```
Sortie attendue :
```
[process] Traitement du nœud 1 : 'hello'
[-] Erreur de traitement — libération dans node_process
[-] node_process a échoué — nettoyage en cours...
=================================================================
==...==ERROR: AddressSanitizer: attempting double-free on 0x...
```

## À vous de jouer

Créez `exoI_fix.c` en corrigeant la vulnérabilité dans `exoI_vuln.c`.

**Critères de réussite :**
- Compile sans avertissement avec `-Wall -Wextra -Wpedantic`
- ASan ne signale aucune erreur `double-free`
- Le programme se termine proprement sans SIGABRT
- La mémoire est libérée exactement une fois, même sur le chemin d'erreur

**Indice :** comment rendre le deuxième appel à `node_destroy` structurellement inoffensif, quelle que soit la séquence d'appel ?
