/*
 * Exercice F — Format string misuse (CWE-134)
 * FICHIER VULNÉRABLE
 *
 * Démonstration : une entrée utilisateur ne doit jamais être interprétée
 * comme chaîne de format. Même une entrée sans intention malveillante
 * peut provoquer un comportement indéfini.
 *
 * Compilation pédagogique :
 *   gcc -O0 -g -Wall -Wextra -Wpedantic exoF_vuln.c -o exoF_vuln
 *   (Note : GCC émettra -Wformat-security sur cette ligne)
 */

#include <stdio.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <message>\n", argv[0]);
        return 1;
    }

    printf("[*] Message reçu : ");

    /*
     * VULNÉRABILITÉ : argv[1] est passé comme chaîne de FORMAT, pas comme argument.
     *
     * Si argv[1] contient des directives de format :
     *   %x  → lit un entier unsigned sur la pile et l'affiche en hex
     *   %s  → lit un pointeur sur la pile et déréférence (peut crasher)
     *   %n  → ÉCRIT un entier à l'adresse du prochain argument sur la pile
     *          → écriture mémoire arbitraire !
     *
     * Mêmes effets que si on avait écrit : printf("%x %x %x")
     * mais sans déclarer d'arguments correspondants.
     */
    printf(argv[1]);   /* <-- JAMAIS en production */

    puts("");
    return 0;
}
