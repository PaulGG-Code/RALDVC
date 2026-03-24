/*
 * Module 05 — Use-After-Free (CWE-416)
 * PATCHED VERSION
 *
 * Fixes applied:
 *   1. The logout() function now takes a Session** and sets *s = NULL after free
 *   2. All dereferences check for NULL first
 *   3. A safe_free() macro ensures the pattern is consistently applied
 *
 * Compile with: gcc -g -O0 -fstack-protector-all -D_FORTIFY_SOURCE=2 -pie
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
 * FIX: safe_free macro — free and immediately NULL the pointer.
 * Using a macro lets it work for any pointer type and avoids
 * the need for a separate wrapper per type.
 */
#define safe_free(ptr) do { free(ptr); (ptr) = NULL; } while (0)

typedef struct {
    int      is_admin;
    int      uid;
    char     username[24];
} Session;

typedef struct {
    int      privilege_level;
    int      token_id;
    char     token_data[24];
} AdminToken;

Session *create_session(const char *username, int uid) {
    Session *s = malloc(sizeof(Session));
    if (!s) return NULL;
    s->is_admin = 0;
    s->uid      = uid;
    strncpy(s->username, username, sizeof(s->username) - 1);
    s->username[sizeof(s->username) - 1] = '\0';
    printf("[+] Session created: user='%s' uid=%d\n", s->username, s->uid);
    return s;
}

/*
 * FIX: Takes Session** so it can NULL the caller's pointer.
 * After this function returns, *session_ptr == NULL.
 * Any subsequent dereference through the now-NULL pointer will
 * crash immediately (SIGSEGV) — a detectable, predictable failure
 * rather than silent heap corruption.
 */
void logout(Session **session_ptr) {
    if (!session_ptr || !*session_ptr) return;
    printf("[*] Logging out user '%s'...\n", (*session_ptr)->username);
    safe_free(*session_ptr);  /* free AND set to NULL */
    printf("[*] Session freed and pointer nulled.\n");
}

AdminToken *issue_admin_token(int token_id) {
    AdminToken *tok = malloc(sizeof(AdminToken));
    if (!tok) return NULL;
    tok->privilege_level = 0;
    tok->token_id        = token_id;
    strncpy(tok->token_data, "READ_ONLY_TOKEN", sizeof(tok->token_data) - 1);
    tok->token_data[sizeof(tok->token_data) - 1] = '\0';
    printf("[+] AdminToken issued: id=%d privilege=%d\n",
           tok->token_id, tok->privilege_level);
    return tok;
}

int main(void) {
    printf("=== User Session Manager (Patched) ===\n\n");

    Session *session = create_session("alice", 1001);

    logout(&session);  /* session is NOW NULL */

    printf("[*] session pointer after logout: %p (NULL = safe)\n\n", (void *)session);

    AdminToken *token = issue_admin_token(42);

    /* FIX: Check for NULL before any access */
    if (session != NULL) {
        printf("[?] session->is_admin = %d\n", session->is_admin);
    } else {
        printf("[+] session is NULL — skipping dereference (SAFE).\n");
        printf("[+] token->privilege_level = %d (uncorrupted)\n",
               token->privilege_level);
    }

    safe_free(token);
    return 0;
}
