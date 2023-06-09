#include "read.h"

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <ctype.h>

#define IDENTIFIER_MAX_LENGTH 255
#define MAX_TEXT_LENGTH 2048

std::vector<int> tokens_intlit;
std::vector<float> tokens_floatlit;
std::vector<char> tokens_charlit;
std::vector<char*> tokens_strlit;
std::vector<char*> tokens_ident;

Lexer* lexer_create(char* source, int length) {
    Lexer* lexer = (Lexer*)malloc(sizeof(*lexer));
    lexer->source = source;
    lexer->length = length;
    lexer->index = 0;
    lexer->line = 0;
    
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
    case '+': {
	if((c = lexer_next_letter(lexer)) == '+') {
	    token.type = TokenType::T_INC;
	} else {
	    lexer->index--;
	    token.type = TokenType::T_PLUS;	    
	}
    } break;
    case '-': {
	c = lexer_next_letter(lexer);
	if(c == '>') {
	    token.type = TokenType::T_ARROW;
	} else if (c == '-') {
	    token.type = TokenType::T_DEC;
	} else if(isdigit(c)) {
	    bool has_decimal;
	    double val = lexer_read_number(lexer, c, &has_decimal);
	    if(has_decimal) {
		token.type = TokenType::T_FLOATLIT;
		tokens_floatlit_push(&token, -val);
	    } else {
		token.type = TokenType::T_INTLIT;
		tokens_intlit_push(&token, -(int)val);
	    }
	} else {
	    lexer->index--;
	    token.type = TokenType::T_MINUS;
	}
    } break;
    case '*': {
	token.type = TokenType::T_STAR;
    } break;
    case '/': {
	c = lexer_next_letter(lexer);
	if (c == '/') {
	    lexer_skip_line(lexer);
	    return lexer_read_token(lexer);
	} else if (c == '*') {
	    while(1) {
		c = lexer_next_letter(lexer);
		if (c == EOF) {
		    printf("unexpected end of file\n");
		    exit(1);
		}
		if(c == '*') {
		    if ((c = lexer_next_letter(lexer)) == '/') {
			return lexer_read_token(lexer);
		    } else {
			lexer->index--;
		    }
		} 
	    }
	} else {
	    lexer->index--;
	    token.type = TokenType::T_SLASH;
	}	
    } break;
    case '=': {
	if((c = lexer_next_letter(lexer)) == '=') {
	    token.type = TokenType::T_EQ;
	} else {
	    lexer->index--;
	    token.type = TokenType::T_ASSIGN;
	}
    } break;
    case '!': {
	if((c = lexer_next_letter(lexer)) == '=') {
	    token.type = TokenType::T_NE;
	} else {
	    lexer->index--;
	    token.type = TokenType::T_EXMARK;
	}
    } break;
    case '<': {
	if((c = lexer_next_letter(lexer)) == '=') {
	    token.type = TokenType::T_LE;
	} else {
	    lexer->index--;
	    token.type = TokenType::T_LT;
	}
    } break;
    case '>': {
	if((c = lexer_next_letter(lexer)) == '=') {
	    token.type = TokenType::T_GE;
	} else {
	    lexer->index--;
	    token.type = TokenType::T_GT;
	}
    } break;
    case '&': {
	if ((c = lexer_next_letter(lexer)) == '&') {
	    token.type = TokenType::T_AND;
	} else {
	    printf("invalid token\n");
	    exit(1);
	}	
    } break;
    case '|': {
	if ((c = lexer_next_letter(lexer)) == '|') {
	    token.type = TokenType::T_OR;
	} else {
	    printf("invalid token\n");
	    exit(1);
	}	
    } break;
    case ';': {
	token.type = TokenType::T_SEMI;
    } break;
    case '.': {
	token.type = TokenType::T_DOT;
    } break;
    case ':': {
	token.type = TokenType::T_COLON;
    } break;
    case ',': {
	token.type = TokenType::T_COMMA;
    } break;
    case '{': {
	token.type = TokenType::T_LBRACE;
    } break;
    case '}': {
	token.type = TokenType::T_RBRACE;
    } break;
    case '(': {
	token.type = TokenType::T_LPAREN;
    } break;
    case ')': {
	token.type = TokenType::T_RPAREN;
    } break;
    case '[': {
	token.type = TokenType::T_LBRACK;
    } break;
    case ']': {
	token.type = TokenType::T_RBRACK;
    } break;
    case '\'': {
	token.type = TokenType::T_CHARLIT;
	tokens_charlit_push(&token, lexer_read_char(lexer));
	
	if(lexer_next_letter(lexer) != '\'') {
	    printf("nova: exprected '\\'' at end of char literal\n");
	    exit(1);
	}
    } break;
    case '"': {
	token.type =  T_STRLIT;
	tokens_strlit_push(&token, lexer_read_string(lexer));
    } break;
    default: {
	if(isdigit(c)) {
	    bool has_decimal;
	    double val = lexer_read_number(lexer, c, &has_decimal);
	    if(has_decimal) {
		token.type = TokenType::T_FLOATLIT;
		tokens_floatlit_push(&token, val);
	    } else {
		token.type = TokenType::T_INTLIT;
		tokens_intlit_push(&token, (int)val);
	    }	    
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
	    char* ident = (char*)malloc(sizeof(*ident) * IDENTIFIER_MAX_LENGTH);
	    strcpy(ident, buf);
	    tokens_ident_push(&token, ident);
	    break;
	} else {
	    printf("nova: unrecognised character %c on line %d\n", c, lexer->line);
	    exit(1);
	}
    } break;
    };

    token.line = lexer->line;
    token.index = lexer->index;
    
    return token;
}

