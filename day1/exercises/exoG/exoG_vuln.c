/*
 * Exercice G — Mémoire non initialisée (CWE-457)
 * FICHIER VULNÉRABLE
 *
 * Démonstration : une fuite d'information locale due à un état non initialisé.
 * Le champ `token` d'un enregistrement contient des résidus de pile imprévisibles.
 *
 * Compilation pédagogique :
 *   gcc -O0 -g -Wall -Wextra -Wpedantic exoG_vuln.c -o exoG_vuln
 */

#include <stdio.h>
#include <string.h>

typedef struct {
    int  id;
    char token[16];
} record_t;

int main(void) {
    /*
     * VULNÉRABILITÉ : record_t r est alloué sur la pile sans initialisation.
     *
     * r.token[0..15] contient ce qui se trouvait à ces adresses pile avant
     * l'appel de main() : données du loader, variables d'environnement,
     * résidus d'autres fonctions, etc.
     *
     * Seul r.id est initialisé — r.token n'est jamais écrit avant la lecture.
     */
    record_t r;
    r.id = 1;
    /* r.token n'est PAS initialisé */

    printf("[*] r.id    = %d\n", r.id);

    /*
     * La sortie de token est non fiable :
     *   - peut contenir des octets nuls → affichage vide
     *   - peut contenir des résidus de la pile → fuite de données
     *   - peut varier d'une exécution à l'autre ou d'une machine à l'autre
     */
    printf("[!] r.token = \"%.*s\" (contenu non défini !)\n",
           (int)sizeof(r.token), r.token);

    /* Affichage hexadécimal pour voir les octets bruts */
    printf("[!] r.token hex : ");
    for (size_t i = 0; i < sizeof(r.token); i++) {
        printf("%02x ", (unsigned char)r.token[i]);
    }
    puts("");

    return 0;
}
