#include "read.h"

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <ctype.h>

FILE* infile;
int current_line = 0;
char putback = 0;

std::vector<Token> tokenizer_scan(FILE* fp) {
    infile = fp;

    std::vector<Token> token_stream;
    Token token;

    while((token = tokenizer_read_token()).type != TokenType::T_EOF) {
	token_stream.push_back(token);
    }
    token_stream.push_back(token);

    return token_stream;
}

Token tokenizer_read_token() {
    Token token;
    char c = next_letter_skip();

    switch(c) {
    case EOF: {
	token.type = TokenType::T_EOF;
    } break;
    case '+': {
	token.type = TokenType::T_PLUS;
    } break;
    case '-': {
	token.type = TokenType::T_MINUS;
    } break;
    case '*': {
	token.type = TokenType::T_STAR;
    } break;
    case '/': {
	token.type = TokenType::T_SLASH;
    } break;
    default: {
	if(isdigit(c)) {
	    token.type = TokenType::T_INT;
	    token.int_value = tokenizer_read_int(c);
	}
    } break;
    };

    return token;
}

int tokenizer_read_int(char c) {
    int k, val = 0;

    while ((k = char_pos("0123456789", c)) >= 0) {
	val = val * 10 + k;
	c = next_letter();
    }

    // NOTE: we need to put it back since we have already readed next character, so
    // next_letter cannot read it again, we need to force if to do so
    put_back(c);
    return val;
}

char next_letter() {
    char c;

    if(putback) {
	c = putback;
	putback = 0;
	return c;
    }

    c = fgetc(infile);
    if(c == '\n')
	++current_line;

    return c;
}

void put_back(char c) {
    putback = c;
}

char next_letter_skip() {
    char c = next_letter();

    while(c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f')
	c = next_letter();

    return c;
}

char char_pos(char *s, char c) {
    char *p;
    
    p = strchr(s, c);
    return (p ? p - s : -1);
}
