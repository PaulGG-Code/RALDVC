/*
 * Exercice B — Overflow cumulatif par concaténation
 * FICHIER CORRIGÉ
 *
 * Correction : snprintf() construit toute la chaîne en une seule opération
 * bornée. Une seule limite, un seul contrôle, pas d'accumulation possible.
 *
 * Compilation défensive :
 *   gcc -O0 -g -Wall -Wextra -Wpedantic -fsanitize=address,undefined \
 *       exoB_fix.c -o exoB_fix
 */

#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <user> <message>\n", argv[0]);
        return 1;
    }

    char out[64];

    /*
     * CORRECTION : snprintf(dst, size, fmt, ...)
     *   - Écrit au plus size-1 caractères utiles + '\0' final
     *   - Retourne le nombre de caractères qui AURAIENT été écrits
     *     si la taille était illimitée → permet de détecter la troncature
     *   - Un seul appel remplace trois strcat indépendants
     */
    int written = snprintf(out, sizeof(out), "User: %s | Msg: %s",
                           argv[1], argv[2]);

    if (written < 0) {
        fprintf(stderr, "[-] Erreur d'encodage snprintf.\n");
        return 1;
    }
    if ((size_t)written >= sizeof(out)) {
        fprintf(stderr, "[!] Message tronqué (%d → %zu octets).\n",
                written, sizeof(out) - 1);
    }

    puts(out);
    return 0;
}
