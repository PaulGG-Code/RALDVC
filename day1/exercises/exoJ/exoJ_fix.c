/*
 * Exercice J — Injection de commande
 * FICHIER CORRIGÉ
 *
 * Correction : remplacer system() par fork() + execvp().
 *
 * execvp(path, argv[]) :
 *   - Lance directement l'exécutable (ici : /usr/bin/wc)
 *   - Passe les arguments comme un tableau de chaînes séparées
 *   - N'invoque JAMAIS un shell intermédiaire
 *   - Les métacaractères shell dans argv[1] sont traités comme du texte
 *     littéral par wc — pas d'interprétation
 *
 * Exemple :
 *   ./exoJ_fix "notes.txt; echo INJECTED"
 *   → wc reçoit le nom de fichier littéral "notes.txt; echo INJECTED"
 *   → wc échoue (fichier introuvable) mais echo n'est JAMAIS exécuté
 *
 * Compilation défensive :
 *   gcc -O0 -g -Wall -Wextra -Wpedantic exoJ_fix.c -o exoJ_fix
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void count_words(const char *filename) {
    printf("=== Comptage de mots (version sûre) ===\n");

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return;
    }

    if (pid == 0) {
        /*
         * FIX : execvp passe les arguments directement au programme,
         * sans passer par un shell.
         *
         * argv transmis à wc :
         *   argv[0] = "wc"       (nom du programme)
         *   argv[1] = "-w"       (option)
         *   argv[2] = "--"       (fin des options : protège les noms commençant par -)
         *   argv[3] = filename   (valeur littérale, métacaractères non interprétés)
         *   argv[4] = NULL       (marqueur de fin)
         *
         * Même si filename = "notes.txt; echo INJECTED",
         * wc voit un seul argument : la chaîne "notes.txt; echo INJECTED".
         * Le shell n'est jamais impliqué.
         */
        execlp("wc", "wc", "-w", "--", filename, (char *)NULL);

        /* execlp ne retourne que si elle échoue */
        perror("execlp");
        _exit(1);
    }

    /* Processus parent : attendre la fin de wc */
    int status;
    waitpid(pid, &status, 0);

    if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
        fprintf(stderr, "[-] wc a échoué (code %d) — fichier introuvable ?\n",
                WEXITSTATUS(status));
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <fichier>\n", argv[0]);
        return 1;
    }

    count_words(argv[1]);
    return 0;
}
