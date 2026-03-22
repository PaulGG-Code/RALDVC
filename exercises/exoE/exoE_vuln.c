/*
 * Exercice E — Use-after-free démo sûre (CWE-416)
 * FICHIER VULNÉRABLE
 *
 * Démonstration : montrer le non-déterminisme d'un accès après libération.
 * Ce n'est pas un comportement « parfois acceptable », c'est une erreur
 * systémique à la source de nombreux CVE critiques.
 *
 * Compilation pédagogique :
 *   gcc -O0 -g -Wall -Wextra -Wpedantic exoE_vuln.c -o exoE_vuln
 */

#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int *p = malloc(sizeof(int));
    if (!p) return 1;

    *p = 42;
    printf("[*] Avant free : p = %p, *p = %d\n", (void *)p, *p);

    free(p);
    /* p est maintenant un pointeur « dangling » (suspendu).
     * La mémoire pointée appartient à nouveau à l'allocateur.
     * Toute lecture ou écriture via p est un comportement non défini. */

    printf("[*] Après free  : p = %p (adresse inchangée, mémoire libérée)\n",
           (void *)p);

    /*
     * VULNÉRABILITÉ : lecture via un pointeur dangling.
     * Le résultat peut être :
     *   - La valeur originale (42)  — si la mémoire n'a pas encore été réutilisée
     *   - Une valeur corrompue      — si glibc a écrit ses métadonnées de freelist
     *   - Un crash (SIGSEGV)        — si l'adresse est maintenant unmapped
     *
     * En exploit réel : un malloc de même taille entre free(p) et l'accès
     * suivant peut recycler exactement ce bloc, permettant de contrôler *p.
     */
    printf("[!] *p après free = %d  (comportement non défini !)\n", *p);

    return 0;
}
