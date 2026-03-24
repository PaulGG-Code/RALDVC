/*
 * Exercice H — Migration API dangereuses → API sûres
 * FICHIER CORRIGÉ
 *
 * Corrections :
 *   1. make_msg prend dst_sz en paramètre (la taille de dst)
 *   2. snprintf() construit directement dans dst avec une borne absolue
 *   3. Vérification du retour pour détecter une troncature
 *   4. Signature documentée, pas d'ambiguïté sur la propriété de la taille
 *
 * Compilation défensive :
 *   gcc -O0 -g -Wall -Wextra -Wpedantic -fsanitize=address,undefined \
 *       exoH_fix.c -o exoH_fix
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
 * CORRECTION : make_msg reçoit dst et dst_sz — elle connaît sa limite.
 *
 * Retourne 0 si le message est complet, -1 en cas d'erreur, 1 si tronqué.
 *
 * Contraste avec la version vulnérable :
 *   strcpy/strcat → snprintf   (opération unique, bornée, non-cumulative)
 *   pas de tmp intermédiaire   (moins de copies = moins de surface d'attaque)
 *   retour de code             (le caller peut réagir à une troncature)
 */
int make_msg(char *dst, size_t dst_sz, const char *user, const char *msg) {
    if (!dst || dst_sz == 0) return -1;

    /*
     * snprintf(dst, dst_sz, fmt, ...) :
     *   - Écrit au plus dst_sz-1 octets + '\0' terminal
     *   - Retourne le nombre d'octets qui AURAIENT été écrits (sans '\0')
     *     si la taille était infinie
     *   → ret >= dst_sz signifie troncature
     */
    int ret = snprintf(dst, dst_sz, "%s: %s", user, msg);

    if (ret < 0)                   return -1;  /* erreur d'encodage */
    if ((size_t)ret >= dst_sz)     return 1;   /* tronqué */
    return 0;                                  /* succès complet */
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <user> <message>\n", argv[0]);
        return 1;
    }

    char output[128];
    int status = make_msg(output, sizeof(output), argv[1], argv[2]);

    switch (status) {
        case 0:  puts(output); break;
        case 1:  fprintf(stderr, "[!] Message tronqué : %s\n", output); break;
        default: fprintf(stderr, "[-] Erreur make_msg.\n"); return 1;
    }

    return 0;
}
