#include "read.h"

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <time.h>
#include <cstring>

const char *tokstr[] = { "NONE", "EOF",
	"+", "-",
	"*", "/",
	"==" , "!=",
	"<", ">", "<=", ">=",
	"intlit", "charlit", "strlit", ";", ":", ",", "=", "ident",
	"{", "}", "(", ")",
	"[", "]",
	"import", "struct", "var", "fun",
	"if", "else", "while", "for", "do", "return", "break", "continue",
	"const", "int", "char", "float",
	"print"};

extern std::vector<int> tokens_intlit;
extern std::vector<char> tokens_charlit;
extern std::vector<char*> tokens_strlit;
extern std::vector<char*> tokens_ident;

bool file_exists_p(const char* path);
bool read_file(const char* path, char** buf);

int main(int argc, char** argv) {
    if(argc != 2) {
	printf("nova: fatal error: no input files\n");
	exit(1);
    }

    const char* file_path = argv[1];
    if(!file_exists_p(file_path)) {
	printf("nova: cannot find %s: no such file or directory\n", file_path);
	exit(1);
    }

    clock_t start, end;
    long elapsed;
    
    start = clock();
    
    char* buf;
    if(!read_file(file_path, &buf)) {
	printf("nova: connat read %s\n", file_path);
	exit(1);
    }
    int len = strlen(buf);

    tokens_intlit_init(0);
    tokens_charlit_init(0);
    tokens_strlit_init(0);
    tokens_ident_init(0);
    
    std::vector<Token> tokens = lexer_scan(buf, len);
    for(int i = 0; i < tokens.size(); ++i) {
	Token token = tokens[i];
	printf("Token %s", tokstr[token.type]);
	if(token.type == T_INTLIT)
	    printf(", value %d", tokens_intlit[token.table_index]);
	else if(token.type == T_CHARLIT)
	    printf(", value %c", tokens_charlit[token.table_index]);
	else if(token.type == T_STRLIT)
	    printf(", value %s", tokens_strlit[token.table_index]);
	else if(token.type == T_IDENT)
	    printf(", name %s", tokens_ident[token.table_index]);
	printf("\n");
    }

    tokens_intlit_clear();
    tokens_charlit_clear();
    tokens_strlit_clear();
    tokens_ident_clear();
    
    free(buf);
    
    end = clock();

    elapsed = ((double)end - start) / CLOCKS_PER_SEC * 1000;
    printf("Elapsed: %ld ms \n", elapsed);
    
    return 0;
}

bool file_exists_p(const char* path) {
    return access(path, F_OK) == 0;
}

bool read_file(const char* path, char** buf) {
    FILE* f = fopen(path, "rb");
    if(f == NULL) {
	printf("file %s can't be opened\n", path);
	return false;
    }
    
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    (*buf) = (char*)malloc(fsize + 1);
    fread(*buf, fsize, 1, f);
    fclose(f);

    (*buf)[fsize] = EOF;

    return true;
}
