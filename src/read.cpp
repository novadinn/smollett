#include "read.h"

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <ctype.h>

Lexer* lexer_create(char* source, int length) {
    Lexer* lexer = (Lexer*)malloc(sizeof(*lexer));
    lexer->source = source;
    lexer->length = length;
    lexer->index = 0;
    lexer->line = 0;
    lexer->putback = 0;
    
    return lexer;
}

void lexer_delete(Lexer* lexer) {
    free(lexer);
}

std::vector<Token> lexer_scan(char* src, int length) {
    Lexer* lexer = lexer_create(src, length);
    
    std::vector<Token> token_stream;
    Token token;

    while((token = lexer_read_token(lexer)).type != TokenType::T_EOF) {
	token_stream.push_back(token);
    }
    token_stream.push_back(token);

    lexer_delete(lexer);
    
    return token_stream;
}

Token lexer_read_token(Lexer* lexer) {
    Token token;
    char c = lexer_next_letter_skip(lexer);
    
    switch(c) {
    case EOF: {
	token.type = TokenType::T_EOF;
    } break;
    case ';': {
	token.type = TokenType::T_SEMI;
    } break;
    case '=': {
	if((c = lexer_next_letter(lexer)) == '=') {
	    token.type = TokenType::T_EQ;
	} else {
	    lexer->putback = c;
	    token.type = TokenType::T_ASSIGN;
	}
    } break;
    case '!': {
	if((c = lexer_next_letter(lexer)) == '=') {
	    token.type = TokenType::T_NE;
	} else {
	    printf("unrecognised character %d\n", c);
	    exit(1);
	}
    } break;
    case '<': {
	if((c = lexer_next_letter(lexer)) == '=') {
	    token.type = TokenType::T_LE;
	} else {
	    lexer->putback = c;
	    token.type = TokenType::T_LT;
	}
    } break;
    case '>': {
	if((c = lexer_next_letter(lexer)) == '=') {
	    token.type = TokenType::T_GE;
	} else {
	    lexer->putback = c;
	    token.type = TokenType::T_GT;
	}
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
	    token.type = TokenType::T_INTLIT;
	    token.int_value = lexer_read_int(lexer, c);
	} else if(isalpha(c) || c == '_') {
	    char buf[IDENTIFIER_MAX_LENGTH];
	    TokenType type;
	    
	    lexer_read_identifier(lexer, c, buf);

	    if((type = lexer_read_keyword(lexer, buf)) != TokenType::T_NONE) {
		token.type = type;
		break;
	    }
	    
	    // printf("nova: unrecognised symbol %s on line %d\n", buf, lexer->line);
	    // exit(1);
	    token.type = TokenType::T_IDENT;
	    strcpy(token.identifier, buf);
	    break;
	} else {
	    printf("nova: unrecognised character %c on line %d\n", c, lexer->line);
	    exit(1);
	}
    } break;
    };

    return token;
}

int lexer_read_int(Lexer* lexer, char c) {
    int k, val = 0;

    while ((k = char_pos("0123456789", c)) >= 0) {
	val = val * 10 + k;
	c = lexer_next_letter(lexer);
    }

    // NOTE: we need to put it back since we have already readed next character, so
    // next_letter cannot read it again, we need to force if to do so
    lexer->putback = c;
    return val;
}

int lexer_read_identifier(Lexer* lexer, char c, char* buf) {
    int i = 0;

    while(isalpha(c) || isdigit(c) || c == '_') {
	if(i == (IDENTIFIER_MAX_LENGTH - 1)) {
	    printf("nova: identifier too long on line %d\n", lexer->line);
	    exit(1);
	}

	buf[i++] = c;
	c = lexer_next_letter(lexer);
    }

    lexer->putback = c;
    buf[i] = '\0';
    return i;
}

TokenType lexer_read_keyword(Lexer* lexer, char* s) {
    switch(*s) {
    case 'i': {
	if(!strcmp(s, "int"))
	    return TokenType::T_INT;
    } break;
    case 'p': {
	if(!strcmp(s, "print"))
	    return TokenType::T_PRINT;
    } break;
    };

    return TokenType::T_NONE;
}

char lexer_next_letter(Lexer* lexer) {
    char c;

    if(lexer->putback) {
	c = lexer->putback;
	lexer->putback = 0;
	return c;
    }

    c = lexer->source[lexer->index++];
    if(c == '\n')
	++lexer->line;

    return c;
}

char lexer_next_letter_skip(Lexer* lexer) {
    char c = lexer_next_letter(lexer);

    while(c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f')
	c = lexer_next_letter(lexer);

    return c;
}

char char_pos(char* s, char c) {
    char* p;
    
    p = strchr(s, c);
    return (p ? p - s : -1);
}
