/*
 * Exercice A — Stack overflow avec impact logique
 * FICHIER VULNÉRABLE
 *
 * Démonstration : un dépassement de tampon peut corrompre une variable
 * de rôle adjacente sur la pile, contournant une décision d'autorisation
 * sans jamais avoir à exécuter de shellcode.
 *
 * Compilation pédagogique (sans protections) :
 *   gcc -O0 -g -Wall -Wextra -Wpedantic -fno-stack-protector -no-pie \
 *       exoA_vuln.c -o exoA_vuln
 */

#include <stdio.h>
#include <string.h>

typedef struct {
    char         user[16];  /* buffer de 16 octets */
    unsigned int role;      /* 0 = USER, !=0 = ADMIN — cible de la corruption */
} session_t;

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <username>\n", argv[0]);
        return 1;
    }

    session_t s;
    s.role = 0;  /* rôle initial : utilisateur normal */

    printf("[*] Avant strcpy :\n");
    printf("    &s.user = %p\n", (void *)s.user);
    printf("    &s.role = %p\n", (void *)&s.role);
    printf("    s.role  = %u\n", s.role);
    printf("    distance user→role : %td octets\n\n",
           (char *)&s.role - (char *)s.user);

    /*
     * VULNÉRABILITÉ : strcpy() ne vérifie pas la taille de la destination.
     * Si strlen(argv[1]) >= 16, les octets supplémentaires écrasent s.role.
     * Sur les architectures little-endian, l'octet 0x41 ('A') écrit dans
     * s.role[0] le fait passer à une valeur non-nulle → rôle ADMIN.
     */
    strcpy(s.user, argv[1]);  /* <-- JAMAIS en production */

    printf("[*] Après strcpy :\n");
    printf("    s.user  = \"%.20s\"\n", s.user);
    printf("    s.role  = %u (0x%X)\n", s.role, s.role);
    printf("\n");

    if (s.role != 0)
        puts("[!] ADMIN — rôle corrompu par dépassement de tampon !");
    else
        puts("[+] USER  — rôle intact.");

    return 0;
}
