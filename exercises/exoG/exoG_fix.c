/*
 * Exercice G — Mémoire non initialisée
 * FICHIER CORRIGÉ
 *
 * Correction : initialisation à zéro de toute la struct à la déclaration.
 *
 * Compilation défensive :
 *   gcc -O0 -g -Wall -Wextra -Wpedantic \
 *       -Wuninitialized -Wmaybe-uninitialized \
 *       exoG_fix.c -o exoG_fix
 */

#include <stdio.h>
#include <string.h>

typedef struct {
    int  id;
    char token[16];
} record_t;

int main(void) {
    /*
     * CORRECTION : initialisation à zéro de la struct entière.
     *
     * { 0 } ou = {0} initialise :
     *   - le premier membre explicitement à sa valeur nulle (0 pour int)
     *   - tous les membres restants à zéro implicitement (norme C99 §6.7.8)
     *
     * Avantages :
     *   - r.token[0..15] = 0x00 garantis → chaîne vide "" → comportement prévisible
     *   - Supprime toute fuite d'information résiduelle
     *   - Le compilateur peut émettre -Wuninitialized si on oublie = {0}
     */
    record_t r = {0};
    r.id = 1;

    printf("[*] r.id    = %d\n", r.id);
    printf("[+] r.token = \"%s\" (initialisé à zéro — chaîne vide)\n", r.token);

    printf("[+] r.token hex : ");
    for (size_t i = 0; i < sizeof(r.token); i++) {
        printf("%02x ", (unsigned char)r.token[i]);
    }
    puts("(tous zéros)");

    return 0;
}
