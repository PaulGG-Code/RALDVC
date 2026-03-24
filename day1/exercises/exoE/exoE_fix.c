/*
 * Exercice E — Use-after-free
 * FICHIER CORRIGÉ
 *
 * Corrections :
 *   1. Supprimer toute lecture/écriture post-libération
 *   2. Invalider le pointeur immédiatement après free (p = NULL)
 *   3. Vérifier NULL avant tout déréférencement
 *
 * Compilation défensive :
 *   gcc -O0 -g -Wall -Wextra -Wpedantic -fsanitize=address \
 *       exoE_fix.c -o exoE_fix
 */

#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int *p = malloc(sizeof(int));
    if (!p) return 1;

    *p = 42;
    printf("[*] Avant free : *p = %d\n", *p);

    free(p);

    /*
     * FIX 1 : invalider le pointeur immédiatement après free.
     *
     * Avantages :
     *   a) Toute tentative d'utilisation ultérieure de p causa un SIGSEGV
     *      immédiatement identifiable (déréférencement de NULL)
     *      plutôt qu'une corruption silencieuse.
     *   b) free(NULL) est une opération sans effet — un double-free
     *      accidentel sur p devient inoffensif.
     */
    p = NULL;

    printf("[*] Après free : p = %p (NULL, invalide)\n", (void *)p);

    /*
     * FIX 2 : vérifier NULL avant tout déréférencement.
     * Ce code est maintenant sûr — il ne déréférence jamais p après free.
     */
    if (p != NULL) {
        printf("[?] *p = %d\n", *p);
    } else {
        printf("[+] p est NULL — accès bloqué, pas d'use-after-free.\n");
    }

    return 0;
}
