/*
 * Module 05 — Use-After-Free (CWE-416)
 * VULNERABLE VERSION — educational use only
 *
 * User session manager demonstrating UAF via allocator reuse.
 * The Session struct is freed on logout, but the pointer is not nulled.
 * A subsequent AdminToken allocation reuses the same memory block.
 * Accessing through the stale session pointer now touches AdminToken fields.
 *
 * Compile with: gcc -g -O0 -fno-stack-protector -z execstack -no-pie -w
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

/* Both structs intentionally the same size to ensure allocator reuse */
typedef struct {
    int      is_admin;         /* 0 = regular user, 1 = admin */
    int      uid;
    char     username[24];
} Session;  /* 32 bytes */

typedef struct {
    int      privilege_level;  /* overlaps with Session.is_admin */
    int      token_id;
    char     token_data[24];
} AdminToken;  /* 32 bytes — same size as Session */

/* Simulate glibc freelist behavior: on free+malloc of same size, reuse block */

Session *create_session(const char *username, int uid) {
    Session *s = malloc(sizeof(Session));
    if (!s) return NULL;
    s->is_admin = 0;
    s->uid      = uid;
    strncpy(s->username, username, sizeof(s->username) - 1);
    s->username[sizeof(s->username) - 1] = '\0';
    printf("[+] Session created: user='%s' uid=%d is_admin=%d\n",
           s->username, s->uid, s->is_admin);
    printf("[*] Session address: %p\n", (void *)s);
    return s;
}

void logout(Session *s) {
    printf("[*] Logging out user '%s'...\n", s->username);
    free(s);
    printf("[*] Session freed. (Pointer NOT nulled in vulnerable version)\n");
    /* BUG: we do NOT do s = NULL here (and even if we did, it only affects
     * the local copy of the pointer — the caller's pointer is unchanged) */
}

AdminToken *issue_admin_token(int token_id) {
    /*
     * malloc(32) will likely return the SAME address as the freed Session,
     * since they have identical sizes and glibc reuses recently freed blocks.
     */
    AdminToken *tok = malloc(sizeof(AdminToken));
    if (!tok) return NULL;
    tok->privilege_level = 0;  /* normal privilege */
    tok->token_id        = token_id;
    strncpy(tok->token_data, "READ_ONLY_TOKEN", sizeof(tok->token_data) - 1);
    tok->token_data[sizeof(tok->token_data) - 1] = '\0';
    printf("[+] AdminToken issued: id=%d privilege=%d\n",
           tok->token_id, tok->privilege_level);
    printf("[*] Token address: %p\n", (void *)tok);
    return tok;
}

int main(void) {
    printf("=== User Session Manager ===\n\n");

    /* Step 1: Create a session */
    Session *session = create_session("alice", 1001);
    printf("\n[Step 1] session->is_admin = %d (should be 0)\n\n", session->is_admin);

    /* Step 2: Logout — frees session but pointer remains */
    logout(session);
    /* session is now a dangling pointer — memory was freed */

    /* Step 3: Allocate AdminToken — likely reuses Session's memory */
    AdminToken *token = issue_admin_token(42);
    printf("\n[Step 3] token->privilege_level = %d\n", token->privilege_level);
    printf("[*] Is token at same address as old session? %s\n",
           (void *)token == (void *)session ? "YES (allocator reused the block!)" : "no");

    /* Step 4: USE-AFTER-FREE — access session after free */
    printf("\n[Step 4] USE-AFTER-FREE: reading session->is_admin after free...\n");
    printf("[!] session->is_admin = %d  (reading from freed/reused memory!)\n",
           session->is_admin);
    /* Since token overlaps session in memory, session->is_admin reads token->privilege_level */

    printf("\n[Step 5] Writing through dangling pointer: session->is_admin = 1\n");
    session->is_admin = 1;  /* UAF WRITE — corrupts token->privilege_level */
    printf("[!] After UAF write: token->privilege_level = %d (was 0!)\n",
           token->privilege_level);
    printf("[!] Privilege escalation via Use-After-Free!\n");

    free(token);
    return 0;
}
