#include "ast.h"

extern char *tokstr[];
extern bool read_file(const char* path, char** buf);
extern bool file_exists_p(const char* path);

extern std::vector<int> tokens_intlit;
extern std::vector<char> tokens_charlit;
extern std::vector<char*> tokens_strlit;
extern std::vector<char*> tokens_ident;

const char *nodestr[] = {
    "*", "/",
    "+", "-",
    ">", "<", ">=", "<=",
    "==", "!=",
    "=",
    "v++", "v--",
    "++v", "--v",

    "ident",
    "intlit", "strlit", "charlit", "structlit",
    "array", "struct member", "struct initialisation",
    
    "funccall", "array access",

    "var", "const", "if", "else", "while",
    "do while", "for", "foreach", "fun", "return",
    "continue", "break", "print", "struct",
    "using",
    
    "int", "char", "float",

    "pgr", "block",
};

std::vector<AST_Node> ast_nodes;

Token* token;
AST_Node* tree;
int curt = -1;
std::vector<Token>* tokens;
const char* source;
int global_nodes_num = 0;

const int opprec[] = {
    5, 5, // * /
    4, 4, // + -
    3, 3, 3, 3, // > < >= <=
    2, 2, // == !==
    1, // =
};

void print_node(AST_Node* node) {
    printf("OperationType: %s\n", nodestr[(int)node->op]);
    printf("Token: %d\n", node->ttable_index);
    printf("AST table index: %d\n", node->atable_index);
    printf("Childs num: %d\n", node->child_num);
}

