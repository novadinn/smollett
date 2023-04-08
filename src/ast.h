#ifndef AST_H
#define AST_H

#include <vector>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "token.h"
#include "read.h"

enum OperationType {
    N_MULT, N_DIV,
    N_PLUS, N_MINUS,
    N_GT, N_LT, N_GE, N_LE,
    N_EQ, N_NE,
    N_ASSIGN,
    N_POSTINC, N_POSTDEC,
    N_PREINC, N_PREDEC,

    N_IDENT,
    N_INTLIT, N_STRLIT, N_CHARLIT, N_STRUCTLIT,
    N_ARRAY, N_STRUCTMEM, N_STRUCTINIT,

    N_FUNCCALL, N_ARAC,

    N_VAR, N_CONST, N_IF, N_ELSE, N_WHILE,
    N_DOWHILE, N_FOR, N_FOREACH, N_FUN, N_RETURN,
    N_CONTINUE, N_BREAK, N_PRINT, N_STRUCT,
    N_USING,

    N_INT, N_CHAR, N_FLOAT,

    N_PRGRM, N_BLOCK,
};

struct AST_Node {
    OperationType op;
    int child_num;
    int atable_index;
    int ttable_index;
};

std::vector<AST_Node> ast(const char* src);
void ast_build(const char* s);
void print_node(AST_Node* node);
void nextt();
void prevt();
void makenode(OperationType t, int tindex, int child_num);
OperationType arithop(TokenType t);
int op_precedence(OperationType op);
bool rightassoc(TokenType t);
bool opb();

void match(TokenType t);
bool matchb(TokenType t);
void matchstr();
void matchtype();

void fatalln(const char* s);
void fatald(const char* s, const char* c);
void fatal(const char* s);
void fatalt(const Token* t, const char* s);

void prefix();
void postfix();
void literal();
void array_access();
void funccall();
void binexpr(int pr);
int global_statements();
int block_statements();
int struct_statements();

void ident();
void semi();
void colon();
bool colonb();
void lbrack();
void rbrack();
void lparen();
void rparen();
void lbrace();
void rbrace();

void while_statement();
void dowhile_statement();
void for_statement();
void foreach_statement();
void import_statement();
void if_statement();
void return_statement();
void continue_statement();
void break_statement();
void print_statement();
void using_statement();

void const_declaration();
void fun_declaration();
void var_declaration();
void struct_member_declaration();
void fun_param_declaration();
void struct_declaration();
void ident_declaration();
void structlit_declaration();

void block();
void type();

#endif
