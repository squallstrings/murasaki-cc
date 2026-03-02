#ifndef TOKEN_H
#define TOKEN_H

typedef enum {
    TOKEN_PRINT,
    TOKEN_NUMBER,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_SEMICOLON,
    TOKEN_EOF,
    TOKEN_INVALID,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
} TokenType;

typedef struct {
    TokenType type;
    long value;  // only used for numbers
} Token;

#endif
