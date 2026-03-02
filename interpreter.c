#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

static long eval_node(ASTNode *node);

static long eval_binary(ASTNode *node) {
    long left = eval_node(node->binary.left);
    long right = eval_node(node->binary.right);

    switch (node->binary.op) {
        case '+':
            return left + right;
        case '-':
            return left - right;
        case '*':
            return left * right;
        case '/':
            if (right == 0) {
                fprintf(stderr, "Runtime error: division by zero\n");
                exit(1);
            }
            return left / right;
        default:
            fprintf(stderr, "Runtime error: unknown operator '%c'\n", node->binary.op);
            exit(1);
    }
}
static long eval_node(ASTNode *node) {
    switch (node->type) {

        case AST_NUMBER:
            return node->number.value;

        case AST_BINARY:
            return eval_binary(node);

        case AST_PRINT: {
            long value = eval_node(node->print_stmt.expr);
            printf("%ld\n", value);
            return 0;
        }

        case AST_PROGRAM: {
            for (size_t i = 0; i < node->program.count; i++) {
                eval_node(node->program.statements[i]);
            }
            return 0;
        }

        default:
            fprintf(stderr, "Runtime error: invalid AST node type\n");
            exit(1);
    }
}

void interpret(ASTNode *root) {
    if (!root) {
        fprintf(stderr, "Runtime error: null AST\n");
        exit(1);
    }

    eval_node(root);
}
