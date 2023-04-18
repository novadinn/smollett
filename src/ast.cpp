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
    
    "funccall", "array access", "member access", "!",

    "var", "const", "if", "else", "while",
    "do while", "for", "foreach", "fun", "return",
    "continue", "break", "print", "struct",
    "using",
    
    "int", "char", "float", "void",

    "pgr", "block",
};

std::vector<AST_Node> ast_nodes;

Token* token;
AST_Node* tree;
int curt = -1;
std::vector<Token>* tokens;
const char* source;

const int opprec[] = {
    5, 5, // * /
    4, 4, // + -
    3, 3, 3, 3, // > < >= <=
    2, 2, // == !==
    1, // =
};

void print_node(AST_Node* node) {       
    printf("OperationType: %s\n", nodestr[(int)node->op]);
    // printf("Token: %d\n", node->ttable_index);
    // printf("AST table index: %d\n", node->atable_index);
    // printf("Childs num: %d\n", node->child_num);
    // printf("Childs start: %d\n", node->child_start);
    
    for (int j = 0; j < node->child_num; ++j) {
	print_node(&ast_nodes[node->child_start + j]);
    }
}

std::vector<AST_Node> ast_build(const char* src) {
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

    std::vector<AST_Node> nodes = global_statements();
    
    free(buf);
    
    source = tempsrc;
    tokens = temptkns;
    curt = tempcurt;
    token = tempt;

    return nodes;
}

