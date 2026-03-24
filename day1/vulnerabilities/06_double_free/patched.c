/*
 * Module 06 — Double Free (CWE-415)
 * PATCHED VERSION
 *
 * Fixes applied:
 *   1. safe_free() macro: free + NULL in one operation
 *   2. Cleanup label ONLY frees non-NULL pointers (safe because safe_free nulls them)
 *   3. Error paths use safe_free() so the cleanup label is a no-op double-free-safe pass
 *   4. Single cleanup path — no early frees on error paths
 *
 * Compile with: gcc -g -O0 -fstack-protector-all -D_FORTIFY_SOURCE=2 -pie
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 64

/*
 * FIX: safe_free ensures the pointer becomes NULL after freeing.
 * This means free(NULL) in cleanup is always safe — C standard guarantees
 * free(NULL) is a no-op. No double-free is possible.
 */
#define safe_free(ptr) do { free(ptr); (ptr) = NULL; } while (0)

typedef struct {
    char *data;
    int   processed;
} Record;

int parse_data(const char *input, char *buffer, size_t bufsize) {
    if (strlen(input) >= bufsize) {
        fprintf(stderr, "[parse] Input too long\n");
        return -1;
    }
    strncpy(buffer, input, bufsize - 1);
    buffer[bufsize - 1] = '\0';
    return 0;
}

int validate_data(const char *buffer) {
    if (buffer == NULL || buffer[0] == '\0') {
        fprintf(stderr, "[validate] Empty data\n");
        return -1;
    }
    printf("[validate] OK: '%s'\n", buffer);
    return 0;
}

int process_record(const char *input, int simulate_validate_error) {
    char   *buffer = NULL;
    Record *record = NULL;
    int     ret    = 0;

    buffer = malloc(BUFFER_SIZE);
    if (!buffer) {
        perror("malloc");
        ret = -1;
        goto cleanup;
    }

    if (parse_data(input, buffer, BUFFER_SIZE) != 0) {
        ret = -1;
        goto cleanup;
    }

    record = malloc(sizeof(Record));
    if (!record) {
        perror("malloc");
        ret = -1;
        goto cleanup;
    }

    /*
     * FIX: Do NOT free buffer on the error path here.
     * Let the single cleanup label handle ALL frees.
     * Because safe_free() sets pointers to NULL, cleanup can
     * safely call free() on them regardless of which path was taken.
     */
    if (simulate_validate_error || validate_data(buffer) != 0) {
        fprintf(stderr, "[-] Validation failed\n");
        ret = -1;
        goto cleanup;  /* falls through to single cleanup — safe */
    }

    record->data      = buffer;
    record->processed = 1;
    printf("[+] Record processed: '%s'\n", record->data);

    /*
     * On success, we transfer ownership of buffer into record.
     * NULL buffer so cleanup doesn't double-free it.
     */
    buffer = NULL;
    safe_free(record);
    return 0;

cleanup:
    /*
     * FIX: Use safe_free so this is idempotent — safe to call even if
     * a previous path already freed the pointer (it's now NULL, so free(NULL)
     * is a no-op per C99/C11 §7.22.3.3).
     */
    safe_free(buffer);
    safe_free(record);
    return ret;
}

int main(void) {
    printf("=== Data Processing Pipeline (Patched) ===\n\n");

    printf("[Test 1] Normal processing:\n");
    process_record("hello world", 0);

    printf("\n[Test 2] Triggering error path (no double-free in patched version):\n");
    process_record("trigger_error", 1);

    printf("\n[+] Program completed cleanly — no double-free.\n");
    return 0;
}
