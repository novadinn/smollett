#ifndef EVAL_H
#define EVAL_H

#include "ast.h"
#include "env.h"

void eval(AST_Node program, int env_index);
SmolValue eval_rec(AST_Node node, int env_index);

SmolValue eval_program(AST_Node node, int env_index);
SmolValue eval_block(AST_Node node, int env_index);
SmolValue eval_function(AST_Node node, int env_index);
SmolValue eval_function_call(AST_Node node, int env_index);

SmolValue eval_return(AST_Node node, int env_index);
SmolValue eval_break(AST_Node node, int env_index);
SmolValue eval_continue(AST_Node node, int env_index);

SmolValue eval_var(AST_Node node, int env_index);
SmolValue eval_ident(AST_Node node, int env_index);

SmolValue eval_intlit(AST_Node node, int env_index);
SmolValue eval_floatlit(AST_Node node, int env_index);
SmolValue eval_charlit(AST_Node node, int env_index);
SmolValue eval_stringlit(AST_Node node, int env_index);

SmolValue eval_plus(AST_Node node, int env_index);
SmolValue eval_minus(AST_Node node, int env_index);
SmolValue eval_mult(AST_Node node, int env_index);
SmolValue eval_div(AST_Node node, int env_index);

SmolValue eval_if(AST_Node node, int env_index);
SmolValue eval_else(AST_Node node, int env_index);
SmolValue eval_assign(AST_Node node, int env_index);
SmolValue eval_for(AST_Node node, int env_index);
SmolValue eval_while(AST_Node node, int env_index);
SmolValue eval_foreach(AST_Node node, int env_index);
SmolValue eval_array_access(AST_Node node, int env_index);

SmolValue eval_greater_than(AST_Node node, int env_index);
SmolValue eval_less_than(AST_Node node, int env_index);
SmolValue eval_greater_than_or_equal(AST_Node node, int env_index);
SmolValue eval_less_than_or_equal(AST_Node node, int env_index);
SmolValue eval_equals(AST_Node node, int env_index);
SmolValue eval_not_equals(AST_Node node, int env_index);
SmolValue eval_and(AST_Node node, int env_index);
SmolValue eval_or(AST_Node node, int env_index);
SmolValue eval_not(AST_Node node, int env_index);

SmolValue eval_post_increment(AST_Node node, int env_index);
SmolValue eval_post_decrement(AST_Node node, int env_index);

SmolValue eval_print(AST_Node node, int env_index);

SmolValueType eval_ottonvt(OperationType type);
double eval_retrieve_number(SmolValue nv);
int eval_push_number_from_type(SmolValueType type, double value);
SmolValueType eval_dominant_type(SmolValueType left, SmolValueType right);

#endif // EVAL_H
