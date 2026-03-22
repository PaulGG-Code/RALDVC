/*
 * Module 08 — Off-by-One Error (CWE-193)
 * VULNERABLE VERSION — educational use only
 *
 * Path builder utility. The copy loop uses <= instead of <,
 * writing the null terminator one byte past the end of the buffer.
 * The adjacent 'permission' variable gets its LSB overwritten.
 *
 * Compile with: gcc -g -O0 -fno-stack-protector -z execstack -no-pie -w
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PATH_SIZE 64

/*
 * These two variables are declared adjacent in the same scope so the
 * compiler places them next to each other on the stack.
 * path[64] is followed immediately by permission.
 *
 * Stack layout (grows downward, variables allocated in order):
 *   [path[64]]  [permission(int)]  [...]
 *    ^buf        ^buf+64
 *
 * Writing buf[64] (path[PATH_SIZE]) touches the first byte of permission.
 */
void build_path(const char *directory, const char *filename) {
    char path[PATH_SIZE];
    int  permission = 0xDEADBEEF;  /* "magic" value — clearly non-zero */

    printf("[*] Before copy:\n");
    printf("    &path:       %p\n", (void *)path);
    printf("    &permission: %p\n", (void *)&permission);
    printf("    permission:  0x%X\n", (unsigned)permission);
    printf("    distance:    %td bytes\n", (char *)&permission - (char *)path);

    /* Copy directory part */
    size_t dir_len = strlen(directory);
    if (dir_len >= PATH_SIZE) {
        fprintf(stderr, "[-] Directory path too long\n");
        return;
    }

    memcpy(path, directory, dir_len);

    /* Append separator */
    if (dir_len < PATH_SIZE - 1) {
        path[dir_len++] = '/';
    }

    /* Copy filename character by character */
    size_t fn_len = strlen(filename);
    size_t i;
    for (i = 0; i <= fn_len; i++) {   /* BUG: should be i < fn_len */
        /*
         * VULNERABILITY: When i == fn_len, filename[fn_len] == '\0'.
         * path[dir_len + fn_len] = '\0' writes one byte PAST the intended
         * end of the path content.
         *
         * If dir_len + fn_len == PATH_SIZE - 1, then
         * path[PATH_SIZE] writes to &permission (one byte past path[]).
         *
         * For little-endian systems: path[64] overwrites permission's
         * least-significant byte with 0x00, changing 0xDEADBEEF to 0xDEADBE00.
         * Still non-zero, but corrupted.
         *
         * For a permission flag set to 0x00000100, the null byte would
         * change it to 0x00000000 — permission denied!
         */
        if (dir_len + i >= PATH_SIZE) {
            fprintf(stderr, "[-] Filename too long\n");
            return;
        }
        path[dir_len + i] = filename[i];   /* off-by-one: writes null past end */
    }

    printf("\n[*] After copy:\n");
    printf("    path:        '%s'\n", path);
    printf("    permission:  0x%X  (was 0xDEADBEEF)\n", (unsigned)permission);

    if (permission == 0xDEADBEEF) {
        printf("    [+] Permission flag intact.\n");
    } else {
        printf("    [!] Permission flag CORRUPTED by off-by-one null byte!\n");
        printf("    [!] 0x%X != 0xDEADBEEF\n", (unsigned)permission);
    }
}

int main(void) {
    printf("=== Path Builder Utility ===\n\n");

    /* Test 1: Short filename — no overflow */
    printf("[Test 1] Short path (no overflow):\n");
    build_path("/tmp", "short.txt");

    printf("\n");

    /* Test 2: Filename exactly fills the buffer — off-by-one triggers */
    /* PATH_SIZE=64, dir="/tmp/" (5 chars), need filename of 58 chars to fill buffer */
    /* path = "/tmp/" + 58 chars = 63 chars + null at index 63 = SAFE */
    /* path = "/tmp/" + 59 chars = 64 chars + null at index 64 = OFF BY ONE */
    char long_filename[60];
    memset(long_filename, 'A', 59);
    long_filename[59] = '\0';  /* 59 A's */

    printf("[Test 2] Filename that fills buffer exactly (off-by-one):\n");
    build_path("/tmp", long_filename);  /* /tmp/ = 5, + 59 A's = 64 = PATH_SIZE */

    return 0;
}
