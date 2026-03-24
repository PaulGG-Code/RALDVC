/*
 * Exercice I — Double Free
 * FICHIER CORRIGÉ
 *
 * Corrections :
 *   1. node_destroy reçoit un pointeur sur pointeur (Node **) pour
 *      invalider le pointeur chez l'appelant après libération (→ NULL).
 *   2. node_destroy vérifie NULL en entrée — free(NULL) est un no-op,
 *      un double-free accidentel via le chemin de nettoyage devient inoffensif.
 *   3. L'appelant utilise &n, ce qui garantit que n vaut NULL après la
 *      première libération — la deuxième libération n'a plus aucun effet.
 *
 * Compilation défensive :
 *   gcc -O0 -g -Wall -Wextra -Wpedantic -fsanitize=address \
 *       exoI_fix.c -o exoI_fix
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *data;
    int   id;
} Node;

Node *node_create(int id, const char *text) {
    Node *n = malloc(sizeof(Node));
    if (!n) return NULL;
    n->data = malloc(strlen(text) + 1);
    if (!n->data) { free(n); return NULL; }
    strcpy(n->data, text);
    n->id = id;
    return n;
}

/*
 * FIX : prend Node ** pour pouvoir écrire *n = NULL après libération.
 *
 *   free(NULL) est garanti sans effet par la norme C (C11 §7.22.3.3).
 *   → Appeler node_destroy(&ptr) deux fois est toujours sûr :
 *     - Premier appel : libère et met *ptr = NULL
 *     - Deuxième appel : *ptr == NULL → retour immédiat, rien de libéré
 */
void node_destroy(Node **n) {
    if (!n || !*n) return;   /* NULL-safe : no-op si déjà libéré */

    free((*n)->data);
    (*n)->data = NULL;       /* invalider le pointeur interne aussi */

    free(*n);
    *n = NULL;               /* invalider le pointeur chez l'appelant */
}

int node_process(Node **n, int simulate_error) {
    printf("[process] Traitement du nœud %d : '%s'\n", (*n)->id, (*n)->data);

    if (simulate_error) {
        fprintf(stderr, "[-] Erreur de traitement — libération dans node_process\n");
        node_destroy(n);   /* libère et met *n = NULL */
        return -1;
    }

    printf("[process] Succès.\n");
    return 0;
}

int main(void) {
    Node *n = node_create(1, "hello");
    if (!n) return 1;

    printf("[*] Nœud alloué : n = %p, n->data = %p\n",
           (void *)n, (void *)n->data);

    int ret = node_process(&n, 1);  /* node_process met n = NULL en cas d'erreur */

    if (ret != 0) {
        fprintf(stderr, "[-] node_process a échoué — nettoyage en cours...\n");
        /*
         * FIX : n vaut NULL ici (node_process l'a mis à NULL).
         * node_destroy voit *n == NULL → retour immédiat, aucune libération.
         * Pas de double-free.
         */
        node_destroy(&n);  /* no-op : n == NULL */
        printf("[+] Nettoyage sûr : n = %p (NULL, aucune double-libération)\n",
               (void *)n);
    }

    return 0;
}