double lexer_read_number(Lexer* lexer, char c, bool* has_decimal) {
    int k, val = 0;
    float fval = 0;
    int num_digits = 0;
    int decimal_pos = -1;

    *has_decimal = false;

    while((k = char_pos("0123456789.", c)) >= 0) {
	if(c == '.') {
	    *has_decimal = true;
	    if(decimal_pos >= 0) {
		printf("nova: invalid float value\n");
		exit(1);
	    }

	    decimal_pos = num_digits;
	} else {
	    val = val * 10 + k;
	    num_digits++;
	}
	
	c = lexer_next_letter(lexer);
    }

    if(decimal_pos >= 0) {
	int divisor = 1;
	for(int i = 0; i < (num_digits - decimal_pos); i++) {
	    divisor *= 10;
	}

	fval = (float)val / (float)divisor;
    } else {
	fval = (float)val;
    }

    lexer->index--;
    return fval;
}

char lexer_read_char(Lexer* lexer) {
    char c;
    
    c = lexer_next_letter(lexer);
    if (c == '\\') {
	switch(c = lexer_next_letter(lexer)) {
	case 'a':  return '\a';
	case 'b':  return '\b';
	case 'f':  return '\f';
	case 'n':  return '\n';
	case 'r':  return '\r';
	case 't':  return '\t';
	case 'v':  return '\v';
	case '\\': return '\\';
	case '"':  return '"' ;
	case '\'': return '\'';
	default:
	    printf("unknown escape sequence %d\n", c);
	    exit(1);
	}
    }

    return c;
}

char* lexer_read_string(Lexer* lexer) {
    int i;
    char c;
    char* buf = (char*)malloc(sizeof(buf) * MAX_TEXT_LENGTH);

    for(i = 0; i < MAX_TEXT_LENGTH - 1; ++i) {
	if ((c = lexer_read_char(lexer)) == '"') {
	    buf[i] = 0;
	    return buf;
	}
	buf[i] = c;
    }

    printf("nova: string literal too long on line%d\n", lexer->line);
    exit(1);
    
    return NULL;
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

    lexer->index--;
    buf[i] = '\0';
    return i;
}

