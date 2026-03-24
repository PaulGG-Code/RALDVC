/*
 * Module 04 — Integer Overflow (CWE-190)
 * VULNERABLE VERSION — educational use only
 *
 * Ticket reservation system. User specifies how many tickets to reserve.
 * The allocation size is count * sizeof(Ticket) using a 32-bit multiply,
 * which can overflow to a small value, causing under-allocation.
 *
 * Compile with: gcc -g -O0 -fno-stack-protector -z execstack -no-pie -w
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_TICKET_NAME 32

typedef struct {
    uint32_t id;
    char     name[MAX_TICKET_NAME];
} Ticket;  /* sizeof(Ticket) == 36 bytes (4 + 32) */

void reserve_tickets(uint32_t count) {
    printf("[*] Requested tickets: %u\n", count);

    /*
     * VULNERABILITY: uint32_t multiplication can overflow.
     * If count is large enough, count * sizeof(Ticket) wraps around
     * to a value smaller than sizeof(Ticket), causing a tiny allocation.
     *
     * Example: sizeof(Ticket) = 36
     *   count = 0x10000000 (268435456)
     *   count * 36 = 0x240000000 → truncated to 0x40000000 (1 GB) in 64-bit
     *
     * More subtle example with a type mismatch:
     *   int count = 0x7FFFFFFF; count * 36 overflows signed int
     *
     * The real danger: count * element_size fits in the TYPE used for malloc's
     * argument, but produces a wrong (small) value.
     */
    size_t alloc_size = (uint32_t)(count * sizeof(Ticket));
    printf("[*] Computed allocation size: %zu bytes\n", alloc_size);
    printf("[*] Expected size: %u * %zu = %llu bytes\n",
           count, sizeof(Ticket), (unsigned long long)count * sizeof(Ticket));

    if (alloc_size == 0) {
        printf("[-] Overflow detected by coincidence (size == 0). Aborting.\n");
        return;
    }

    Ticket *tickets = malloc(alloc_size);
    if (!tickets) {
        printf("[-] malloc failed\n");
        return;
    }

    printf("[*] Allocated %zu bytes at %p\n", alloc_size, (void *)tickets);
    printf("[*] Initializing %u ticket(s)...\n", count);

    /*
     * Now we iterate over 'count' tickets, but only allocated alloc_size bytes.
     * If count was large enough to cause overflow, we write far beyond
     * the allocated region — heap buffer overflow from integer overflow.
     */
    for (uint32_t i = 0; i < count && i < 5; i++) {  /* cap at 5 for demo */
        tickets[i].id = i + 1;
        snprintf(tickets[i].name, MAX_TICKET_NAME, "Ticket-%u", i + 1);
        printf("[+] Initialized ticket %u\n", i + 1);
    }

    printf("[+] Reservation complete.\n");
    free(tickets);
}

int main(void) {
    uint32_t count;

    printf("=== Ticket Reservation System ===\n");
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
