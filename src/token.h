#ifndef TOKEN_H
#define TOKEN_H

// import, foreach, continue, return, break, struct
enum TokenType {
    T_NONE = 0, T_EOF,
    // +    -
    T_PLUS, T_MINUS,
    // *    /
    T_STAR, T_SLASH,
    // == !=
    T_EQ, T_NE,
    // <  >     <=    >=
    T_LT, T_GT, T_LE, T_GE,
    //        ;       =
    T_INTLIT, T_CHARLIT, T_STRLIT, T_SEMI, T_ASSIGN, T_IDENT,
    // {      }         (         )
    T_LBRACE, T_RBRACE, T_LPAREN, T_RPAREN,
    // [      ]        
    T_LBRACK, T_RBRACK,
    // Keywords
    T_IF, T_ELIF, T_ELSE, T_WHILE, T_FOR, T_DO, T_INT,
    T_PRINT
};

struct Token {
    TokenType type;
    int table_index;
};

#endif // TOKEN_H
