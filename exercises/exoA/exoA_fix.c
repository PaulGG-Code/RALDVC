/*
 * Exercice A — Stack overflow avec impact logique
 * FICHIER CORRIGÉ
 *
 * Correction : strncpy() borné + null-termination explicite.
 *
 * Compilation défensive :
 *   gcc -O0 -g -Wall -Wextra -Wpedantic -fsanitize=address,undefined \
 *       -fstack-protector-all exoA_fix.c -o exoA_fix
 */

#include <stdio.h>
#include <string.h>

typedef struct {
    char         user[16];
    unsigned int role;
} session_t;

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <username>\n", argv[0]);
        return 1;
    }

    session_t s;
    s.role = 0;

    /*
     * CORRECTION 1 : strncpy avec la taille exacte du buffer.
     *   - sizeof(s.user) - 1  réserve toujours un octet pour le '\0'
     *   - L'entrée est tronquée proprement, jamais dépassée
     *
     * CORRECTION 2 : null-termination explicite.
     *   strncpy ne garantit PAS le '\0' final si len == n.
     *   On force toujours le dernier octet à '\0'.
     */
    strncpy(s.user, argv[1], sizeof(s.user) - 1);
    s.user[sizeof(s.user) - 1] = '\0';

    printf("[*] s.user = \"%s\"\n", s.user);
    printf("[*] s.role = %u (doit rester 0)\n", s.role);

    if (s.role != 0)
        puts("[-] ADMIN — INATTENDU : la correction est incomplète !");
    else
        puts("[+] USER  — rôle intact, overflow bloqué.");

    return 0;
}
