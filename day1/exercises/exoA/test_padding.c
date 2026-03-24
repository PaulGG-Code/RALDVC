#include <stdio.h>

typedef struct {
    char         user[16];  /* buffer de 16 octets */
    unsigned int role;      /* 4 octets (sur x86-64) */
} session_t;

int main(void) {
    session_t s;
    printf("sizeof(session_t) = %zu\n", sizeof(s));
    printf("offsetof(user) = 0\n");
    printf("offsetof(role) = %zu\n", (char *)&s.role - (char *)s.user);
    printf("sizeof(user) = %zu\n", sizeof(s.user));
    printf("sizeof(role) = %zu\n", sizeof(s.role));
    return 0;
}
