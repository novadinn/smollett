#include "read.h"

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <time.h>

const char *tokstr[] = { "EOF", "+", "-", "*", "/", "int" };

bool file_exists_p(const char* path) {
    return access(path, F_OK) == 0;
}

int main(int argc, char** argv) {
    if(argc != 2) {
	printf("nova: fatal error: no input files\n");
	return 1;
    }

    const char* file_path = argv[1];
    if(!file_exists_p(file_path)) {
	printf("nova: cannot find %s: no such file or directory\n", file_path);
	return 1;
    }

    FILE* fp = fopen(file_path, "rb");
    
    clock_t start, end;
    long elapsed;

    start = clock();
    std::vector<Token> tokens = tokenizer_scan(fp);
    for(int i = 0; i < tokens.size(); ++i) {
	Token token = tokens[i];
	printf("Token %s", tokstr[token.type]);
	if(token.type == T_INT)
	    printf(", value %d", token.int_value);
	printf("\n");
    }
    end = clock();

    elapsed = ((double)end - start) / CLOCKS_PER_SEC * 1000;
    printf("Elapsed: %ld ms \n", elapsed);

    fclose(fp);
    
    return 0;
}
