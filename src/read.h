#ifndef READ_H
#define READ_H

#include "token.h"

#include <vector>
#include <cstdio>

struct Lexer {
    char* source;
    int length;
    int index;

    int line;

    char putback;
};

Lexer* lexer_create(char* source, int length);
void lexer_delete(Lexer* lexer);

std::vector<Token> lexer_scan(char* src, int length);
Token lexer_read_token(Lexer* lexer);
int lexer_read_int(Lexer* lexer, char c);
int lexer_read_identifier(Lexer* lexer, char c, char* buf);
TokenType lexer_read_keyword(Lexer* lexer, char* s);

char lexer_next_letter(Lexer* lexer);
char lexer_next_letter_skip(Lexer* lexer);

char char_pos(char* s, char c);

#endif // READ_H
