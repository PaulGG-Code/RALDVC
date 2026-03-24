/*
 * Exercice I — Double Free (CWE-415)
 * FICHIER VULNÉRABLE
 *
 * Démonstration : montrer comment une double libération survient
 * lorsqu'un pointeur est libéré sur deux chemins d'exécution différents.
 *
 * Scénario : nœud de liste chaînée libéré une fois via le chemin d'erreur,
 * puis une seconde fois via le chemin de nettoyage. Les deux chemins
 * atteignent free() sur le même pointeur sans que l'un sache que l'autre
 * a déjà libéré.
 *
 * Compilation pédagogique :
 *   gcc -O0 -g -Wall -Wextra -Wpedantic exoI_vuln.c -o exoI_vuln
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
 * Libère un nœud et ses ressources internes.
 * Appelée depuis deux endroits différents — sans coordination.
 */
void node_destroy(Node *n) {
    if (!n) return;
    free(n->data);   /* libère la chaîne */
    free(n);         /* libère le nœud  */
}

/*
 * Simule un traitement qui peut échouer.
 * En cas d'échec, libère n avant de retourner.
 * PROBLÈME : l'appelant libère aussi n dans son bloc de nettoyage.
 */
int node_process(Node *n, int simulate_error) {
    printf("[process] Traitement du nœud %d : '%s'\n", n->id, n->data);

    if (simulate_error) {
        fprintf(stderr, "[-] Erreur de traitement — libération dans node_process\n");
        node_destroy(n);   /* PREMIÈRE LIBÉRATION */
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

    int ret = node_process(n, 1);  /* déclenche le chemin d'erreur */

    if (ret != 0) {
        fprintf(stderr, "[-] node_process a échoué — nettoyage en cours...\n");
        /*
         * VULNÉRABILITÉ : node_destroy a déjà libéré n dans node_process.
         * Cette deuxième libération corrompt les métadonnées du tas (heap).
         * Comportement : crash, SIGABRT, ou corruption silencieuse.
         */
        node_destroy(n);  /* DEUXIÈME LIBÉRATION — double-free */
    }

    return 0;
}
