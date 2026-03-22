/*
 * Module 04 — Integer Overflow (CWE-190)
 * PATCHED VERSION
 *
 * Fixes applied:
 *   1. Use __builtin_mul_overflow() to detect multiplication overflow
 *      before passing the result to malloc()
 *   2. Add an explicit upper bound check (MAX_TICKETS)
 *   3. Use size_t consistently for size calculations
 *
 * Compile with: gcc -g -O0 -fstack-protector-all -D_FORTIFY_SOURCE=2 -pie
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_TICKET_NAME 32
#define MAX_TICKETS     10000  /* reasonable upper bound for this application */

typedef struct {
    uint32_t id;
    char     name[MAX_TICKET_NAME];
} Ticket;

void reserve_tickets(uint32_t count) {
    printf("[*] Requested tickets: %u\n", count);

    /*
     * FIX 1: Explicit upper bound.
     * Reject absurdly large counts before even attempting the multiplication.
     * This is the first line of defense and makes the intent clear.
     */
    if (count == 0 || count > MAX_TICKETS) {
        fprintf(stderr, "[-] Error: count must be between 1 and %d.\n", MAX_TICKETS);
        return;
    }

    /*
     * FIX 2: Use __builtin_mul_overflow() to check for overflow.
     * If count * sizeof(Ticket) would overflow size_t, abort.
     * This is a GCC/Clang built-in available since GCC 5.
     */
    size_t alloc_size;
    if (__builtin_mul_overflow((size_t)count, sizeof(Ticket), &alloc_size)) {
        fprintf(stderr, "[-] Error: ticket count too large — integer overflow in size calculation.\n");
        return;
    }

    printf("[*] Allocation size: %zu bytes (safe)\n", alloc_size);

    Ticket *tickets = malloc(alloc_size);
    if (!tickets) {
        perror("malloc");
        return;
    }

    for (uint32_t i = 0; i < count; i++) {
        tickets[i].id = i + 1;
        snprintf(tickets[i].name, MAX_TICKET_NAME, "Ticket-%u", i + 1);
    }

    printf("[+] Successfully reserved %u ticket(s).\n", count);
    free(tickets);
}

int main(void) {
    uint32_t count;

    printf("=== Ticket Reservation System (Patched) ===\n");
    printf("sizeof(Ticket) = %zu bytes\n\n", sizeof(Ticket));
    printf("Number of tickets to reserve: ");
    fflush(stdout);

    if (scanf("%u", &count) != 1) {
        fprintf(stderr, "Invalid input.\n");
        return 1;
    }

    reserve_tickets(count);
    return 0;
}
