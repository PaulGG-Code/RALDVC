/*
 * Module 07 — Null Pointer Dereference (CWE-476)
 * VULNERABLE VERSION — educational use only
 *
 * Configuration parser. get_config_value() returns NULL for unknown keys.
 * Multiple callers dereference the return value without NULL checks.
 *
 * Compile with: gcc -g -O0 -fno-stack-protector -z execstack -no-pie -w
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_KEY   32
#define MAX_VALUE 64

typedef struct ConfigEntry {
    char key[MAX_KEY];
    char value[MAX_VALUE];
    struct ConfigEntry *next;
} ConfigEntry;

/* Simple linked-list config store */
static ConfigEntry *config_head = NULL;

void config_set(const char *key, const char *value) {
    ConfigEntry *entry = malloc(sizeof(ConfigEntry));
    if (!entry) { perror("malloc"); return; }
    strncpy(entry->key,   key,   MAX_KEY   - 1); entry->key[MAX_KEY-1]   = '\0';
    strncpy(entry->value, value, MAX_VALUE - 1); entry->value[MAX_VALUE-1] = '\0';
    entry->next  = config_head;
    config_head  = entry;
}

/*
 * Returns pointer to value string, or NULL if key not found.
 * Callers MUST check the return value before use.
 */
const char *get_config_value(const char *key) {
    for (ConfigEntry *e = config_head; e != NULL; e = e->next) {
        if (strcmp(e->key, key) == 0) {
            return e->value;
        }
    }
    return NULL;  /* Key not found */
}

void print_config_summary(void) {
    const char *host    = get_config_value("host");
    const char *port    = get_config_value("port");
    const char *timeout = get_config_value("timeout");

    printf("=== Configuration Summary ===\n");

    /*
     * VULNERABILITY: No NULL check before dereferencing host.
     * If "host" was never set, host == NULL and printf("%s", host) is UB.
     * On Linux with NULL != a valid mapping, this is a SIGSEGV.
     */
    printf("  host:    %s\n", host);     /* <-- CRASH if host is NULL */
    printf("  port:    %s\n", port);     /* <-- CRASH if port is NULL */
    printf("  timeout: %s\n", timeout);  /* <-- CRASH if timeout is NULL */
}

int check_admin_access(const char *username) {
    const char *admin_user = get_config_value("admin_user");

    /*
     * VULNERABILITY: If admin_user is NULL (not configured),
     * strcmp(NULL, username) is undefined behavior / SIGSEGV.
     *
     * Historical security relevance: if the NULL check is written as
     *   if (admin_user == username)   [pointer comparison, not string comparison]
     * or if the code has
     *   if (!admin_user || strcmp(admin_user, username) == 0)
     * with a logic bug, NULL could inadvertently grant access.
     */
    if (strcmp(admin_user, username) == 0) {  /* <-- CRASH if admin_user is NULL */
        return 1;  /* is admin */
    }
    return 0;
}

int main(void) {
    printf("=== Configuration System ===\n\n");

    /* Load partial config — intentionally missing some keys */
    config_set("port",    "8080");
    config_set("timeout", "30");
    /* "host" and "admin_user" are intentionally NOT set */

    printf("[*] Loaded config (missing: host, admin_user)\n\n");

    /* This will crash because "host" is not set */
    print_config_summary();

    return 0;
}
