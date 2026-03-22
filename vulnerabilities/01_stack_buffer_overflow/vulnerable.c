/*
 * Module 01 — Stack Buffer Overflow (CWE-121)
 * VULNERABLE VERSION — educational use only
 *
 * Simulates a login system. The vulnerability is the use of gets(),
 * which performs no bounds checking, allowing stack smashing.
 *
 * Compile with: gcc -g -O0 -fno-stack-protector -z execstack -no-pie
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

void login(void) {
    /* BUG: 'authenticated' sits adjacent to 'buf' on the stack.
     * Overflowing buf with enough bytes will overwrite authenticated,
     * bypassing the password check entirely. */
    int authenticated = 0;
    char buf[MAX_USERNAME];  /* fixed-size stack buffer */

    printf("=== Secure Login System ===\n");
    printf("Username: ");
    fflush(stdout);

    /* VULNERABILITY: gets() reads until newline with NO size limit.
     * Input longer than 64 bytes overflows into 'authenticated' and beyond. */
    gets(buf);  /* <-- NEVER USE THIS FUNCTION */

    printf("Password: ");
    fflush(stdout);

    char password[64];
    gets(password);  /* same bug on the password field */

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
