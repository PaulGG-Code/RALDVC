/*
 * Exercice H — Migration API dangereuses → API sûres (CWE-676)
 * FICHIER VULNÉRABLE
 *
 * Démonstration : une dette technique classique — composition de chaînes
 * via API non bornées (strcpy, strcat, sprintf) cumulant les risques.
 *
 * Compilation pédagogique :
 *   gcc -O0 -g -Wall -Wextra -Wpedantic exoH_vuln.c -o exoH_vuln
 */

#include <stdio.h>
#include <string.h>

/*
 * Construit un message formaté dans dst.
 * VULNÉRABLE : aucune des trois opérations ne vérifie la taille de dst.
 *
 * Taille de tmp = 64 octets.
 * Contenu final : user + ": " + msg
 * Si strlen(user) + strlen(msg) > 62, tmp déborde.
 *
 * dst n'a pas de taille connue ici — sprintf(dst, ...) ne peut pas
 * la protéger même si tmp était correct.
 */
void make_msg(char *dst, const char *user, const char *msg) {
    char tmp[64];

    strcpy(tmp, user);      /* BUG 1 : pas de borne sur user */
    strcat(tmp, ": ");      /* BUG 2 : pas de contrôle de la taille cumulée */
    strcat(tmp, msg);       /* BUG 3 : idem pour msg */
    sprintf(dst, "%s", tmp);/* BUG 4 : dst peut être plus petit que tmp */
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <user> <message>\n", argv[0]);
        return 1;
    }

    char output[128];
    make_msg(output, argv[1], argv[2]);
    puts(output);
    return 0;
}
