/*
 * Exercice F — Format string misuse
 * FICHIER CORRIGÉ
 *
 * Correction : printf("%s", argv[1]) — la chaîne de format est un littéral
 * constant, argv[1] n'est qu'un argument de valeur.
 *
 * Compilation défensive (le flag -Wformat-security détecte le bug à la compilation) :
 *   gcc -O0 -g -Wall -Wextra -Wpedantic \
 *       -Wformat -Wformat-security -Werror=format-security \
 *       exoF_fix.c -o exoF_fix
 */

#include <stdio.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <message>\n", argv[0]);
        return 1;
    }

    printf("[*] Message reçu : ");

    /*
     * CORRECTION : "%s" est un littéral de format contrôlé par le programmeur.
     * argv[1] est passé comme ARGUMENT de valeur, pas comme format.
     *
     * Même si argv[1] contient "%x %x %n", ces caractères sont traités
     * comme de simples caractères textuels par printf("%s", ...).
     * Aucune interprétation de directives de format n'a lieu.
     *
     * Règle : le premier argument de printf/fprintf/sprintf est TOUJOURS
     * un littéral de chaîne de format sous contrôle du programmeur.
     */
    printf("%s", argv[1]);   /* SAFE */

    puts("");
    return 0;
}
