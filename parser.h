#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "token.h"

ASTNode* parse_program(Token (*next_token_func)(void));
void free_ast(ASTNode *node);
void print_ast(ASTNode *node, int depth);

#endif
