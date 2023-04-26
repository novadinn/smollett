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
    N_AND,
    N_OR,
    N_ASSIGN,
    N_POSTINC, N_POSTDEC,

    N_IDENT,
    N_INTLIT, N_FLOATLIT, N_STRLIT, N_CHARLIT, N_STRUCTLIT,
    N_ARRAY, N_STRUCTMEM, N_STRUCTINIT,

    N_FUNCCALL, N_ARAC, N_MEMBAC, N_NOT,

    N_VAR, N_IF, N_ELSE, N_WHILE,
    N_FOR, N_FOREACH, N_FUN, N_RETURN,
    N_CONTINUE, N_BREAK, N_PRINT, N_STRUCT,
    N_USING,

    N_INT, N_CHAR, N_FLOAT, N_VOID, N_STRING,

    N_PRGRM, N_BLOCK,
};

struct AST_Node {
    OperationType op;
    int child_num;
    int child_start;
    int atable_index;
    int ttable_index;
}; 

std::vector<AST_Node> ast(const char* src);
std::vector<AST_Node> ast_build(const char* s);
void print_node(AST_Node* node);
void nextt();
void prevt();
AST_Node makenode(OperationType t, int tindex, int child_num, int child_start);
OperationType arithop(TokenType t);
int op_precedence(OperationType op);
bool rightassoc(TokenType t);
bool opb();
int ast_push(AST_Node node);
int ast_push(std::vector<AST_Node> v);

void match(TokenType t);
bool matchb(TokenType t);
void matchstr();
void matchtype();

void fatalln(const char* s);
void fatald(const char* s, const char* c);
void fatal(const char* s);
void fatalt(const Token* t, const char* s);

AST_Node prefix();
AST_Node postfix();
AST_Node literal();
AST_Node array_access();
AST_Node funccall();
AST_Node binexpr(int pr);
std::vector<AST_Node> global_statements();
std::vector<AST_Node> struct_statements();

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

std::vector<AST_Node> import_statement();
AST_Node while_statement();
AST_Node for_statement();
AST_Node foreach_statement();
AST_Node if_statement();
AST_Node return_statement();
AST_Node continue_statement();
AST_Node break_statement();
AST_Node print_statement();
AST_Node using_statement();

AST_Node fun_declaration();
AST_Node var_declaration(bool need_type = true);
AST_Node struct_member_declaration();
AST_Node fun_param_declaration();
AST_Node struct_declaration();
AST_Node ident_declaration(bool need_type = true);

AST_Node structlit();
AST_Node block();
AST_Node type();

#endif
