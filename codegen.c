#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

static void emit_expression(FILE *out, ASTNode *node);

static void emit_expression(FILE *out, ASTNode *node) {
    switch (node->type) {
        case AST_NUMBER:
            fprintf(out, "%ld", node->number.value);
            break;

        case AST_BINARY:
            fprintf(out, "(");
            emit_expression(out, node->binary.left);
            fprintf(out, " %c ", node->binary.op);
            emit_expression(out, node->binary.right);
            fprintf(out, ")");
            break;

        default:
            fprintf(stderr, "Codegen error: invalid expression node\n");
            exit(1);
    }
}

void generate_c(ASTNode *root) {
    FILE *out = fopen("output.c", "w");
    if (!out) {
        fprintf(stderr, "Failed to open output.c\n");
        exit(1);
    }

    fprintf(out, "#include <stdio.h>\n\n");
    fprintf(out, "int main() {\n");

    for (size_t i = 0; i < root->program.count; i++) {
        ASTNode *stmt = root->program.statements[i];

        if (stmt->type == AST_PRINT) {
            fprintf(out, "    printf(\"%%ld\\n\", ");
            emit_expression(out, stmt->print_stmt.expr);
            fprintf(out, ");\n");
        }
    }

    fprintf(out, "    return 0;\n");
    fprintf(out, "}\n");

    fclose(out);
}
