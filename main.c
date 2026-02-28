#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"

char* read_file(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        perror("Failed to open file");
        exit(1);
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    char *buffer = malloc(size + 1);
    if (!buffer) {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }

    fread(buffer, 1, size, f);
    buffer[size] = '\0';

    fclose(f);
    return buffer;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: murasaki-cc <file>\n");
        return 1;
    }

    char *source = read_file(argv[1]);

    init_lexer(source);

    ASTNode *program = parse_program(next_token);

    interpret(program);

    free_ast(program);
    free(source);

    return 0;
}
