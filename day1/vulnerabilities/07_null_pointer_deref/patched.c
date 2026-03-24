/*
 * Module 07 — Null Pointer Dereference (CWE-476)
 * PATCHED VERSION
 *
 * Fixes applied:
 *   1. Always check pointer return values before dereference
 *   2. get_config_value_or_default() — never returns NULL to callers
 *      that can't handle it
 *   3. Explicit error messages instead of silent NULL dereferences
 *
 * Compile with: gcc -g -O0 -fstack-protector-all -D_FORTIFY_SOURCE=2 -pie
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

static ConfigEntry *config_head = NULL;

void config_set(const char *key, const char *value) {
    ConfigEntry *entry = malloc(sizeof(ConfigEntry));
    if (!entry) { perror("malloc"); return; }
    strncpy(entry->key,   key,   MAX_KEY   - 1); entry->key[MAX_KEY-1]   = '\0';
    strncpy(entry->value, value, MAX_VALUE - 1); entry->value[MAX_VALUE-1] = '\0';
    entry->next  = config_head;
    config_head  = entry;
}

const char *get_config_value(const char *key) {
    for (ConfigEntry *e = config_head; e != NULL; e = e->next) {
        if (strcmp(e->key, key) == 0) {
            return e->value;
        }
    }
    return NULL;
}

/*
 * FIX: Wrapper that returns a default value instead of NULL.
 * Use this when callers cannot handle NULL (e.g., passing to printf %s).
 */
const char *get_config_value_or_default(const char *key, const char *default_val) {
    const char *val = get_config_value(key);
    return (val != NULL) ? val : default_val;
}

void print_config_summary(void) {
    /*
     * FIX: Use get_config_value_or_default so we never pass NULL to printf.
     * The "(not set)" default makes missing config visible and safe.
     */
    const char *host    = get_config_value_or_default("host",    "(not set)");
    const char *port    = get_config_value_or_default("port",    "(not set)");
    const char *timeout = get_config_value_or_default("timeout", "(not set)");

    printf("=== Configuration Summary ===\n");
    printf("  host:    %s\n", host);
    printf("  port:    %s\n", port);
    printf("  timeout: %s\n", timeout);
}

int check_admin_access(const char *username) {
    const char *admin_user = get_config_value("admin_user");

    /*
     * FIX: Explicit NULL check before using admin_user.
     * If admin_user is not configured, deny access by default
     * (fail-closed security posture: when in doubt, deny).
     */
    if (admin_user == NULL) {
        fprintf(stderr, "[-] admin_user not configured — denying access.\n");
        return 0;  /* deny by default when config is missing */
    }

    return (strcmp(admin_user, username) == 0) ? 1 : 0;
}

int main(void) {
    printf("=== Configuration System (Patched) ===\n\n");

    config_set("port",    "8080");
    config_set("timeout", "30");
    /* "host" and "admin_user" still not set */

    printf("[*] Loaded config (missing: host, admin_user)\n\n");

    print_config_summary();   /* safe — uses default values */

    printf("\n[*] Checking admin access for 'root':\n");
    int is_admin = check_admin_access("root");
    printf("[*] Result: %s\n", is_admin ? "admin" : "not admin (access denied)");

    return 0;
}