void ast_build(const char* src) {
    const char* tempsrc = source;
    std::vector<Token>* temptkns = tokens;
    int tempcurt = curt;
    Token* tempt = token;
    
    source = src;

    if(!file_exists_p(src)) {
	printf("nova: cannot find %s: no such file or directory\n", src);
	exit(1);
    }
    
    char* buf;
    if(!read_file(src, &buf)) {
	printf("nova: cannot read %s\n", src);
	exit(1);
    }
    int len = strlen(buf);

    std::vector<Token> tkns = lexer_scan(buf, len);
    for(int i = 0; i < tkns.size(); ++i) {
	Token token = tkns[i];
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
    tokens = &tkns;
    curt = -1;

    nextt();

    global_nodes_num += global_statements();
    
    free(buf);
    
    source = tempsrc;
    tokens = temptkns;
    curt = tempcurt;
    token = tempt;
}

std::vector<AST_Node> ast(const char* src) {
    ast_build(src);
    
    makenode(N_PRGRM, 0, 0);

    return ast_nodes;
}

void nextt() {
    if (curt == tokens->size())
	fatalln("recieved end of tokens when trying to get next token");
    curt++;
    token = &(*tokens)[curt];
}

void prevt() {
    if (curt == 0)
	fatalln("received start of tokens when trying to get prev token");
    curt--;
    token = &(*tokens)[curt];
}

void makenode(OperationType t, int tindex, int child_num) {
    AST_Node n;
    n.op = t;
    n.ttable_index = tindex;
    n.child_num = child_num;
    n.atable_index = ast_nodes.size();
    ast_nodes.push_back(n);
}

OperationType arithop(TokenType t) {
    OperationType o;
    switch(t) {
    case T_STAR:
	o = N_MULT;
	break;
    case T_SLASH:
	o = N_DIV;
	break;
    case T_PLUS:
	o = N_PLUS;
	break;
    case T_MINUS:
	o = N_MINUS;
	break;
    case T_GT:
	o = N_GT;
	break;
    case T_LT:
	o = N_LT;
	break;
    case T_GE:
	o = N_GE;
	break;
    case T_LE:
	o = N_LE;
	break;
    case T_EQ:
	o = N_EQ;
	break;
    case T_NE:
	o = N_NE;
	break;
    case T_ASSIGN:
	o = N_ASSIGN;
	break;
    
    default: fatald("unknown token type", tokstr[(int)t]);
    }
    return o;
}

int op_precedence(OperationType op) {
    if ((int)op > 11) fatalln("unknown operation precedence");

    return opprec[(int)op];
}

bool rightassoc(TokenType t) {
    if (t == T_ASSIGN) {
	return true;
    } else if (t == T_STAR) {
	return true;
    }
    return false;
}

bool opb() {
    return (int)token->type > 1 && (int)token->type < 13;
}

void match(TokenType t) {
    if (token->type != t)
	fatald("no matches for provided token type", tokstr[(int)t]);
    nextt();
}

bool matchb(TokenType t) {
    return (token->type == t);
}

void matchstr() {
    if (token->type != T_STRLIT)
	fatalln("token type doesn't match string literal type");
}

void matchtype() {
    if(token->type != T_INT
       && token->type != T_FLOAT
       && token->type != T_CHAR
       && token->type != T_IDENT)
	fatalln("token type doesn't match \"type\" type");
}

void fatalln(const char* s) {
    fprintf(stderr, "%s\n", s);
    exit(-1);
}

void fatald(const char* s, const char* c) {
    fprintf(stderr, "fatal: %s: %s\n", s, c);
    exit(-1);
}

void fatalt(const Token* t, const char* s) {
    fprintf(stderr, "%s:%d:%d %s\n", source, t->line, t->index, s);
}

void fatal(const char* s) {
    fprintf(stderr, "fatal: %s", s);
    exit(-1);
}

void prefix() {
    switch(token->type) {
    case T_INC:
	nextt();
	ident();
	makenode(N_PREINC, token->table_index, 0);
	nextt();
	break;
    case T_DEC:
	nextt();
	ident();
	makenode(N_PREDEC, token->table_index, 0);
	nextt();
	break;
    default: fatalt(token, "token type doesn't match prefix type");
    }    
}

void postfix() {
    int index = token->table_index;
    nextt();
    
    switch(token->type) {
    case T_LPAREN:
	funccall();
	break;
    case T_LBRACK:
	array_access();
	break;
    case T_INC:
	makenode(N_POSTINC, index, 0);
	nextt();
	break;
    case T_DEC:
	makenode(N_POSTDEC, index, 0);
	nextt();
	break;
    case T_LBRACE:
	structlit_declaration();
	makenode(N_STRUCTINIT, index, 1);
	break;
    default:
	makenode(N_IDENT, index, 0);
    }
}

void literal() {
    switch(token->type) {
    case T_INTLIT:
	makenode(N_INTLIT, token->table_index, 0);
	break;
    case T_CHARLIT:
	makenode(N_CHARLIT, token->table_index, 0);
	break;
    case T_STRLIT:
	makenode(N_STRLIT, token->table_index, 0);
	break;
    case T_LPAREN:
	nextt();

	binexpr(0);

	rparen();
	break;
    case T_LBRACE:
	structlit_declaration();
	break;
    case T_IDENT:
	return postfix();    
    default: return prefix();
    }
    
    nextt();
}

void array_access() {
    makenode(N_IDENT, token->table_index, 0);
    lbrack();

    binexpr(0);

    rbrack();

    makenode(N_ARAC, 0, 2);
}

void funccall() {
    makenode(N_IDENT, token->table_index, 0);

    lparen();

    int cn = 1;
    while(1) {
	if (token->type == T_COMMA) {
	    nextt();
	    binexpr(0);
	    cn++;
	} else if (token->type == T_RPAREN) {
	    nextt();
	    break;
	} else {
	    binexpr(0);
	    cn++;
	}
    }

    makenode(N_FUNCCALL, 0, cn);
}

void binexpr(int pr) {
    literal();
    
    if (!opb())
	return;

    OperationType op = arithop(token->type);

    while((op_precedence(op) > pr)
	  || (op_precedence(op) == pr) && rightassoc(token->type)) {
	int p = op_precedence(op);
	nextt();

	binexpr(p);
	makenode(op, 0, 2);
	
	if (!opb())
	    return;
	op = arithop(token->type);
    }
}


int global_statements() {
    int n = 0;
    while(1) {
	switch(token->type) {
	case T_IMPORT:
	    import_statement();
	    break;
	case T_VAR:
	    var_declaration();
	    semi();
	    break;
	case T_CONST:
	    const_declaration();
	    semi();
	    break;
	case T_IF:
	    if_statement();
	    break;
	case T_FOR:
	    for_statement();
	    break;
	case T_DO:
	    dowhile_statement();
	    break;
	case T_WHILE:
	    while_statement();
	    break;
	case T_FOREACH:
	    foreach_statement();
	    break;	
	case T_FUN:
	    fun_declaration();
	    break;
	case T_PRINT:
	    print_statement();
	    break;
	case T_STRUCT:
	    struct_declaration();
	    break;
	case T_EOF:
	    return n;
	default:
	    binexpr(0);
	    semi();
	}

	n++;
    }
}

int block_statements() {
    int n = 0;
    while(1) {
    switch(token->type) {
    case T_VAR:
	var_declaration();
	semi();
	break;
    case T_CONST:
	const_declaration();
	semi();
	break;
    case T_IF:
	if_statement();
	break;
    case T_FOR:
	for_statement();
	break;
    case T_DO:
	dowhile_statement();
	break;
    case T_WHILE:
	while_statement();
	break;
    case T_FOREACH:
	foreach_statement();
	break;	
    case T_FUN:
	fun_declaration();
	break;
    case T_RETURN:
	return_statement();
	break;
    case T_CONTINUE:
	continue_statement();
	break;
    case T_BREAK:
	break_statement();
	break;
    case T_PRINT:
	print_statement();
	break;
    case T_STRUCT:
	struct_declaration();
	semi();
	break;
    case T_RBRACE:
	return n;
    default:
	binexpr(0);
	semi();
    }
    
    n++;
    }
}

int struct_statements() {
    int n = 0;
    while(1) {
	switch(token->type) {
	case T_IDENT:
	    struct_member_declaration();
	    break;
	case T_FUN:
	    fun_declaration();
	    break;
	case T_USING:
	    using_statement();
	    break;
	case T_RBRACE:
	    return n;
	    break;
	default:
	    fatalt(token, "token doesn't match struct statement type");
	}
	
	n++;
    }
}

void ident() {
    if (token->type != T_IDENT)
	fatalln("token type doesn't match \"ident\" type");
}

void semi() {
    match(T_SEMI);
}

void colon() {
    match(T_COLON);
}

bool colonb() {
    return matchb(T_COLON);
}

void lbrack() {
    match(T_LBRACK);
}

void rbrack() {
    match(T_RBRACK);
}

void lparen() {
    match(T_LPAREN);
}

void rparen() {
    match(T_RPAREN);
}

void lbrace() {
    match(T_LBRACE);
}

void rbrace() {
    match(T_RBRACE);
}

void while_statement() {
    match(T_WHILE);

    lparen();
    binexpr(0);
    rparen();
    
    block();

    makenode(N_WHILE, 0, 2);
}

void dowhile_statement() {
    match(T_DO);

    block();

    match(T_WHILE);

    lparen();
    binexpr(0);
    rparen();
    
    semi();

    makenode(N_DOWHILE, 0, 2);
}

void for_statement() {
    match(T_FOR);

    int cn = 4;

    lparen();
    if (token->type == T_VAR) {
	var_declaration();
    } else {
	binexpr(0);
    }
    semi();
    binexpr(0);
    semi();
    binexpr(0);
    rparen();
    
    if (token->type == T_IDENT) {
	makenode(N_IDENT, token->table_index, 0);
	nextt();
	cn++;
    }

    block();

    makenode(N_FOR, 0, cn);
}

void foreach_statement() {
    match(T_FOREACH);

    lparen();
    var_declaration();

    match(T_IN);

    ident();
    makenode(N_IDENT, token->table_index, 0);
    nextt();
    rparen();
    
    block();
    makenode(N_FOREACH, 0, 3);
}

void import_statement() {
    match(T_IMPORT);

    matchstr();
    int index = token->table_index;
    nextt();
    
    semi();

    ast_build(tokens_strlit[index]);
}

void if_statement() {
    match(T_IF);

    int cn = 2;
    lparen();
    binexpr(0);
    rparen();
    
    block();

    if (token->type == T_ELSE) {	
	nextt();	

	if (token->type == T_IF) {
	    if_statement();	    
	} else {
	    block();	    
	}
		
	makenode(N_ELSE, 0, 1);
	cn++;
    }

    makenode(N_IF, 0, cn);
}

void return_statement() {
    match(T_RETURN);

    int cn = 0;
    if (token->type != T_SEMI) {
       
	binexpr(0);
	cn++;
    }
    
    semi();
    
    makenode(N_RETURN, 0, 1);
}

void continue_statement() {
    match(T_CONTINUE);

    int cn = 0;
    if (token->type == T_IDENT) {
	makenode(N_IDENT, token->table_index, 0);
	cn++;
	nextt();
    }

    semi();
    makenode(N_CONTINUE, 0, cn);
}

void break_statement() {
    match(T_BREAK);

    int cn = 0;
    if (token->type == T_IDENT) {
	makenode(N_IDENT, token->table_index, 0);
	cn++;
	nextt();
    }

    semi();
    makenode(N_BREAK, 0, cn);
}

void print_statement() {
    match(T_PRINT);

    lparen();

    binexpr(0);
    int cn = 1;
    while(1) {
	if (token->type == T_COMMA) {
	    nextt();
	    binexpr(0);
	    cn++;
	} else {
	    break;
	}
    }

    rparen();
    semi();
    
    makenode(N_PRINT, 0, cn);
}

void using_statement() {
    match(T_USING);

    ident();
    makenode(N_IDENT, token->table_index, 0);
    nextt();
    
    semi();

    makenode(N_USING, 0, 1);
}

void const_declaration() {
    match(T_CONST);

    int cn = 1;
    ident_declaration();

    while(1) {
	if (token->type == T_COMMA) {
	    nextt();
	    ident_declaration();
	    cn++;
	} else {
	    break;
	}
    }

    makenode(N_CONST, 0, cn);
}

void fun_declaration() {
    match(T_FUN);

    ident();
    makenode(N_IDENT, token->table_index, 0);
    nextt();

    lparen();

    int cn = 2;
    while(1) {	
	if (token->type == T_IDENT) {
	    fun_param_declaration();
	    cn++;
	} else if (token->type == T_COMMA) {
	    nextt();
	    fun_param_declaration();
	    cn++;
	} else {
	    break;
	}
    }

    rparen();

    if (token->type == T_ARROW) {
	nextt();
	type();
	cn++;
    }
    
    block();

    makenode(N_FUN, 0, cn);
}

void var_declaration() {
    match(T_VAR);

    int cn = 1;
    ident_declaration();
    while(1) {
	if (token->type == T_COMMA) {
	    nextt();
	    ident_declaration();	    
	    cn++;
	} else {
	    break;
	}
    }
    
    makenode(N_VAR, 0, cn);
}

void struct_member_declaration() {
    ident_declaration();
    
    int cn = 1;
    while(1) {
	if (token->type == T_COMMA) {
	    nextt();
	    ident_declaration();
	    cn++;
	} else if (token->type == T_SEMI) {
	    nextt();
	    break;
	} else {
	    ident_declaration();
	    cn++;
	}
    }

    makenode(N_STRUCTMEM, 0, cn);
}

void fun_param_declaration() {
    ident();
    int ident_index = token->table_index;
    nextt();
    
    int cn = 0;
    if (colonb()) {
	nextt();
	matchtype();
	type();
	cn++;
    }
    
    makenode(N_IDENT, ident_index, cn);
}

void struct_declaration() {
    match(T_STRUCT);

    ident();
    makenode(N_IDENT, token->table_index, 0);
    nextt();

    lbrace();

    int cn = struct_statements();
    makenode(N_BLOCK, 0, cn);
    
    rbrace();

    makenode(N_STRUCT, 0, 2);
}

void ident_declaration() {
    ident();
    int ident_index = token->table_index;
    bool arr = false;
    nextt();

    if (token->type == T_LBRACK) {
	nextt();
	binexpr(0);
	rbrack();
	arr = true;
    }
    
    int cn = 0;
    if (colonb()) {
	nextt();
	matchtype();
	type();
	cn++;
    }
    
    makenode(N_IDENT, ident_index, cn);
    
    if (arr) {
	makenode(N_ARRAY, 0, 1);

	if (token->type == T_LBRACE) {
	    structlit_declaration();
	    return;
	}	
    }
    
    if (token->type == T_ASSIGN) {
	nextt();
	binexpr(0);
	makenode(N_ASSIGN, 0, 2);
    }
}

void structlit_declaration() {
    lbrace();

    int cn = 0;
    while(1) {
	if (token->type == T_COMMA) {
	    nextt();
	    binexpr(0);
	    cn++;
	} else if (token->type == T_RBRACE) {
	    nextt();
	    break;
	} else {
	    binexpr(0);
	    cn++;
	}	
    }

    makenode(N_STRUCTLIT, 0, cn);
}

void block() {
    lbrace();

    int cn = block_statements();

    rbrace();
    makenode(N_BLOCK, 0, cn);
}

void type() {
    switch(token->type) {
    case T_INT:
	makenode(N_INT, 0, 0);
	break;
    case T_CHAR:
	makenode(N_CHAR, 0, 0);
	break;
    case T_FLOAT:
	makenode(N_FLOAT, 0, 0);
	break;
    case T_IDENT:
	makenode(N_IDENT, token->table_index, 0);
	break;
    default: fatalln("token type doesn't match \"type\" type");
    }
    nextt();
    if (token->type == T_LBRACK) {
	nextt();
	binexpr(0);
	rbrack();
	
	makenode(N_ARRAY, 0, 1);

	nextt();
    }    
}
