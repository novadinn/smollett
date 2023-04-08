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
};

Lexer* lexer_create(char* source, int length);
void lexer_delete(Lexer* lexer);

std::vector<Token> lexer_scan(char* src, int length);
Token lexer_read_token(Lexer* lexer);
int lexer_read_int(Lexer* lexer, char c);
char lexer_read_char(Lexer* lexer);
char* lexer_read_string(Lexer* lexer);
int lexer_read_identifier(Lexer* lexer, char c, char* buf);
TokenType lexer_read_keyword(Lexer* lexer, char* s);

char lexer_next_letter(Lexer* lexer);
char lexer_next_letter_skip(Lexer* lexer);

char char_pos(char* s, char c);

extern std::vector<int> tokens_intlit;
extern std::vector<char> tokens_charlit;
extern std::vector<char*> tokens_strlit;
extern std::vector<char*> tokens_ident;

void tokens_intlit_init(int length);
void tokens_intlit_push(Token* token, int val);
void tokens_intlit_clear();


void tokens_charlit_init(int length);
void tokens_charlit_push(Token* token, char val);
void tokens_charlit_clear();

void tokens_strlit_init(int length);
void tokens_strlit_push(Token* token, char* val);
void tokens_strlit_clear();

void tokens_ident_init(int length);
void tokens_ident_push(Token* token, char* val);
void tokens_ident_clear();

#endif // READ_H
