/*
 * Module 10 — Race Condition / TOCTOU (CWE-367)
 * VULNERABLE VERSION — educational use only
 *
 * Privileged file reader. Checks access() then open() — classic TOCTOU.
 * An attacker can swap the file between the check and the open.
 *
 * In real setuid binaries, this allows reading /etc/shadow or other
 * root-owned files by winning the race:
 *   1. access() checks the symlink → points to safe file → OK
 *   2. Attacker redirects symlink → points to sensitive file
 *   3. open() follows the new symlink → reads sensitive file
 *
 * Compile with: gcc -g -O0 -fno-stack-protector -z execstack -no-pie -w
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define READ_SIZE 4096

void read_file(const char *path) {
    printf("[*] Checking access for path: %s\n", path);

    /*
     * VULNERABILITY STEP 1 — TIME OF CHECK
     * access() checks if the REAL user (not effective user) can read this path.
     * In setuid programs, this is used to verify the invoking user's permissions.
     *
     * The kernel resolves 'path' (including symlinks) at THIS moment.
     */
    if (access(path, R_OK) != 0) {
        fprintf(stderr, "[-] Access denied (or file not found): %s\n", path);
        return;
    }

    printf("[+] access() check passed.\n");
    printf("[*] [TOCTOU WINDOW: attacker can swap the symlink HERE]\n");

    /*
     * Artificial delay to make the race more reproducible in demo.
     * In real code, this window is microseconds — but still exploitable
     * with a tight race loop.
     */
    usleep(100000);  /* 100ms delay — makes race easily winnable for demo */

    /*
     * VULNERABILITY STEP 2 — TIME OF USE
     * open() also resolves 'path' including symlinks.
     * But 'path' may now point to a DIFFERENT file than when access() ran.
     */
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "[-] open() failed: %s\n", strerror(errno));
        return;
    }

    /* Read and print the file */
    char buf[READ_SIZE];
    ssize_t n = read(fd, buf, sizeof(buf) - 1);
    if (n > 0) {
        buf[n] = '\0';
        printf("\n[FILE CONTENTS BEGIN]\n%s\n[FILE CONTENTS END]\n", buf);
    } else {
        printf("[-] Empty file or read error\n");
    }
    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filepath>\n", argv[0]);
        fprintf(stderr, "Example: %s /tmp/safe_file\n", argv[0]);
        return 1;
    }

    printf("=== Privileged File Reader ===\n\n");
    read_file(argv[1]);
    return 0;
}
