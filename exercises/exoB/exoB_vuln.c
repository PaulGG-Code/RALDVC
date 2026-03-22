/*
 * Exercice B — Overflow cumulatif par concaténation (CWE-121)
 * FICHIER VULNÉRABLE
 *
 * Démonstration : une faille peut émerger d'une accumulation d'écritures
 * apparemment « raisonnables » si la taille finale n'est jamais contrôlée.
 *
 * Compilation pédagogique :
 *   gcc -O0 -g -Wall -Wextra -Wpedantic exoB_vuln.c -o exoB_vuln
 */

#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <user> <message>\n", argv[0]);
        return 1;
    }

    char out[64] = "User: ";

    printf("[*] Buffer initial : \"%s\" (%zu octets)\n", out, strlen(out));
    printf("[*] Capacité restante : %zu octets\n", sizeof(out) - strlen(out) - 1);
    printf("[*] Longueur user    : %zu\n", strlen(argv[1]));
    printf("[*] Longueur message : %zu\n", strlen(argv[2]));
    printf("[*] Total prévu      : %zu\n\n",
           strlen("User: ") + strlen(argv[1]) + strlen(" | Msg: ") + strlen(argv[2]));

    /*
     * VULNÉRABILITÉ : chaque strcat vérifie individuellement que sa source
     * « tient dans quelque chose », mais aucun ne contrôle la taille CUMULÉE.
     *
     * "User: "   = 6
     * argv[1]    = variable
     * " | Msg: " = 8
     * argv[2]    = variable
     * Total peut dépasser 64 sans avertissement.
     */
    strcat(out, argv[1]);       /* 1er débordement potentiel */
    strcat(out, " | Msg: ");    /* 2e débordement potentiel */
    strcat(out, argv[2]);       /* 3e débordement potentiel */

    puts(out);
    return 0;
}
