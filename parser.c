#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "token.h"

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

static Token current;
static Token (*next_tok_fn)();

static void parser_error(const char *msg) {
    fprintf(stderr, "Parse error: %s\n", msg);
    exit(1);
}

static void advance_token() {
    current = next_tok_fn();
}

static int match(TokenType t) {
    if (current.type == t) {
        advance_token();
        return 1;
    }
    return 0;
}

static void expect(TokenType t, const char *msg) {
    if (!match(t)) {
        parser_error(msg);
    }
}

static ASTNode* new_node(ASTNodeType t) {
    ASTNode *n = (ASTNode*)malloc(sizeof(ASTNode));
    if (!n) {
        fprintf(stderr, "No mem\n");
        exit(1);
    }
    n->type = t;
    return n;
}

static ASTNode* new_number(long v) {
    ASTNode *n = new_node(AST_NUMBER);
    n->number.value = v;
    return n;
}

static ASTNode* new_binary(ASTNode *l, char op, ASTNode *r) {
    ASTNode *n = new_node(AST_BINARY);
    n->binary.left = l;
    n->binary.right = r;
    n->binary.op = op;
    return n;
}

static ASTNode* new_print(ASTNode *e) {
    ASTNode *n = new_node(AST_PRINT);
    n->print_stmt.expr = e;
    return n;
}

static ASTNode* new_program() {
    ASTNode *n = new_node(AST_PROGRAM);
    n->program.statements = NULL;
    n->program.count = 0;
    n->program.capacity = 0;
    return n;
}

static void program_push(ASTNode *prog, ASTNode *stmt) {
    if (prog->type != AST_PROGRAM) {
        parser_error("Internal: not a program node");
    }
    if (prog->program.count == prog->program.capacity) {
        size_t newcap = prog->program.capacity ? prog->program.capacity * 2 : 8;
        ASTNode **newarr = (ASTNode**)realloc(prog->program.statements, newcap * sizeof(ASTNode*));
        if (!newarr) {
            fprintf(stderr, "Out of memory\n");
            exit(1);
        }
        prog->program.statements = newarr;
        prog->program.capacity = newcap;
    }
    prog->program.statements[prog->program.count++] = stmt;
}

static ASTNode* parse_expression();

static ASTNode* parse_term() {
    if (current.type == TOKEN_NUMBER) {
        long v = current.value;
        advance_token();
        return new_number(v);
    }
    parser_error("Expected number");
    return NULL;
}

static ASTNode* parse_expression() {
    ASTNode *left = parse_term();

    while (current.type == TOKEN_PLUS || current.type == TOKEN_MINUS) {
        char op = (current.type == TOKEN_PLUS) ? '+' : '-';
        advance_token();
        ASTNode *right = parse_term();
        left = new_binary(left, op, right);
    }

    return left;
}

static ASTNode* parse_statement() {
    if (current.type == TOKEN_PRINT) {
        advance_token();
        ASTNode *expr = parse_expression();
        expect(TOKEN_SEMICOLON, "Expected ';' after print expression");
        return new_print(expr);
    }

    parser_error("Expected statement");
    return NULL;
}

ASTNode* parse_program(Token (*next_token_func)()) {
    next_tok_fn = next_token_func;
    advance_token();

    ASTNode *prog = new_program();

    while (current.type != TOKEN_EOF) {
        if (current.type == TOKEN_INVALID) {
            parser_error("Invalid token");
        }
        ASTNode *stmt = parse_statement();
        program_push(prog, stmt);
    }

    return prog;
}

void free_ast(ASTNode *n) {
    if (!n) return;

    switch (n->type) {
        case AST_NUMBER:
            break;

        case AST_BINARY:
            free_ast(n->binary.left);
            free_ast(n->binary.right);
            break;

        case AST_PRINT:
            free_ast(n->print_stmt.expr);
            break;

        case AST_PROGRAM:
            for (size_t i = 0; i < n->program.count; i++) {
                free_ast(n->program.statements[i]);
            }
            free(n->program.statements);
            break;
    }

    free(n);
}

static void indent(int depth) {
    for (int i = 0; i < depth; i++) putchar(' ');
}

void print_ast(ASTNode *n, int depth) {
    if (!n) return;

    indent(depth);

    switch (n->type) {
        case AST_NUMBER:
            printf("Number(%ld)\n", n->number.value);
            break;

        case AST_BINARY:
            printf("Binary(%c)\n", n->binary.op);
            print_ast(n->binary.left, depth + 2);
            print_ast(n->binary.right, depth + 2);
            break;

        case AST_PRINT:
            printf("Print\n");
            print_ast(n->print_stmt.expr, depth + 2);
            break;

        case AST_PROGRAM:
            printf("Program\n");
            for (size_t i = 0; i < n->program.count; i++) {
                print_ast(n->program.statements[i], depth + 2);
            }
            break;
    }
}
