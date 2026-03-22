/*
 * Module 01 — Stack Buffer Overflow (CWE-121)
 * PATCHED VERSION
 *
 * Fixes applied:
 *   1. Replace gets() with fgets() — enforces a maximum read size
 *   2. Strip trailing newline left by fgets
 *   3. Compiled with -fstack-protector-all for canary protection
 *   4. Compiled with -D_FORTIFY_SOURCE=2 for additional libc hardening
 *
 * Compile with: gcc -g -O0 -fstack-protector-all -D_FORTIFY_SOURCE=2 -pie
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_USERNAME 64
#define SECRET_PASSWORD "s3cr3tP@ssw0rd"

void access_granted(void) {
    printf("\n[!] ACCESS GRANTED — Welcome, privileged user!\n");
    printf("[!] You now have elevated system access.\n");
}

/* Helper: strip trailing newline that fgets includes */
static void strip_newline(char *s, size_t len) {
    size_t n = strnlen(s, len);
    if (n > 0 && s[n - 1] == '\n') {
        s[n - 1] = '\0';
    }
}

void login(void) {
    int authenticated = 0;
    char buf[MAX_USERNAME];
    char password[64];

    printf("=== Secure Login System (Patched) ===\n");
    printf("Username: ");
    fflush(stdout);

    /*
     * FIX: fgets(buf, sizeof(buf), stdin) reads at most sizeof(buf)-1 bytes.
     * No matter how much input the attacker sends, the buffer cannot overflow.
     * The size argument is derived from the buffer's actual size — never a
     * hard-coded constant that can drift out of sync with the declaration.
     */
    if (fgets(buf, sizeof(buf), stdin) == NULL) {
        fprintf(stderr, "Error reading username.\n");
        exit(1);
    }
    strip_newline(buf, sizeof(buf));

    printf("Password: ");
    fflush(stdout);

    if (fgets(password, sizeof(password), stdin) == NULL) {
        fprintf(stderr, "Error reading password.\n");
        exit(1);
    }
    strip_newline(password, sizeof(password));

    if (strcmp(password, SECRET_PASSWORD) == 0) {
        authenticated = 1;
    }

    if (authenticated) {
        access_granted();
    } else {
        printf("[-] Access denied. Invalid credentials.\n");
    }
}

int main(void) {
    login();
    return 0;
}
