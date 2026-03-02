#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "token.h"
#include "ast.h"

static Token current;
static Token (*next_tok_fn)(void);

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
    ASTNode *n = malloc(sizeof(ASTNode));
    if (!n) {
        fprintf(stderr, "Out of memory\n");
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
    if (prog->program.count == prog->program.capacity) {
        size_t newcap = prog->program.capacity ? prog->program.capacity * 2 : 8;
        ASTNode **newarr = realloc(prog->program.statements, newcap * sizeof(ASTNode*));
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

static ASTNode* parse_factor() {
    if (current.type == TOKEN_NUMBER) {
        long v = current.value;
        advance_token();
        return new_number(v);
    }

    if (current.type == TOKEN_LPAREN) {
        advance_token();
        ASTNode *expr = parse_expression();
        expect(TOKEN_RPAREN, "Expected ')'");
        return expr;
    }

    parser_error("Expected number or '('");
    return NULL;
}
static ASTNode* parse_term() {
    ASTNode *left = parse_factor();

    while (current.type == TOKEN_STAR || current.type == TOKEN_SLASH) {
        char op = (current.type == TOKEN_STAR) ? '*' : '/';
        advance_token();
        ASTNode *right = parse_factor();
        left = new_binary(left, op, right);
    }

    return left;
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
        expect(TOKEN_SEMICOLON, "Expected ';' after expression");
        return new_print(expr);
    }

    parser_error("Expected statement");
    return NULL;
}



ASTNode* parse_program(Token (*next_token_func)(void)) {
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
