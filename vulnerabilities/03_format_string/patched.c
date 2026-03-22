/*
 * Module 03 — Format String Vulnerability (CWE-134)
 * PATCHED VERSION
 *
 * Fix applied:
 *   Use printf("%s", msg) instead of printf(msg).
 *   The format string is now a compile-time literal.
 *   User input can only affect the VALUE of the %s argument,
 *   not the format string itself.
 *
 * Compile with: gcc -g -O0 -fstack-protector-all -D_FORTIFY_SOURCE=2 -pie
 *               -Wformat -Werror=format-security
 *
 * Note: -Werror=format-security will cause the VULNERABLE version's
 * printf(msg) to fail at compile time, demonstrating how the compiler
 * can catch this class of bug.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int authorized = 0;

void show_access_level(void) {
    if (authorized) {
        printf("[!] ACCESS LEVEL: ADMIN\n");
    } else {
        printf("[-] ACCESS LEVEL: USER (no admin rights)\n");
    }
}

void log_message(const char *msg) {
    printf("=== LOG: ");
    /*
     * FIX: printf("%s", msg) — msg is an ARGUMENT, not the format string.
     * Any %n, %x, %s inside msg are printed as literal characters,
     * not interpreted as format specifiers.
     *
     * The -Wformat -Werror=format-security compiler flags would have
     * caught the vulnerable version at compile time.
     */
    printf("%s", msg);  /* SAFE: user input is only the argument */
    printf(" ===\n");
}

int main(void) {
    char input[512];

    printf("=== Secure Logging System (Patched) ===\n");
    printf("[*] Try entering: %%x.%%x.%%x.%%x\n");
    printf("[*] In the patched version, these are printed literally.\n\n");
    printf("Enter log message: ");
    fflush(stdout);

    if (fgets(input, sizeof(input), stdin) == NULL) {
        return 1;
    }
    size_t n = strlen(input);
    if (n > 0 && input[n-1] == '\n') input[n-1] = '\0';

    log_message(input);

    printf("\n[*] authorized = %d (unchanged)\n", authorized);
    show_access_level();

    return 0;
}
