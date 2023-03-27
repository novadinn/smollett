#ifndef READ_H
#define READ_H

#include "token.h"

#include <vector>
#include <cstdio>

std::vector<Token> tokenizer_scan(FILE* fp);
Token tokenizer_read_token();
int tokenizer_read_int(char c);

char next_letter();
void put_back(char c);
char next_letter_skip();
char char_pos(char *s, char c);

#endif // READ_H
