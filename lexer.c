#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "token.h"

static const char *src;
static size_t pos = 0;

void init_lexer(const char *input) {
    src = input;
    pos = 0;

    if ((unsigned char)src[0] == 0xEF &&
        (unsigned char)src[1] == 0xBB &&
        (unsigned char)src[2] == 0xBF) {
        pos = 3;
    }
}

static char peek() {
    return src[pos];
}

static char advance() {
    return src[pos++];
}

Token next_token() {
    while (isspace(peek()))
        advance();

    char c = peek();

    if (c == '\0')
        return (Token){TOKEN_EOF, 0};

    if (isdigit(c)) {
        long value = 0;
        while (isdigit(peek())) {
            value = value * 10 + (advance() - '0');
        }
        return (Token){TOKEN_NUMBER, value};
    }

    if (isalpha(c)) {
        size_t start = pos;

        while (isalpha(peek()))
            advance();

        size_t length = pos - start;

        if (length == 5 && strncmp(&src[start], "print", 5) == 0)
            return (Token){TOKEN_PRINT, 0};

        return (Token){TOKEN_INVALID, 0};
    }

    if (c == '+') {
        advance();
        return (Token){TOKEN_PLUS, 0};
    }

    if (c == '-') {
        advance();
        return (Token){TOKEN_MINUS, 0};
    }

    if (c == '*') {
        advance();
        return (Token){TOKEN_STAR, 0};
    }

    if (c == '/') {
        advance();
        return (Token){TOKEN_SLASH, 0};
    }

    if (c == '(') {
        advance();
        return (Token){TOKEN_LPAREN, 0};
    }

    if (c == ')') {
        advance();
        return (Token){TOKEN_RPAREN, 0};
    }

    if (c == ';') {
        advance();
        return (Token){TOKEN_SEMICOLON, 0};
    }

    advance();
    return (Token){TOKEN_INVALID, 0};
}