TokenType lexer_read_keyword(Lexer* lexer, char* s) {    
    switch(*s) {
    case 'i': {
	if(!strcmp(s, "int"))
	    return TokenType::T_INT;
	if(!strcmp(s, "if"))
	    return TokenType::T_IF;
	if(!strcmp(s, "import"))
	    return TokenType::T_IMPORT;
	if(!strcmp(s, "in"))
	    return TokenType::T_IN;
    } break;
    case 'p': {
	if(!strcmp(s, "print"))
	    return TokenType::T_PRINT;
    } break;
    case 'w': {
	if(!strcmp(s, "while"))
	    return TokenType::T_WHILE;
    } break;
    case 'f': {
	if(!strcmp(s, "float"))
	    return TokenType::T_FLOAT;
	if(!strcmp(s, "for"))
	    return TokenType::T_FOR;
	if(!strcmp(s, "fun"))
	    return TokenType::T_FUN;
	if(!strcmp(s, "foreach"))
	    return TokenType::T_FOREACH;
    } break;    
    case 's': {
	if(!strcmp(s, "struct"))
	    return TokenType::T_STRUCT;
	if(!strcmp(s, "string"))
	    return TokenType::T_STRING;
    } break;
    case 'v': {
	if(!strcmp(s, "var"))
	    return TokenType::T_VAR;
	if(!strcmp(s, "void"))
	    return TokenType::T_VOID;
    } break;
    case 'r': {
	if(!strcmp(s, "return"))
	    return TokenType::T_RETURN;
    } break;
    case 'b': {
	if(!strcmp(s, "break"))
	    return TokenType::T_BREAK;
    } break;
    case 'c': {
	if(!strcmp(s, "char"))
	    return TokenType::T_CHAR;
	if(!strcmp(s, "continue"))
	    return TokenType::T_CONTINUE;
    } break;
    case 'e': {
	if(!strcmp(s, "else"))
	    return TokenType::T_ELSE;
    } break;
    case 'u': {
	if(!strcmp(s, "using"))
	    return TokenType::T_USING;
    } break;
    };

    return TokenType::T_NONE;
}

char lexer_next_letter(Lexer* lexer) {
    char c;

    // if(lexer->putback) {
    // 	c = lexer->putback;
    // 	lexer->putback = 0;
    // 	return c;
    // }

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

void lexer_skip_line(Lexer* lexer) {
    int line = lexer->line;

    while(line == lexer->line)
	lexer_next_letter(lexer);
}

char char_pos(char* s, char c) {
    char* p;
    
    p = strchr(s, c);
    return (p ? p - s : -1);
}

void tokens_intlit_init(int length) {
    tokens_intlit.reserve(length);
}
void tokens_intlit_push(Token* token, int val) {
    tokens_intlit.push_back(val);
    token->table_index = tokens_intlit.size()-1;
}
void tokens_intlit_clear() {
    tokens_intlit.clear();
}

void tokens_floatlit_init(int length) {
    tokens_floatlit.reserve(length);
}
void tokens_floatlit_push(Token* token, float val) {
    tokens_floatlit.push_back(val);
    token->table_index = tokens_floatlit.size()-1;
}
void tokens_floatlit_clear() {
    tokens_floatlit.clear();
}

void tokens_charlit_init(int length) {
    tokens_charlit.reserve(length);
}
void tokens_charlit_push(Token* token, char val) {
    tokens_charlit.push_back(val);
    token->table_index = tokens_charlit.size()-1;
}
void tokens_charlit_clear() {
    tokens_charlit.clear();
}

void tokens_ident_init(int length) {
    tokens_ident.reserve(length);
}
void tokens_ident_push(Token* token, char* val) {
    tokens_ident.push_back(val);
    token->table_index = tokens_ident.size()-1;
}
void tokens_ident_clear() {
    for(int i = 0; i < tokens_ident.size(); ++i) {
	free(tokens_ident[i]);
    }
    tokens_ident.clear();
}

void tokens_strlit_init(int length) {
    tokens_strlit.reserve(length);
}
void tokens_strlit_push(Token* token, char* val) {
    tokens_strlit.push_back(val);
    token->table_index = tokens_strlit.size()-1;
}
void tokens_strlit_clear() {
    for(int i = 0; i < tokens_strlit.size(); ++i) {
	free(tokens_strlit[i]);
    }
    tokens_strlit.clear();
}
