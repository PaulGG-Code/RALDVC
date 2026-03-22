/*
 * Module 09 — Command Injection (CWE-78)
 * VULNERABLE VERSION — educational use only
 *
 * File backup utility that constructs a shell command using snprintf
 * and executes it with system(). User-supplied filename is not sanitized,
 * allowing injection of arbitrary shell commands.
 *
 * Compile with: gcc -g -O0 -fno-stack-protector -z execstack -no-pie -w
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CMD_SIZE    512
#define BACKUP_DIR  "/tmp/backups"

void backup_file(const char *filename) {
    char cmd[CMD_SIZE];

    /*
     * VULNERABILITY: filename is embedded directly into the shell command.
     * system() passes the string to /bin/sh -c, which interprets:
     *   ;   — command separator (run next command)
     *   &&  — run next command if previous succeeded
     *   ||  — run next command if previous failed
     *   |   — pipe output
     *   `cmd` or $(cmd) — command substitution
     *   > < >> — I/O redirection
     *
     * Example attack: filename = "file.txt; cat /etc/passwd"
     * Resulting cmd: "tar -czf /tmp/backups/backup.tar.gz file.txt; cat /etc/passwd"
     * shell runs: tar ... && cat /etc/passwd
     */
    snprintf(cmd, sizeof(cmd),
             "tar -czf %s/backup.tar.gz %s 2>/dev/null; echo '[backup] done'",
             BACKUP_DIR, filename);

    printf("[*] Executing: %s\n\n", cmd);

    /* system() = fork() + exec("/bin/sh", "-c", cmd) */
    int ret = system(cmd);  /* <-- NEVER use system() with user input */

    if (ret == 0) {
        printf("[+] Backup successful.\n");
    } else {
        printf("[-] Backup returned status: %d\n", ret);
    }
}

int main(void) {
    char filename[256];

    /* Create backup directory */
    system("mkdir -p " BACKUP_DIR);  /* also vulnerable but for setup only */

    printf("=== File Backup Utility ===\n");
    printf("Enter filename to backup: ");
    fflush(stdout);

    if (fgets(filename, sizeof(filename), stdin) == NULL) {
        return 1;
    }

    /* Strip trailing newline */
    size_t n = strlen(filename);
    if (n > 0 && filename[n-1] == '\n') filename[n-1] = '\0';

    backup_file(filename);
    return 0;
}
