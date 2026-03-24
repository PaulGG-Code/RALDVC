/*
 * Module 09 — Command Injection (CWE-78)
 * PATCHED VERSION
 *
 * Fixes applied:
 *   1. Replace system() with execve() — runs the binary directly,
 *      NO shell is invoked, NO metacharacter interpretation
 *   2. Validate filename: only allow [a-zA-Z0-9._-] (whitelist)
 *   3. Constructed backup path stays within BACKUP_DIR (no path traversal)
 *
 * Compile with: gcc -g -O0 -fstack-protector-all -D_FORTIFY_SOURCE=2 -pie
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <errno.h>

#define BACKUP_DIR "/tmp/backups"
#define MAX_PATH   512

/*
 * FIX 1: Strict filename whitelist.
 * Only allow characters that are safe in a filename.
 * Reject anything with shell metacharacters.
 */
static int validate_filename(const char *filename) {
    if (filename == NULL || filename[0] == '\0') {
        fprintf(stderr, "[-] Empty filename\n");
        return 0;
    }
    /* Reject path separators and shell metacharacters */
    for (const char *p = filename; *p; p++) {
        if (!isalnum((unsigned char)*p) &&
            *p != '.' && *p != '_' && *p != '-') {
            fprintf(stderr,
                "[-] Invalid character '%c' in filename — only [a-zA-Z0-9._-] allowed\n",
                *p);
            return 0;
        }
    }
    /* Reject path traversal attempts */
    if (strstr(filename, "..") != NULL) {
        fprintf(stderr, "[-] Path traversal detected\n");
        return 0;
    }
    return 1;
}

void backup_file(const char *filename) {
    if (!validate_filename(filename)) {
        return;
    }

    /* Construct the output archive path safely */
    char archive_path[MAX_PATH];
    int  ret = snprintf(archive_path, sizeof(archive_path),
                        "%s/backup.tar.gz", BACKUP_DIR);
    if (ret < 0 || ret >= (int)sizeof(archive_path)) {
        fprintf(stderr, "[-] Archive path too long\n");
        return;
    }

    printf("[*] Backing up '%s' to '%s'\n", filename, archive_path);

    /*
     * FIX 2: Use fork() + execve() instead of system().
     *
     * execve() replaces the current process image with the specified
     * program. There is NO shell involved — arguments are passed as
     * an array, not as a single string. Shell metacharacters in filename
     * are treated as literal characters, not commands.
     *
     * argv[] is constructed from explicit, separate strings.
     * Even if filename = "a.txt; rm -rf /", tar receives that entire
     * string as its FOURTH argument — not a shell command.
     */
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return;
    }

    if (pid == 0) {
        /* Child process */
        char *const argv[] = {
            "/usr/bin/tar",
            "-czf",
            archive_path,
            (char *)filename,  /* passed as a literal argument, not parsed by shell */
            NULL
        };
        char *const envp[] = { NULL };  /* clean environment */

        execve("/usr/bin/tar", argv, envp);
        /* execve only returns on error */
        perror("execve");
        _exit(1);
    }

    /* Parent: wait for child */
    int status;
    if (waitpid(pid, &status, 0) < 0) {
        perror("waitpid");
        return;
    }

    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        printf("[+] Backup completed successfully.\n");
    } else {
        /* tar exits non-zero if file not found — that is expected in demo */
        printf("[*] tar exited with status %d (file may not exist — demo only).\n",
               WEXITSTATUS(status));
    }
}

int main(void) {
    char filename[256];

    /* Create backup dir safely (fixed path, no user input) */
    if (system("mkdir -p " BACKUP_DIR) != 0) {
        fprintf(stderr, "[-] Could not create backup dir\n");
        return 1;
    }

    printf("=== File Backup Utility (Patched) ===\n");
    printf("Enter filename to backup: ");
    fflush(stdout);

    if (fgets(filename, sizeof(filename), stdin) == NULL) {
        return 1;
    }

    size_t n = strlen(filename);
    if (n > 0 && filename[n-1] == '\n') filename[n-1] = '\0';

    backup_file(filename);
    return 0;
}
