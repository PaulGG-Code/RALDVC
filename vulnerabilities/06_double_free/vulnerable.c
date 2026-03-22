/*
 * Module 06 — Double Free (CWE-415)
 * VULNERABLE VERSION — educational use only
 *
 * Data processing pipeline with goto-based error handling.
 * A double-free occurs when an intermediate step fails:
 * the error goto frees 'buffer', and then the cleanup label
 * frees 'buffer' again.
 *
 * Compile with: gcc -g -O0 -fno-stack-protector -z execstack -no-pie -w
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 64

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
    /* Simulate validation — reject empty strings */
    if (buffer == NULL || buffer[0] == '\0') {
        fprintf(stderr, "[validate] Empty data — validation failed\n");
        return -1;
    }
    printf("[validate] OK: '%s'\n", buffer);
    return 0;
}

int process_record(const char *input, int simulate_validate_error) {
    char   *buffer  = NULL;
    Record *record  = NULL;
    int     ret     = 0;

    buffer = malloc(BUFFER_SIZE);
    if (!buffer) {
        fprintf(stderr, "[-] malloc(buffer) failed\n");
        ret = -1;
        goto cleanup;
    }
    printf("[*] Allocated buffer at %p\n", (void *)buffer);

    if (parse_data(input, buffer, BUFFER_SIZE) != 0) {
        ret = -1;
        goto cleanup;  /* frees buffer */
    }

    record = malloc(sizeof(Record));
    if (!record) {
        fprintf(stderr, "[-] malloc(record) failed\n");
        ret = -1;
        goto cleanup;  /* frees buffer */
    }

    /*
     * Simulate a validation step that can fail.
     * When it fails, we free buffer here (on the error path)...
     */
    if (simulate_validate_error || validate_data(buffer) != 0) {
        fprintf(stderr, "[-] Validation failed — aborting\n");
        free(buffer);    /* FIRST FREE */
        free(record);
        ret = -1;
        goto cleanup;    /* ...and then cleanup frees buffer AGAIN */
    }

    record->data      = buffer;
    record->processed = 1;
    printf("[+] Record processed successfully: '%s'\n", record->data);
    free(record->data);
    free(record);
    return 0;

cleanup:
    /*
     * VULNERABILITY: If the validate_error path was taken,
     * buffer was already freed above. Now we free it again.
     * This is a double-free — heap metadata corruption.
     */
    printf("[*] Cleanup: freeing buffer at %p\n", (void *)buffer);
    free(buffer);   /* SECOND FREE — double-free if we came from error path */
    free(record);   /* may also double-free record depending on path */
    return ret;
}

int main(void) {
    printf("=== Data Processing Pipeline ===\n\n");

    printf("[Test 1] Normal processing:\n");
    process_record("hello world", 0);

    printf("\n[Test 2] Triggering double-free via validation error:\n");
    process_record("trigger_error", 1);  /* simulate_validate_error = 1 */

    printf("\n[*] Program completed (or crashed from double-free above)\n");
    return 0;
}
