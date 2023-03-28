#ifndef TOKEN_H
#define TOKEN_H

#include <vector>

enum TokenType {
    T_NONE = 0, T_EOF,
    T_SEMI,
    T_ASSIGN, T_EQ, T_NE, T_LT, T_GT, T_LE, T_GE,
    T_PLUS, T_MINUS, T_STAR, T_SLASH,
    T_PRINT,
    T_IDENT,
    T_INT,
    T_INTLIT
};

#define IDENTIFIER_MAX_LENGTH 255

// TODO: instead of this, use tables (vector<int> tokens_intlit, vector<char*> tokens_ident),
// so that we dont need to strore additional 255 bytes for a non-identifier
struct Token {
    TokenType type;
    int int_value;
    char identifier[IDENTIFIER_MAX_LENGTH];
};

#endif // TOKEN_H
