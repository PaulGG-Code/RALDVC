/*
 * Exercice C — Off-by-one (CWE-193)
 * FICHIER VULNÉRABLE
 *
 * Démonstration : une erreur de borne d'un seul octet suffit à violer
 * la sûreté mémoire. L'utilisation de <= au lieu de < est l'erreur la
 * plus courante de ce type.
 *
 * Compilation pédagogique :
 *   gcc -O0 -g -Wall -Wextra -Wpedantic exoC_vuln.c -o exoC_vuln
 */

#include <stdio.h>

int main(void) {
    char buf[8] = {0};

    printf("[*] sizeof(buf) = %zu\n", sizeof(buf));
    printf("[*] Indices valides : 0..%zu\n\n", sizeof(buf) - 1);

    /*
     * VULNÉRABILITÉ : la condition i <= 8 itère 9 fois (0..8).
     * L'indice 8 pointe UN OCTET APRÈS la fin du buffer.
     * buf[8] écrit en dehors des limites allouées.
     *
     * Correction : i < 8  (ou i < sizeof(buf))
     */
    for (int i = 0; i <= 8; i++) {   /* BUG : <= au lieu de < */
        printf("[*] Écriture à buf[%d]...\n", i);
        buf[i] = 'A';                 /* buf[8] est hors limites */
    }

    puts("[?] done (un octet de trop a été écrit hors du buffer)");
    return 0;
}
