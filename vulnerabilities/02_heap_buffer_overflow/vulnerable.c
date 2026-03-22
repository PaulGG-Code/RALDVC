/*
 * Module 02 — Heap Buffer Overflow (CWE-122)
 * VULNERABLE VERSION — educational use only
 *
 * A note-taking app that allocates two adjacent heap objects:
 *   1. char note[NOTE_SIZE]   — the note content
 *   2. NoteCallback           — a struct containing a function pointer
 *
 * VULNERABILITY: strcpy() into note performs no bounds check.
 * If input > NOTE_SIZE, it overflows into the NoteCallback struct,
 * allowing an attacker to overwrite the on_save function pointer.
 *
 * Compile with: gcc -g -O0 -fno-stack-protector -z execstack -no-pie
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define NOTE_SIZE 32

/* Callback struct stored on the heap right after the note buffer */
typedef struct {
    void (*on_save)(const char *note);  /* function pointer — attacker target */
    char  label[16];
} NoteCallback;

/* Normal save function */
void normal_save(const char *note) {
    printf("[+] Note saved: %s\n", note);
}

/* Win function — should never be called directly by a normal user */
void secret_admin_fn(const char *note) {
    (void)note;
    printf("\n[!] SECRET ADMIN FUNCTION CALLED!\n");
    printf("[!] Attacker redirected execution via function pointer overwrite.\n");
    printf("[!] In a real scenario, this could spawn a shell or escalate privileges.\n");
}

void save_note(char *note_buf, NoteCallback *cb, const char *input) {
    printf("[*] Saving note (input length: %zu)...\n", strlen(input));

    /*
     * VULNERABILITY: strcpy() copies until it hits a null byte.
     * If strlen(input) >= NOTE_SIZE, bytes spill into cb's memory region.
     * The on_save function pointer is the first field of NoteCallback,
     * so a sufficiently long input overwrites it with attacker-controlled bytes.
     */
    strcpy(note_buf, input);  /* <-- NO BOUNDS CHECK */

    printf("[*] Invoking save callback...\n");
    cb->on_save(note_buf);   /* calls whatever function pointer now holds */
}

int main(void) {
    /* Allocate note buffer and callback struct consecutively on the heap.
     * glibc's allocator will place them next to each other (typically). */
    char *note_buf   = malloc(NOTE_SIZE);
    NoteCallback *cb = malloc(sizeof(NoteCallback));

    if (!note_buf || !cb) {
        perror("malloc");
        return 1;
    }

    /* Initialize callback with the normal save function */
    cb->on_save = normal_save;
    strncpy(cb->label, "default", sizeof(cb->label) - 1);
    cb->label[sizeof(cb->label) - 1] = '\0';

    printf("=== Note Taking Application ===\n");
    printf("[*] note_buf address:  %p\n", (void *)note_buf);
    printf("[*] callback address:  %p\n", (void *)cb);
    printf("[*] on_save ptr:       %p\n", (void *)cb->on_save);
    printf("[*] secret_admin_fn:   %p\n", (void *)secret_admin_fn);
    printf("[*] Distance between:  %td bytes\n\n",
           (uint8_t *)cb - (uint8_t *)note_buf);

    printf("Enter note (max %d chars): ", NOTE_SIZE - 1);
    fflush(stdout);

    char input[256];
    if (fgets(input, sizeof(input), stdin) == NULL) {
        free(note_buf);
        free(cb);
        return 1;
    }
    /* strip newline */
    size_t n = strlen(input);
    if (n > 0 && input[n-1] == '\n') input[n-1] = '\0';

    save_note(note_buf, cb, input);

    free(note_buf);
    free(cb);
    return 0;
}
