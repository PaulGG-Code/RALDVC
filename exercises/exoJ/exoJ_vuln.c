/*
 * Exercice J — Injection de commande (CWE-78)
 * FICHIER VULNÉRABLE
 *
 * Utilitaire de comptage de mots : affiche le nombre de mots dans un fichier.
 * Le nom du fichier fourni par l'utilisateur est inséré directement dans
 * une commande shell exécutée via system().
 *
 * system(cmd) = /bin/sh -c cmd
 *
 * Le shell interprète les métacaractères présents dans cmd :
 *   ;    — séparateur de commandes
 *   &&   — commande suivante si la précédente réussit
 *   ||   — commande suivante si la précédente échoue
 *   |    — pipe
 *   `…`  ou $(…) — substitution de commande
 *
 * Exemple d'injection :
 *   ./exoJ_vuln "notes.txt; whoami"
 *   → exécute : wc -w notes.txt; whoami
 *
 * Compilation pédagogique :
 *   gcc -O0 -g -Wall -Wextra -Wpedantic exoJ_vuln.c -o exoJ_vuln
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CMD_SIZE 256

void count_words(const char *filename) {
    char cmd[CMD_SIZE];

    /*
     * VULNÉRABILITÉ : filename est interpolé dans une chaîne shell.
     * system() délègue l'exécution à /bin/sh -c, qui interprète
     * les métacaractères shell présents dans filename.
     *
     * Si filename = "notes.txt; echo INJECTED",
     * le shell exécute :
     *   1. wc -w notes.txt
     *   2. echo INJECTED   ← commande injectée
     */
    snprintf(cmd, sizeof(cmd), "wc -w %s", filename);

    printf("[*] Commande : %s\n\n", cmd);

    system(cmd);  /* JAMAIS utiliser system() avec une entrée utilisateur */
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <fichier>\n", argv[0]);
        return 1;
    }

    printf("=== Comptage de mots ===\n");
    count_words(argv[1]);
    return 0;
}
