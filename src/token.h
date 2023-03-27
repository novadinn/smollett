#ifndef TOKEN_H
#define TOKEN_H

enum TokenType {
    T_EOF, T_PLUS, T_MINUS, T_STAR, T_SLASH, T_INT
};

struct Token {
    TokenType type;
    int int_value;
};

#endif // TOKEN_H
