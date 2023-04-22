#ifndef EVAL_H
#define EVAL_H

#include "ast.h"
#include "env.h"

void eval(AST_Node program, int env_index);
NovaValue eval_rec(AST_Node node, int env_index);

NovaValue eval_program(AST_Node node, int env_index);
NovaValue eval_block(AST_Node node, int env_index);
NovaValue eval_function(AST_Node node, int env_index);
NovaValue eval_function_call(AST_Node node, int env_index);

NovaValue eval_return(AST_Node node, int env_index);
NovaValue eval_break(AST_Node node, int env_index);
NovaValue eval_continue(AST_Node node, int env_index);

NovaValue eval_var(AST_Node node, int env_index);
NovaValue eval_ident(AST_Node node, int env_index);

NovaValue eval_intlit(AST_Node node, int env_index);
NovaValue eval_floatlit(AST_Node node, int env_index);
NovaValue eval_charlit(AST_Node node, int env_index);

NovaValue eval_plus(AST_Node node, int env_index);
NovaValue eval_minus(AST_Node node, int env_index);
NovaValue eval_mult(AST_Node node, int env_index);
NovaValue eval_div(AST_Node node, int env_index);

NovaValue eval_if(AST_Node node, int env_index);
NovaValue eval_else(AST_Node node, int env_index);
NovaValue eval_assign(AST_Node node, int env_index);
NovaValue eval_for(AST_Node node, int env_index);
NovaValue eval_while(AST_Node node, int env_index);

NovaValue eval_greater_than(AST_Node node, int env_index);
NovaValue eval_less_than(AST_Node node, int env_index);
NovaValue eval_greater_than_or_equal(AST_Node node, int env_index);
NovaValue eval_less_than_or_equal(AST_Node node, int env_index);
NovaValue eval_equals(AST_Node node, int env_index);
NovaValue eval_not_equals(AST_Node node, int env_index);
NovaValue eval_and(AST_Node node, int env_index);
NovaValue eval_or(AST_Node node, int env_index);
NovaValue eval_not(AST_Node node, int env_index);

NovaValue eval_post_increment(AST_Node node, int env_index);
NovaValue eval_post_decrement(AST_Node node, int env_index);

NovaValue eval_print(AST_Node node, int env_index);

NovaValueType eval_ottonvt(OperationType type);
double eval_retrieve_number(NovaValue nv);
int eval_push_number_from_type(NovaValueType type, double value);
NovaValueType eval_dominant_type(NovaValueType left, NovaValueType right);

#endif // EVAL_H
