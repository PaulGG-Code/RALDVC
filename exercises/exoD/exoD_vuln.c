/*
 * Exercice D — Integer overflow de taille (CWE-190)
 * FICHIER VULNÉRABLE
 *
 * Démonstration en deux temps :
 *   1. L'overflow arithmétique silencieux produit une taille invalide
 *   2. L'écriture dans le buffer trop petit déclenche un heap-buffer-overflow
 *      détectable par ASan
 *
 * Note sur UBSan : l'overflow d'entiers NON SIGNÉS (uint32_t) est un
 * comportement DÉFINI en C (wrapping modulo 2^32) — UBSan ne le signale
 * pas. Seul l'overflow d'entiers signés est du comportement INDÉFINI et
 * capturé par UBSan. C'est pourquoi on utilise ASan ici pour montrer la
 * conséquence (heap overflow), pas la cause arithmétique.
 *
 * Compilation pédagogique :
 *   gcc -O0 -g -Wall -Wextra -Wpedantic exoD_vuln.c -o exoD_vuln
 * Avec ASan (montre le heap overflow résultant) :
 *   gcc -O0 -g -fsanitize=address exoD_vuln.c -o exoD_vuln_asan
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

int main(void) {
    /*
     * VULNÉRABILITÉ PART 1 — Overflow arithmétique silencieux
     *
     * n = 2^30 = 1 073 741 824
     * n * 8   = 8 589 934 592  > UINT32_MAX (4 294 967 295)
     * Résultat en uint32_t : 8 589 934 592 mod 2^32 = 0
     *
     * malloc(0) retourne soit NULL, soit un pointeur unique de taille 0
     * ou 1 octet selon l'implémentation. Dans tous les cas : TROP PETIT.
     */
    uint32_t n     = 1U << 30;
    uint32_t total = n * 8;     /* overflow silencieux → 0 */

    printf("[*] n           = %u  (2^30 = %u)\n", n, 1U << 30);
    printf("[*] n * 8       = %u  ← valeur après overflow !\n", total);
    printf("[*] Vraie valeur = %llu\n", (unsigned long long)n * 8ULL);
    printf("[*] Différence   = %llu octets NON alloués\n\n",
           (unsigned long long)n * 8ULL - total);

    char *buf = malloc(total);   /* malloc(0) ou malloc(très_petit) */
    if (!buf) {
        printf("[!] malloc(%u) a retourné NULL.\n", total);
        return 1;
    }
    printf("[*] malloc(%u) a alloué ~%u octets à %p\n", total, total, (void *)buf);

    /*
     * VULNÉRABILITÉ PART 2 — Écriture hors limites résultante
     *
     * Le code suppose avoir alloué n*8 octets mais le buffer est minuscule.
     * Même écrire 32 octets déborde massivement.
     * ASan détecte cette corruption heap.
     */
    printf("[*] Écriture de 32 octets dans un buffer de %u octets...\n", total);
    memset(buf, 'A', 32);   /* heap-buffer-overflow → ASan le détecte */
    printf("[?] Écriture apparemment réussie (comportement non défini).\n");

    free(buf);
    return 0;
}
