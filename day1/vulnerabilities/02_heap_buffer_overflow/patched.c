/*
 * Module 02 — Heap Buffer Overflow (CWE-122)
 * PATCHED VERSION
 *
 * Fixes applied:
 *   1. Replace strcpy() with strncpy() + explicit size enforcement
 *   2. Validate input length BEFORE copying
 *   3. Null-terminate after strncpy (strncpy does NOT guarantee null termination
 *      if source is longer than n)
 *   4. Compiled with -fstack-protector-all -D_FORTIFY_SOURCE=2 -pie
 *
 * Compile with: gcc -g -O0 -fstack-protector-all -D_FORTIFY_SOURCE=2 -pie
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NOTE_SIZE 32

typedef struct {
    void (*on_save)(const char *note);
    char  label[16];
} NoteCallback;

void normal_save(const char *note) {
    printf("[+] Note saved: %s\n", note);
}

void secret_admin_fn(const char *note) {
    (void)note;
    /* Should never be reached in the patched version */
    printf("[!] Secret admin function (should be unreachable from user input)\n");
}

void save_note(char *note_buf, NoteCallback *cb, const char *input) {
    /*
     * FIX 1: Validate input length before copying.
     * Reject anything that would not fit, rather than silently truncating.
     * This gives the caller an explicit error instead of undefined behavior.
     */
    if (strlen(input) >= NOTE_SIZE) {
        fprintf(stderr, "[-] Error: Note too long (max %d chars). Aborting save.\n",
                NOTE_SIZE - 1);
        return;
    }

    /*
     * FIX 2: strncpy with explicit size limit.
     * Even if the length check above is somehow bypassed (defense-in-depth),
     * strncpy will never copy more than NOTE_SIZE - 1 characters.
     */
    strncpy(note_buf, input, NOTE_SIZE - 1);

    /*
     * FIX 3: Explicit null termination.
     * strncpy does NOT null-terminate if source length >= n.
     * Always set the last byte to '\0' explicitly after strncpy.
     */
    note_buf[NOTE_SIZE - 1] = '\0';

    cb->on_save(note_buf);
}

int main(void) {
    char *note_buf   = malloc(NOTE_SIZE);
    NoteCallback *cb = malloc(sizeof(NoteCallback));

    if (!note_buf || !cb) {
        perror("malloc");
        free(note_buf);
        free(cb);
        return 1;
    }

    cb->on_save = normal_save;
    strncpy(cb->label, "default", sizeof(cb->label) - 1);
    cb->label[sizeof(cb->label) - 1] = '\0';

    printf("=== Note Taking Application (Patched) ===\n");
    printf("Enter note (max %d chars): ", NOTE_SIZE - 1);
    fflush(stdout);

    char input[256];
    if (fgets(input, sizeof(input), stdin) == NULL) {
        free(note_buf);
        free(cb);
        return 1;
    }
    size_t n = strlen(input);
    if (n > 0 && input[n-1] == '\n') input[n-1] = '\0';

    save_note(note_buf, cb, input);

    free(note_buf);
    free(cb);
    return 0;
}
