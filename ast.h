#ifndef AST_H
#define AST_H

#include <stddef.h>

typedef enum {
    AST_NUMBER,
    AST_BINARY,
    AST_PRINT,
    AST_PROGRAM
} ASTNodeType;

typedef struct ASTNode ASTNode;

struct ASTNode {
    ASTNodeType type;

    union {
        struct {
            long value;
        } number;

        struct {
            ASTNode *left;
            ASTNode *right;
            char op;
        } binary;

        struct {
            ASTNode *expr;
        } print_stmt;

        struct {
            ASTNode **statements;
            size_t count;
            size_t capacity;
        } program;
    };
};

#endif
