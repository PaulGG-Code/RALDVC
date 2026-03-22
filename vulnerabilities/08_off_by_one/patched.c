/*
 * Module 08 — Off-by-One Error (CWE-193)
 * PATCHED VERSION
 *
 * Fixes applied:
 *   1. Use snprintf() for path construction — it handles the null terminator
 *      correctly and never writes past the buffer size
 *   2. Check snprintf return value for truncation
 *   3. If manual loop is necessary, use i < len (not i <= len)
 *
 * Compile with: gcc -g -O0 -fstack-protector-all -D_FORTIFY_SOURCE=2 -pie
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PATH_SIZE 64

void build_path(const char *directory, const char *filename) {
    char path[PATH_SIZE];
    int  permission = 0xDEADBEEF;

    printf("[*] Before copy:\n");
    printf("    permission:  0x%X\n", (unsigned)permission);

    /*
     * FIX: Use snprintf() for path construction.
     *
     * snprintf(dst, size, fmt, ...) writes AT MOST size-1 characters
     * plus a null terminator — it NEVER writes past dst[size-1].
     * There is no off-by-one risk with snprintf.
     *
     * Additionally, the return value tells us if the result was truncated
     * (ret >= PATH_SIZE means truncation occurred).
     */
    int ret = snprintf(path, sizeof(path), "%s/%s", directory, filename);

    if (ret < 0) {
        fprintf(stderr, "[-] snprintf encoding error\n");
        return;
    }
    if (ret >= (int)sizeof(path)) {
        fprintf(stderr, "[-] Path too long — truncated to %zu chars\n",
                sizeof(path) - 1);
        /* Decide: reject or use the truncated path? Here we reject. */
        return;
    }

    printf("\n[*] After copy:\n");
    printf("    path:        '%s'\n", path);
    printf("    permission:  0x%X  (unchanged)\n", (unsigned)permission);

    if (permission == 0xDEADBEEF) {
        printf("    [+] Permission flag intact — no off-by-one.\n");
    } else {
        printf("    [-] Unexpected corruption (this should not happen)\n");
    }
}

int main(void) {
    printf("=== Path Builder Utility (Patched) ===\n\n");

    printf("[Test 1] Short path:\n");
    build_path("/tmp", "short.txt");

    printf("\n");

    char long_filename[60];
    memset(long_filename, 'A', 59);
    long_filename[59] = '\0';

    printf("[Test 2] Filename that fills buffer exactly:\n");
    build_path("/tmp", long_filename);

    return 0;
}
