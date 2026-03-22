/*
 * Exercice D — Integer overflow de taille (CWE-190)
 * FICHIER VULNÉRABLE
 *
 * Démonstration : une erreur arithmétique peut invalider une allocation
 * sans crash immédiat, préparant une corruption ultérieure.
 *
 * Compilation pédagogique :
 *   gcc -O0 -g -Wall -Wextra -Wpedantic exoD_vuln.c -o exoD_vuln
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(void) {
    /*
     * VULNÉRABILITÉ : n * 8 avec n = 2^30 donne 2^33 = 8 589 934 592.
     * Ce résultat dépasse UINT32_MAX (4 294 967 295).
     * En arithmétique uint32_t, 8 589 934 592 mod 2^32 = 0.
     *
     * malloc(0) selon la norme C retourne soit NULL soit un pointeur unique
     * non déréférençable. Dans tous les cas, l'allocation est bien plus
     * petite que prévu — toute écriture ultérieure déborde.
     */
    uint32_t n     = 1U << 30;          /* 1 073 741 824       */
    uint32_t total = n * 8;             /* overflow → 0 (!)    */

    printf("[*] n         = %u (2^30)\n", n);
    printf("[*] n * 8     = %u (attendu: %llu, obtenu après overflow: %u)\n",
           total, (unsigned long long)n * 8ULL, total);
    printf("[*] Différence = %llu octets non alloués\n\n",
           (unsigned long long)n * 8ULL - total);

    char *p = malloc(total);
    if (!p) {
        printf("[!] malloc(%u) a retourné NULL.\n", total);
        return 1;
    }

    printf("[*] malloc(%u) a réussi — mais le buffer est trop petit !\n", total);
    printf("[*] Toute écriture de plus de %u octets déborde.\n", total);

    /* En production, le code écrirait ici n*8 octets → débordement de tas */

    free(p);
    return 0;
}
