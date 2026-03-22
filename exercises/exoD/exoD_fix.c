/*
 * Exercice D — Integer overflow de taille
 * FICHIER CORRIGÉ
 *
 * Corrections :
 *   1. Utiliser size_t (64 bits sur x86-64) au lieu de uint32_t pour les tailles
 *   2. Vérifier l'overflow AVANT la multiplication avec __builtin_mul_overflow
 *   3. Vérifier que la taille est dans un intervalle raisonnable
 *
 * Compilation défensive :
 *   gcc -O0 -g -Wall -Wextra -Wpedantic -fsanitize=undefined \
 *       exoD_fix.c -o exoD_fix
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>

/* Taille maximale acceptable pour une allocation (1 Go) */
#define MAX_ALLOC ((size_t)1 << 30)

int main(void) {
    size_t n = (size_t)1 << 30;   /* FIX 1 : size_t, pas uint32_t */

    /*
     * FIX 2 : __builtin_mul_overflow(a, b, &result)
     *   - Calcule a * b en arithmétique exacte
     *   - Stocke le résultat dans result
     *   - Retourne 1 (vrai) si un overflow s'est produit, 0 sinon
     *
     * Disponible depuis GCC 5 et Clang 3.8 sur toutes les plateformes.
     */
    size_t total;
    if (__builtin_mul_overflow(n, (size_t)8, &total)) {
        fprintf(stderr, "[-] Overflow détecté dans n * 8 — allocation refusée.\n");
        return 1;
    }

    /*
     * FIX 3 : borne supérieure explicite.
     * Même sans overflow arithmétique, une allocation de plusieurs Go
     * peut être une anomalie applicative à rejeter.
     */
    if (total > MAX_ALLOC) {
        fprintf(stderr, "[-] Taille trop grande (%zu > %zu) — refusé.\n",
                total, MAX_ALLOC);
        return 1;
    }

    printf("[+] n     = %zu\n", n);
    printf("[+] total = %zu (valeur correcte avant allocation)\n", total);

    /* Dans cet exercice, on ne fait pas réellement l'allocation de 8 Go */
    printf("[+] Validation réussie. (Allocation non effectuée dans cet exercice.)\n");
    return 0;
}