std::vector<AST_Node> ast(const char* src) {
    std::vector<AST_Node> nodes = ast_build(src);

    int start = ast_push(nodes);
    
    ast_push(makenode(N_PRGRM, 0, nodes.size(), start));

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

AST_Node makenode(OperationType t, int tindex, int child_num, int child_start) {
    AST_Node n;
    n.op = t;
    n.ttable_index = tindex;
    n.child_num = child_num;
    n.child_start = child_start;
    // ast_nodes.push_back(n);
    return n;
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

int ast_push(AST_Node node) {
    int index = ast_nodes.size();
    node.atable_index = index;
    ast_nodes.push_back(node);
    return index;
}

int ast_push(std::vector<AST_Node> v) {
    int start = -1;
    for (int i = 0; i < v.size(); ++i) {
	int index = ast_push(v[i]);
	if (i == 0) {
	    start = index;
	}
    }
    return start;
}

void match(TokenType t) {
    printf("%s\n", tokstr[(int)token->type]);
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
       && token->type != T_VOID
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
    fprintf(stderr, "%s:%d:%d %s: %s\n", source, t->line, t->index, s, tokstr[(int)t->type]);
    exit(-1);
}

void fatal(const char* s) {
    fprintf(stderr, "fatal: %s", s);
    exit(-1);
}

AST_Node prefix() {
    AST_Node node;
    switch(token->type) {
    case T_INC:
	nextt();
	ident();
	node = makenode(N_PREINC, token->table_index, 0, -1);
	nextt();
	break;
    case T_DEC:
	nextt();
	ident();
	node = makenode(N_PREDEC, token->table_index, 0, -1);
	nextt();
	break;
    case T_EXMARK:
	nextt();
	node = makenode(N_NOT, 0, 1, ast_push(literal()));	
	break;
    default: fatalt(token, "token type doesn't match prefix type");
    }
    return node;
}

AST_Node postfix() {
    int index = token->table_index;
    nextt();

    AST_Node node;
    
    switch(token->type) {
    case T_LPAREN:
	node = funccall();
	break;
    case T_LBRACK:
	node = array_access();
	break;
    case T_INC:
	node = makenode(N_POSTINC, index, 0, -1);
	nextt();
	break;
    case T_DEC:
	node = makenode(N_POSTDEC, index, 0, -1);
	nextt();
	break;
    case T_DOT:
	nextt();
	node = makenode(N_MEMBAC, index, 1, ast_push(postfix()));
	break;
    case T_LBRACE:	
	node = makenode(N_STRUCTINIT, index, 1, ast_push(structlit_declaration()));
	break;
    default: node = makenode(N_IDENT, index, 0, -1);
    }

    return node;
}

AST_Node literal() {
    AST_Node node;

    switch(token->type) {
    case T_INTLIT:
	node = makenode(N_INTLIT, token->table_index, 0, -1);
	break;
    case T_CHARLIT:
	node = makenode(N_CHARLIT, token->table_index, 0, -1);
	break;
    case T_STRLIT:
	node = makenode(N_STRLIT, token->table_index, 0, -1);
	break;
    case T_LPAREN:
	nextt();

	node = binexpr(0);

	rparen();
	return node;
    case T_LBRACE:
	node = structlit_declaration();
	break;
    case T_IDENT:
	return postfix();
    default: return prefix();
    }
    
    nextt();
    
    return node;
}

AST_Node array_access() {
    std::vector<AST_Node> nodes;
    
    nodes.push_back(makenode(N_IDENT, token->table_index, 0, -1));
    lbrack();

    nodes.push_back(binexpr(0));

    rbrack();

    int start = ast_push(nodes);

    return makenode(N_ARAC, 0, nodes.size(), start);
}

AST_Node funccall() {
    std::vector<AST_Node> nodes;
    nodes.push_back(makenode(N_IDENT, token->table_index, 0, -1));

    lparen();

    while(1) {
	if (token->type == T_COMMA) {
	    nextt();
	    nodes.push_back(binexpr(0));
	} else if (token->type == T_RPAREN) {
	    nextt();
	    break;
	} else {
	    nodes.push_back(binexpr(0));
	}
    }

    int start = ast_push(nodes);

    return makenode(N_FUNCCALL, 0, nodes.size(), start);
}

AST_Node binexpr(int pr) {
    AST_Node left = literal();
    
    if (!opb())
	return left;

    OperationType op = arithop(token->type);

    while((op_precedence(op) > pr)
	  || (op_precedence(op) == pr) && rightassoc(token->type)) {
	std::vector<AST_Node> nodes;
	
	int p = op_precedence(op);
	nextt();

	nodes.push_back(left);
	nodes.push_back(binexpr(p));

	int start = ast_push(nodes);
	
	left = makenode(op, 0, nodes.size(), start);
	
	if (!opb())
	    return left;
	op = arithop(token->type);
    }

    return left;
}


std::vector<AST_Node> global_statements() {
    std::vector<AST_Node> nodes;

    bool run = true;
    while(run) {
	AST_Node node;
	switch(token->type) {
	case T_IMPORT: {	   
	    std::vector<AST_Node> imp_nodes = import_statement();
	    nodes.insert(nodes.end(), imp_nodes.begin(), imp_nodes.end());	    
	    continue;
	}
	case T_VAR:
	    node = var_declaration();
	    semi();
	    break;
	case T_CONST:
	    node = const_declaration();
	    semi();
	    break;
	case T_IF:
	    node = if_statement();
	    break;
	case T_FOR:
	    node = for_statement();
	    break;
	case T_DO:
	    node = dowhile_statement();
	    break;
	case T_WHILE:
	    node = while_statement();
	    break;
	case T_FOREACH:
	    node = foreach_statement();
	    break;	
	case T_FUN:
	    node = fun_declaration();
	    break;
	case T_PRINT:
	    node = print_statement();
	    break;
	case T_STRUCT:
	    node = struct_declaration();
	    break;
	case T_EOF:
	    run = false;
	    continue;
	default:
	    node = binexpr(0);
	    semi();
	}

	nodes.push_back(node);
    }

    return nodes;
}

std::vector<AST_Node> struct_statements() {
    std::vector<AST_Node> nodes;

    bool run = true;
    while(run) {
	AST_Node node;
	switch(token->type) {
	case T_IDENT:
	    node = struct_member_declaration();
	    break;
	case T_FUN:
	    node = fun_declaration();
	    break;
	case T_USING:
	    node = using_statement();
	    break;
	case T_RBRACE:
	    run = false;
	    continue;
	default:
	    fatalt(token, "token doesn't match struct statement type");
	}
	nodes.push_back(node);
    }

    return nodes;
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

AST_Node while_statement() {
    std::vector<AST_Node> nodes;
    match(T_WHILE);

    lparen();
    nodes.push_back(binexpr(0));
    rparen();
    
    nodes.push_back(block());

    int start = ast_push(nodes);

    return makenode(N_WHILE, 0, nodes.size(), start);
}

AST_Node dowhile_statement() {
    std::vector<AST_Node> nodes;
    match(T_DO);

    nodes.push_back(block());

    match(T_WHILE);

    lparen();
    nodes.push_back(binexpr(0));
    rparen();
    
    semi();

    int start = ast_push(nodes);
    
    return makenode(N_DOWHILE, 0, nodes.size(), start);
}

AST_Node for_statement() {
    std::vector<AST_Node> nodes;
    match(T_FOR);

    lparen();
    if (token->type == T_VAR) {
	nodes.push_back(var_declaration());
    } else {
	nodes.push_back(binexpr(0));
    }
    semi();
    nodes.push_back(binexpr(0));
    semi();
    nodes.push_back(binexpr(0));
    rparen();
    
    if (token->type == T_IDENT) {
	nodes.push_back(makenode(N_IDENT, token->table_index, 0, -1));
	nextt();
    }

    nodes.push_back(block());

    int start = ast_push(nodes);
    
    return makenode(N_FOR, 0, nodes.size(), start);
}

AST_Node foreach_statement() {
    std::vector<AST_Node> nodes;
    match(T_FOREACH);

    lparen();
    nodes.push_back(var_declaration(false));

    match(T_IN);

    ident();
    nodes.push_back(makenode(N_IDENT, token->table_index, 0, -1));
    nextt();
    rparen();
    
    nodes.push_back(block());

    int start = ast_push(nodes);
    
    return makenode(N_FOREACH, 0, nodes.size(), start);
}

std::vector<AST_Node> import_statement() {
    match(T_IMPORT);

    matchstr();
    int index = token->table_index;
    nextt();
    
    semi();

    return ast_build(tokens_strlit[index]);
}

AST_Node if_statement() {
    std::vector<AST_Node> nodes;
    match(T_IF);

    lparen();
    nodes.push_back(binexpr(0));
    rparen();
    
    nodes.push_back(block());

    if (token->type == T_ELSE) {	
	nextt();	

	int start;
	if (token->type == T_IF) {
	    start = ast_push(if_statement());	    
	} else {
	    start = ast_push(block());
	}
		
	nodes.push_back(makenode(N_ELSE, 0, 1, start));	
    }

    int start = ast_push(nodes);

    return makenode(N_IF, 0, nodes.size(), start);
}

AST_Node return_statement() {
    std::vector<AST_Node> nodes;
    match(T_RETURN);

    if (token->type != T_SEMI) {       
	nodes.push_back(binexpr(0));
    }
    
    semi();

    int start = ast_push(nodes);
    
    return makenode(N_RETURN, 0, nodes.size(), start);
}

AST_Node continue_statement() {
    std::vector<AST_Node> nodes;
    match(T_CONTINUE);

    if (token->type == T_IDENT) {
	nodes.push_back(makenode(N_IDENT, token->table_index, 0, -1));
	nextt();
    }

    semi();

    int start = ast_push(nodes);
    
    return makenode(N_CONTINUE, 0, nodes.size(), start);
}

AST_Node break_statement() {
    std::vector<AST_Node> nodes;
    match(T_BREAK);

    if (token->type == T_IDENT) {
	nodes.push_back(makenode(N_IDENT, token->table_index, 0, -1));
	nextt();
    }

    semi();

    int start = ast_push(nodes);
    
    return makenode(N_BREAK, 0, nodes.size(), start);
}

AST_Node print_statement() {
    std::vector<AST_Node> nodes;
    match(T_PRINT);

    lparen();

    nodes.push_back(binexpr(0));
    
    while(1) {
	if (token->type == T_COMMA) {
	    nextt();
	    nodes.push_back(binexpr(0));
	} else {
	    break;
	}
    }

    rparen();
    semi();

    int start = ast_push(nodes);
    
    return makenode(N_PRINT, 0, nodes.size(), start);
}

AST_Node using_statement() {
    match(T_USING);

    ident();
    int start = ast_push(makenode(N_IDENT, token->table_index, 0, -1));
    nextt();
    
    semi();

    return makenode(N_USING, 0, 1, start);
}

AST_Node const_declaration(bool need_type) {
    match(T_CONST);

    std::vector<AST_Node> nodes;
    
    nodes.push_back(ident_declaration(need_type));

    while(1) {
	if (token->type == T_COMMA) {
	    nextt();
	    nodes.push_back(ident_declaration(need_type));	    
	} else {
	    break;
	}
    }

    int start = ast_push(nodes);

    return makenode(N_CONST, 0, nodes.size(), start);
}

AST_Node fun_declaration() {
    match(T_FUN);

    std::vector<AST_Node> nodes;
    
    ident();
    nodes.push_back(makenode(N_IDENT, token->table_index, 0, -1));
    nextt();

    lparen();

    while(1) {
	if (token->type == T_IDENT) {
	    nodes.push_back(fun_param_declaration());
	} else if (token->type == T_COMMA) {
	    nextt();
	    nodes.push_back(fun_param_declaration());	    
	} else {
	    break;
	}
    }

    rparen();

    if (token->type == T_ARROW) {
	nextt();
	nodes.push_back(type());
    } else {
	nodes.push_back(makenode(N_VOID, 0, 0, 0));
    }
    
    nodes.push_back(block());

    int start = ast_push(nodes);
    
    return makenode(N_FUN, 0, nodes.size(), start);
}

AST_Node var_declaration(bool need_type) {
    match(T_VAR);

    std::vector<AST_Node> nodes;
    
    nodes.push_back(ident_declaration(need_type));
    while(1) {
	if (token->type == T_COMMA) {
	    nextt();
	    nodes.push_back(ident_declaration(need_type));   
	} else {
	    break;
	}
    }

    int start = ast_push(nodes);
    
    return makenode(N_VAR, 0, nodes.size(), start);
}

AST_Node struct_member_declaration() {
    std::vector<AST_Node> nodes;

    nodes.push_back(ident_declaration());
    
    while(1) {
	if (token->type == T_COMMA) {
	    nextt();
	    nodes.push_back(ident_declaration());
	} else if (token->type == T_SEMI) {
	    nextt();
	    break;
	} else {
	    nodes.push_back(ident_declaration());
	}
    }

    int start = ast_push(nodes);

    return makenode(N_STRUCTMEM, 0, nodes.size(), start);
}

AST_Node fun_param_declaration() {
    ident();
    int ident_index = token->table_index;
    nextt();

    std::vector<AST_Node> nodes;
    
    if (colonb()) {
	nextt();
	matchtype();
	nodes.push_back(type());
    }

    int start = ast_push(nodes);
    
    return makenode(N_IDENT, ident_index, nodes.size(), start);
}

AST_Node struct_declaration() {
    match(T_STRUCT);

    std::vector<AST_Node> nodes;
    
    ident();
    nodes.push_back(makenode(N_IDENT, token->table_index, 0, -1));
    nextt();

    lbrace();

    std::vector<AST_Node> statements = struct_statements();
    int start = ast_push(statements);

    rbrace();
    
    nodes.push_back(makenode(N_BLOCK, 0, statements.size(), start));

    start = ast_push(nodes);

    return makenode(N_STRUCT, 0, nodes.size(), start);
}

AST_Node ident_declaration(bool need_type) {
    std::vector<AST_Node> ident_nodes;
    std::vector<AST_Node> arr_nodes;
    
    ident();
    int ident_index = token->table_index;
    bool arr = false;
    bool have_type = !need_type;
    nextt();

    if (token->type == T_LBRACK) {
	nextt();
	arr_nodes.push_back(binexpr(0));
	rbrack();
	arr = true;
    }
    
    if (colonb()) {
	nextt();
	matchtype();
	ident_nodes.push_back(type());
	have_type = have_type || true;
    }

    int id_start = ast_push(ident_nodes);
    
    AST_Node ident = makenode(N_IDENT, ident_index, ident_nodes.size(), id_start);
    
    if (arr) {
	arr_nodes.push_back(ident);

	bool init = false;
	if (token->type == T_LBRACE) {
	    arr_nodes.push_back(structlit_declaration());
	    init = true;
	}

	int arr_start = ast_push(arr_nodes);
	
	ident = makenode(N_ARRAY, 0, arr_nodes.size(), arr_start);
	if (init) {
	    return ident;
	}
    }
    
    if (token->type == T_ASSIGN) {
	std::vector<AST_Node> nodes;
	nodes.push_back(ident);
	
	nextt();
	nodes.push_back(binexpr(0));

	int start = ast_push(nodes);
	
	ident = makenode(N_ASSIGN, 0, nodes.size(), start);
	have_type = have_type || true;
    }

    if (!have_type) {
	fatalln("no type is provided for ident declaration");
    }

    return ident;
}

AST_Node structlit_declaration() {
    lbrace();

    std::vector<AST_Node> nodes;
    
    while(1) {
	if (token->type == T_COMMA) {
	    nextt();
	    nodes.push_back(binexpr(0));
	} else if (token->type == T_RBRACE) {
	    nextt();
	    break;
	} else {
	    nodes.push_back(binexpr(0));
	}	
    }

    int start = ast_push(nodes);

    return makenode(N_STRUCTLIT, 0, nodes.size(), start);
}

AST_Node block() {
    lbrace();

    std::vector<AST_Node> nodes;

    bool run = true;
    while(run) {
	AST_Node node;	
	switch(token->type) {
	case T_VAR:
	    node = var_declaration();
	    semi();
	    break;
	case T_CONST:
	    node = const_declaration();
	    semi();
	    break;
	case T_IF:
	    node = if_statement();
	    break;
	case T_FOR:
	    node = for_statement();
	    break;
	case T_DO:
	    node = dowhile_statement();
	    break;
	case T_WHILE:
	    node = while_statement();
	    break;
	case T_FOREACH:
	    node = foreach_statement();
	    break;	
	case T_FUN:
	    node = fun_declaration();
	    break;
	case T_RETURN:
	    node = return_statement();
	    break;
	case T_CONTINUE:
	    node = continue_statement();
	    break;
	case T_BREAK:
	    node = break_statement();
	    break;
	case T_PRINT:
	    node = print_statement();
	    break;
	case T_STRUCT:
	    node = struct_declaration();
	    break;
	case T_RBRACE:
	    nextt();
	    run = false;
	    continue;
	default:
	    node = binexpr(0);
	    semi();
	}
    
	nodes.push_back(node);
    }

    int start = ast_push(nodes);

    return makenode(N_BLOCK, 0, nodes.size(), start);
}

AST_Node type() {
    AST_Node node;
    switch(token->type) {
    case T_INT:
	node = makenode(N_INT, 0, 0, -1);
	break;
    case T_CHAR:
	node = makenode(N_CHAR, 0, 0, -1);
	break;
    case T_FLOAT:
	node = makenode(N_FLOAT, 0, 0, -1);
	break;
    case T_VOID:
	node = makenode(N_VOID, 0, 0, 0);
	break;
    case T_IDENT:
	node = makenode(N_IDENT, token->table_index, 0, -1);
	break;
    default: fatalln("token type doesn't match \"type\" type");
    }
    nextt();
    if (token->type == T_LBRACK) {
	std::vector<AST_Node> nodes;
	nodes.push_back(node);
	
	nextt();
	nodes.push_back(binexpr(0));
	rbrack();

	int start = ast_push(nodes);      

	nextt();

	return makenode(N_ARRAY, 0, nodes.size(), start);
    }

    return node;
}
