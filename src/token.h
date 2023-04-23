#ifndef TOKEN_H
#define TOKEN_H

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
    // &&
    T_AND,
    // ||
    T_OR,
    // =
    T_ASSIGN,
    // ++ --
    T_INC, T_DEC,
    //        ;       ->  .   
    T_INTLIT, T_FLOATLIT, T_CHARLIT, T_STRLIT, T_SEMI, T_COLON, T_COMMA, T_IDENT, T_ARROW, T_DOT,
    // !
    T_EXMARK,
    // {      }         (         )
    T_LBRACE, T_RBRACE, T_LPAREN, T_RPAREN,
    // [      ]        
    T_LBRACK, T_RBRACK,
    // Keywords
    T_IMPORT, T_STRUCT, T_VAR, T_FUN,
    T_IF, T_ELSE, T_WHILE, T_FOR, T_RETURN, T_BREAK, T_CONTINUE,
    T_INT, T_CHAR, T_FLOAT, T_VOID, T_STRING,
    T_PRINT, T_FOREACH, T_IN, T_USING,    
};

struct Token {
    TokenType type;
    int table_index = -1;
    int line;
    int index;
};

#endif // TOKEN_H
