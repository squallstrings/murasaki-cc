#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "token.h"


void init_lexer(const char *input);
Token next_token(void);

char *read_file(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    char *buffer = malloc(size + 1);
    if (!buffer) {
        fclose(f);
        return NULL;
    }

    fread(buffer, 1, size, f);
    buffer[size] = '\0';

    fclose(f);
    return buffer;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("feed me files\n");
        return 1;
    }

    char *source = read_file(argv[1]);
 if (!source) {
    perror("File open failed");
    return 1;
}
    init_lexer(source);

    Token t;

    printf("TOKENS\n");

    do {
        t = next_token();
        printf("Token: %d  Value: %ld\n", t.type, t.value);
    } while (t.type != TOKEN_EOF);

    free(source);

    return 0;
}
