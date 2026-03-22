/*
 * Exercice C — Off-by-one
 * FICHIER CORRIGÉ
 *
 * Correction : utiliser i < sizeof(buf) (strict) au lieu de i <= sizeof(buf).
 *
 * Règle mémo : un tableau de N éléments a des indices 0..N-1 (N indices valides).
 * La condition d'arrêt doit être STRICTEMENT INFÉRIEURE à N.
 *
 * Compilation défensive :
 *   gcc -O0 -g -Wall -Wextra -Wpedantic -fsanitize=address,undefined \
 *       exoC_fix.c -o exoC_fix
 */

#include <stdio.h>
#include <string.h>

int main(void) {
    char buf[8] = {0};

    printf("[*] sizeof(buf) = %zu\n", sizeof(buf));

    /*
     * CORRECTION : i < sizeof(buf) — itère exactement 8 fois (0..7).
     * Le dernier indice valide est 7 = sizeof(buf) - 1.
     *
     * Utiliser sizeof(buf) au lieu du littéral 8 est aussi plus robuste :
     * si la taille du buffer change, la condition reste correcte.
     */
    for (size_t i = 0; i < sizeof(buf); i++) {
        buf[i] = 'A';
    }

    printf("[+] done — %zu octets écrits, tous dans les limites.\n", sizeof(buf));
    return 0;
}
