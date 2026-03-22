/*
 * Module 03 — Format String Vulnerability (CWE-134)
 * VULNERABLE VERSION — educational use only
 *
 * A logging utility where user input is passed directly as the
 * format string to printf(). This allows:
 *   - Reading arbitrary memory with %x, %s, %p
 *   - Writing to arbitrary memory with %n
 *
 * Compile with: gcc -g -O0 -fno-stack-protector -z execstack -no-pie -w
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Global authorization flag — target for %n write */
int authorized = 0;

void show_access_level(void) {
    if (authorized) {
        printf("[!] ACCESS LEVEL: ADMIN\n");
        printf("[!] Authorization flag overwritten via format string!\n");
    } else {
        printf("[-] ACCESS LEVEL: USER (no admin rights)\n");
    }
}

void log_message(const char *msg) {
    printf("=== LOG: ");
    /*
     * VULNERABILITY: msg is treated as the format string.
     * If msg contains %x, %s, %n, etc., printf interprets them.
     *
     * With %n, printf writes the number of bytes printed so far
     * into the address pointed to by the next argument on the stack.
     * Since no argument was passed, %n uses whatever is on the stack —
     * which an attacker can arrange to be a target address.
     */
    printf(msg);  /* <-- NEVER DO THIS */
    printf(" ===\n");
}

int main(void) {
    char input[512];

    printf("=== Secure Logging System ===\n");
    printf("[*] &authorized = %p\n", (void *)&authorized);
    printf("[*] authorized  = %d\n", authorized);
    printf("\n");

    printf("Enter log message: ");
    fflush(stdout);

    if (fgets(input, sizeof(input), stdin) == NULL) {
        return 1;
    }
    size_t n = strlen(input);
    if (n > 0 && input[n-1] == '\n') input[n-1] = '\0';

    log_message(input);

    printf("\n[*] After log_message(), authorized = %d\n", authorized);
    show_access_level();

    return 0;
}
