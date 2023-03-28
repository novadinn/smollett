#include "read.h"

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <time.h>
#include <cstring>

const char *tokstr[] = { "NONE", "EOF",
    ";",
    "=", "==", "!=", "<", ">", "<=", ">=",
    "+", "-", "*", "/",
    "print",
    "ident",
    "int",
    "intlit" };

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
    
    std::vector<Token> tokens = lexer_scan(buf, len);
    for(int i = 0; i < tokens.size(); ++i) {
	Token token = tokens[i];
	printf("Token %s", tokstr[token.type]);
	if(token.type == T_INTLIT)
	    printf(", value %d", token.int_value);
	else if(token.type == T_IDENT)
	    printf(", name %s", token.identifier);
	printf("\n");
    }

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
