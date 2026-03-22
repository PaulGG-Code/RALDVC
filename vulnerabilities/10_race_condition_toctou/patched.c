/*
 * Module 10 — Race Condition / TOCTOU (CWE-367)
 * PATCHED VERSION
 *
 * Fix: Eliminate the TOCTOU gap by:
 *   1. Using O_NOFOLLOW to refuse symlinks on open()
 *   2. Opening the file FIRST (getting an fd), THEN checking permissions
 *      on the fd using fstat() — not on the path
 *   3. Since we check the already-open fd, there's no window for the attacker
 *      to swap what the path resolves to between check and use
 *
 * The key insight: once you have an fd, the kernel has bound it to a specific
 * inode. Operations on the fd (fstat, fread) are always on that same inode,
 * regardless of what the path resolves to now.
 *
 * Compile with: gcc -g -O0 -fstack-protector-all -D_FORTIFY_SOURCE=2 -pie
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#define READ_SIZE 4096

void read_file(const char *path) {
    printf("[*] Opening path: %s\n", path);

    /*
     * FIX 1: Open with O_NOFOLLOW.
     * If 'path' is a symlink, open() with O_NOFOLLOW returns ELOOP immediately.
     * This prevents the classic "swap symlink after access() check" attack.
     *
     * Note: For directories, use O_NOFOLLOW | O_DIRECTORY.
     *
     * FIX 2: Open FIRST, check SECOND.
     * We get the fd before doing any access check. The check (fstat) is done
     * on the fd — which is bound to a specific inode — not on the path string.
     * There is NO window between check and use.
     */
    int fd = open(path, O_RDONLY | O_NOFOLLOW);
    if (fd < 0) {
        if (errno == ELOOP) {
            fprintf(stderr, "[-] Rejected: '%s' is a symbolic link (O_NOFOLLOW)\n", path);
        } else {
            fprintf(stderr, "[-] Cannot open '%s': %s\n", path, strerror(errno));
        }
        return;
    }

    /*
     * FIX 3: Check permissions on the open fd using fstat().
     * fstat() operates on the inode behind fd — immune to any path manipulation.
     *
     * For a setuid program, we'd also check that st.st_uid matches the real uid,
     * or use the process's effective uid. Here we check that the file is
     * world-readable as a simple permission demonstration.
     */
    struct stat st;
    if (fstat(fd, &st) < 0) {
        perror("fstat");
        close(fd);
        return;
    }

    printf("[*] File inode: %lu  mode: %04o  uid: %u\n",
           (unsigned long)st.st_ino, st.st_mode & 07777, st.st_uid);

    /* Simple check: is this a regular file? */
    if (!S_ISREG(st.st_mode)) {
        fprintf(stderr, "[-] Not a regular file\n");
        close(fd);
        return;
    }

    /* Check world-readable bit as a demo (real code would check real uid vs st_uid) */
    if (!(st.st_mode & S_IROTH) && getuid() != st.st_uid && getuid() != 0) {
        fprintf(stderr, "[-] Permission denied (fd-based check)\n");
        close(fd);
        return;
    }

    printf("[+] Permission check passed (fd-based, no TOCTOU gap).\n");

    /* Read and print */
    char buf[READ_SIZE];
    ssize_t n = read(fd, buf, sizeof(buf) - 1);
    if (n > 0) {
        buf[n] = '\0';
        printf("\n[FILE CONTENTS BEGIN]\n%s\n[FILE CONTENTS END]\n", buf);
    }
    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filepath>\n", argv[0]);
        return 1;
    }

    printf("=== Privileged File Reader (Patched) ===\n\n");
    read_file(argv[1]);
    return 0;
}
