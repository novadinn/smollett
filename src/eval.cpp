#include "eval.h"

#include "log.h"

extern std::vector<int> tokens_intlit;
extern std::vector<float> tokens_floatlit;
extern std::vector<char> tokens_charlit;
extern std::vector<char*> tokens_strlit;
extern std::vector<char*> tokens_ident;

extern std::vector<AST_Node> ast_nodes;
extern std::vector<Enviroinment> envs;

// TODO:
// N_STRUCTLIT, N_STRUCTMEM, N_STRUCTINIT, N_MEMBAC, N_STRUCT,
// N_USING

void eval(AST_Node program, int env_index) {
	eval_rec(program, env_index);
}

// TODO: we are not clearing the envs and nova_* vectors
NovaValue eval_rec(AST_Node node, int env_index) {
	switch(node.op) {
	case OperationType::N_PRGRM: {
		return eval_program(node, env_index);
	} break;
	case OperationType::N_BLOCK: {
		return eval_block(node, env_index);
	} break;
	case OperationType::N_FUN: {
		return eval_function(node, env_index);
	} break;
	case OperationType::N_FUNCCALL: {
		return eval_function_call(node, env_index);
	} break;
	case OperationType::N_RETURN: {
		return eval_return(node, env_index);
	} break;
	case OperationType::N_BREAK: {
		return eval_break(node, env_index);
	} break;
	case OperationType::N_CONTINUE: {
		return eval_continue(node, env_index);
	} break;
	case OperationType::N_VAR: {
		return eval_var(node, env_index);
	} break;
	case OperationType::N_IDENT: {
		return eval_ident(node, env_index);
	} break;
	case OperationType::N_INTLIT: {
		return eval_intlit(node, env_index);
	};
	case OperationType::N_FLOATLIT: {
		return eval_floatlit(node, env_index);
	} break;
	case OperationType::N_CHARLIT: {
		return eval_charlit(node, env_index);
	};
	case OperationType::N_STRLIT: {
		return eval_stringlit(node, env_index);
	} break;
	case OperationType::N_PLUS: {
		return eval_plus(node, env_index);
	} break;
	case OperationType::N_MINUS: {
		return eval_minus(node, env_index);
	} break;
	case OperationType::N_MULT: {
		return eval_mult(node, env_index);
	} break;
	case OperationType::N_DIV: {
		return eval_div(node, env_index);
	} break;
	case OperationType::N_IF: {
		return eval_if(node, env_index);
	} break;
	case OperationType::N_ELSE: {
		return eval_else(node, env_index);
	} break;
	case OperationType::N_ASSIGN: {
		return eval_assign(node, env_index);
	} break;
	case OperationType::N_FOR: {
		return eval_for(node, env_index);
	} break;
	case OperationType::N_WHILE: {
		return eval_while(node, env_index);
	} break;
	case OperationType::N_FOREACH: {
		return eval_foreach(node, env_index);
	} break;
	case OperationType::N_ARAC: {
		return eval_array_access(node, env_index);
	} break;
	case OperationType::N_GT: {
		return eval_greater_than(node, env_index);
	};
	case OperationType::N_LT: {
		return eval_less_than(node, env_index);
	};
	case OperationType::N_GE: {
		return eval_greater_than_or_equal(node, env_index);
	};
	case OperationType::N_LE: {
		return eval_less_than_or_equal(node, env_index);
	};
	case OperationType::N_EQ: {
		return eval_equals(node, env_index);
	};
	case OperationType::N_NE: {
		return eval_not_equals(node, env_index);
	};
	case OperationType::N_AND: {
		return eval_and(node, env_index);
	} break;
	case OperationType::N_OR: {
		return eval_or(node, env_index);
	} break;
	case OperationType::N_NOT: {
		return eval_not(node, env_index);
	} break;
	case OperationType::N_POSTINC: {
		return eval_post_increment(node, env_index);
	} break;
	case OperationType::N_POSTDEC: {
		return eval_post_decrement(node, env_index);
	} break;
	case OperationType::N_PRINT: {
		return eval_print(node, env_index);
	} break;
	};

	return NovaValue{E_UNKNOWN};
}

NovaValue eval_program(AST_Node node, int env_index) {
	// Evaluate every element
	for(int i = node.child_start; i < node.child_start + node.child_num; ++i) {
		eval_rec(ast_nodes[i], env_index);
	}

	return NovaValue{E_UNKNOWN};
}

const char *nodestr2[] = {
    "*", "/",
    "+", "-",
    ">", "<", ">=", "<=",
    "==", "!=",
    "&&",
    "||",
    "=",
    "v++", "v--",    

    "ident",
    "intlit", "floatlit", "strlit", "charlit", "structlit",
    "array", "struct member", "struct initialisation",
    
    "funccall", "array access", "member access", "!",

    "var", "if", "else", "while",
    "for", "foreach", "fun", "return",
    "continue", "break", "print", "struct",
    "using",
    
    "int", "char", "float", "void", "string",

    "pgr", "block",
};

NovaValue eval_block(AST_Node node, int env_index) {
	// Evaluate every element
	for(int i = node.child_start; i < node.child_start + node.child_num; ++i) {
		// Store the last evaluated element, and return it after
		NovaValue fin;
		
		AST_Node element = ast_nodes[i];
		fin = eval_rec(element, env_index);

		// Stop at marker
		switch(fin.payload) {
		case P_RETURN: {
			return fin;
		} break;
		case P_BREAK: {
			return fin;
		};
		case P_CONTINUE: {
			return fin;
		};
		};
	}

	return NovaValue{NovaValueType::E_UNKNOWN};
}

NovaValue eval_function(AST_Node node, int env_index) {
	// Create function's local env
	int local_env_index = envs_push(env_index);

	NovaFunData data;

	AST_Node name_node = ast_nodes[node.child_start];
	
	// Get the function name
	char* fn_name = tokens_ident[name_node.ttable_index];
	// Foreach function argument
	for(int i = node.child_start+1; i < node.child_start + node.child_num-2; ++i) {
		AST_Node arg_node = ast_nodes[i];
		// Type of the argument
		AST_Node type_node = ast_nodes[arg_node.child_start];
		
		char* arg_name = tokens_ident[arg_node.ttable_index];
		NovaValueType type = eval_ottonvt(type_node.op);

		// Store the unbound argument in the local env
		env_push_unbound(local_env_index, arg_name, type);

		data.types.push_back(type);
	}
	AST_Node return_value = ast_nodes[node.child_start+node.child_num-2];
	AST_Node block = ast_nodes[node.child_start+node.child_num-1];

	data.return_value = eval_ottonvt(return_value.op);
	data.block = block;
	data.env_index = local_env_index;

	int fn_index = nova_funs_push(data);
	NovaValue result = {NovaValueType::E_FUN, fn_index};

	// Store the function in the outer env
	env_push_value(env_index, fn_name, result);

	return result;
}

NovaValue eval_function_call(AST_Node node, int env_index) {
	AST_Node name_node = ast_nodes[node.child_start];
	// Get the function name
	char* fn_name = tokens_ident[name_node.ttable_index];

	// Search the function in the env
	NovaValue function = envs_search(fn_name, env_index);
	ASSERT_TYPE(fn_name, function.type, E_FUN, "function");
	ASSERT_UNBOUND(function, fn_name);
	
	NovaFunData data = nova_funs[function.index];
	NovaValueType return_value_type = data.return_value;
	AST_Node block = data.block;
	int function_env_index = data.env_index;

	// Function argument count
	int argc = node.child_num-1;
	ASSERT_FUN_ARGC(fn_name, argc, (int)data.types.size());
	
	// Get the arguments
	for(int i = node.child_start + 1; i < node.child_start + node.child_num; ++i) {
		int env_types_index = i - (node.child_start + 1);
		
		AST_Node arg_node = ast_nodes[i];

		// Evaluate the argument
		NovaValue eval_arg = eval_rec(arg_node, env_index);

		NovaValueType got = eval_arg.type;
		NovaValueType expected = data.types[env_types_index];

		if(got != expected) {
			// TODO: check if got type can be converted to the expected type. If not, log error
		}

		env_set_unbound(function_env_index, env_types_index, eval_arg);
	}
	
	NovaValue eval_result = eval_rec(block, function_env_index);
	if(eval_result.type != return_value_type) {
		// TODO: check if got type can be converted to the expected type. If not, log error
	}

	return eval_result;
}

NovaValue eval_return(AST_Node node, int env_index) {
	AST_Node return_value = ast_nodes[node.child_start];
	NovaValue eval_value = eval_rec(return_value, env_index);
	eval_value.payload = NovaPayloadFlag::P_RETURN;

	return eval_value;
}

NovaValue eval_break(AST_Node node, int env_index) {
	NovaValue result;
	result.payload = P_BREAK;

	return result;
}

NovaValue eval_continue(AST_Node node, int env_index) {
	NovaValue result;
	result.payload = P_CONTINUE;
	
	return result;
}

NovaValue eval_var(AST_Node node, int env_index) {
	// Store the last evaluated value
	NovaValue fin;
	// Loop over multiple definitions (var a = 0, b = 0;)
	for(int i = node.child_start; i < node.child_start + node.child_num; ++i) {
		AST_Node child = ast_nodes[i];
		NovaValue result;
		// Variable with a value
		if(child.op == OperationType::N_ASSIGN) {
			AST_Node lhs = ast_nodes[child.child_start];	
			AST_Node rhs = ast_nodes[child.child_start+1];

			char* var_name = tokens_ident[lhs.ttable_index];
			result = eval_rec(rhs, env_index);
			// Type is specified
			if(lhs.child_num == 1) {
				AST_Node type_node = ast_nodes[lhs.child_start];
				// TODO: check if type is correct. If not, log error
			}

			env_push_value(env_index, var_name, result);
		} else if(child.op == OperationType::N_IDENT) { // Variable with a specified type, with no value
			AST_Node type = ast_nodes[child.child_start];

			char* var_name = tokens_ident[child.ttable_index];
			result = NovaValue{eval_ottonvt(type.op)};

			env_push_value(env_index, var_name, result);
		} else if(child.op == OperationType::N_ARRAY) {
			AST_Node num_node = ast_nodes[child.child_start];
			AST_Node ident_node = ast_nodes[child.child_start+1];
			AST_Node type_node = ast_nodes[ident_node.child_start];
			char* var_name = tokens_ident[ident_node.ttable_index];

			// Evaluate the result in case size value is an ident or an expression
			NovaValue len_value = eval_rec(num_node, env_index);
			ASSERT_IS_NUMBER(len_value, var_name);
			int num_elements = (int)eval_retrieve_number(len_value);

			// Reserve elements
			std::vector<NovaValue> vec;
			vec.reserve(num_elements);
			for(int i = 0; i < num_elements; ++i) {
				NovaValue element = NovaValue{eval_ottonvt(type_node.op)};
				vec.push_back(element);
			}

			result = NovaValue{NovaValueType::E_ARRAY};
			// Store the array
			result.index = nova_arrays_push(vec);
			
			env_push_value(env_index, var_name, result);
		}

		fin = result;
	}

	return fin;
}

NovaValue eval_ident(AST_Node node, int env_index) {
	char* sym = tokens_ident[node.ttable_index];

	NovaValue result = envs_search(sym, env_index);
	ASSERT_UNBOUND(result, sym);
	
	return result;
}

NovaValue eval_intlit(AST_Node node, int env_index) {
	NovaValue result = NovaValue{NovaValueType::E_INTLIT};
	// Get the value from the tokens table
	int value = tokens_intlit[node.ttable_index];
	result.index = nova_integers_push(value);

	return result;
}

NovaValue eval_floatlit(AST_Node node, int env_index) {
	NovaValue result = NovaValue{NovaValueType::E_FLOATLIT};
	// Get the value from the tokens table
	float value = tokens_floatlit[node.ttable_index];
	result.index = nova_floats_push(value);

	return result;
}

NovaValue eval_charlit(AST_Node node, int env_index) {
	NovaValue result = NovaValue{NovaValueType::E_CHARLIT};
	// Get the value from the tokens table
	int value = tokens_charlit[node.ttable_index];
	result.index = nova_chars_push(value);

	return result;
}

NovaValue eval_stringlit(AST_Node node, int env_index) {
	NovaValue result = NovaValue{NovaValueType::E_STRINGLIT};
	// Get the value from the tokens table
	char* value = tokens_strlit[node.ttable_index];
	result.index = nova_strings_push(value);

	return result;
}

NovaValue eval_plus(AST_Node node, int env_index) {
	AST_Node lhs = ast_nodes[node.child_start];
	AST_Node rhs = ast_nodes[node.child_start+1];

	NovaValue left_nv = eval_rec(lhs, env_index);
	ASSERT_IS_NUMBER(left_nv, "+ argument");
	NovaValue right_nv = eval_rec(rhs, env_index);
	ASSERT_IS_NUMBER(right_nv, "+ argument");
	
	double left_val = eval_retrieve_number(left_nv);
	double right_val = eval_retrieve_number(right_nv);

	NovaValueType type = eval_dominant_type(left_nv.type, right_nv.type);

	NovaValue result = NovaValue{type};
	result.index = eval_push_number_from_type(type, left_val + right_val);
	
	return result;
}

NovaValue eval_minus(AST_Node node, int env_index) {
	AST_Node lhs = ast_nodes[node.child_start];
	AST_Node rhs = ast_nodes[node.child_start+1];

	NovaValue left_nv = eval_rec(lhs, env_index);
	ASSERT_IS_NUMBER(left_nv, "- argument");
	NovaValue right_nv = eval_rec(rhs, env_index);
	ASSERT_IS_NUMBER(right_nv, "- argument");

	double left_val = eval_retrieve_number(left_nv);
	double right_val = eval_retrieve_number(right_nv);

	NovaValueType type = eval_dominant_type(left_nv.type, right_nv.type);

	NovaValue result = NovaValue{type};
	result.index = eval_push_number_from_type(type, left_val - right_val);
	
	return result;
}

NovaValue eval_mult(AST_Node node, int env_index) {
	AST_Node lhs = ast_nodes[node.child_start];
	AST_Node rhs = ast_nodes[node.child_start+1];

	NovaValue left_nv = eval_rec(lhs, env_index);
	ASSERT_IS_NUMBER(left_nv, "* argument");
	NovaValue right_nv = eval_rec(rhs, env_index);
	ASSERT_IS_NUMBER(right_nv, "* argument");

	double left_val = eval_retrieve_number(left_nv);
	double right_val = eval_retrieve_number(right_nv);

	NovaValueType type = eval_dominant_type(left_nv.type, right_nv.type);

	NovaValue result = NovaValue{type};
	result.index = eval_push_number_from_type(type, left_val * right_val);
	
	return result;
}

NovaValue eval_div(AST_Node node, int env_index) {
	AST_Node lhs = ast_nodes[node.child_start];
	AST_Node rhs = ast_nodes[node.child_start+1];

	NovaValue left_nv = eval_rec(lhs, env_index);
	ASSERT_IS_NUMBER(left_nv, "/ argument");
	NovaValue right_nv = eval_rec(rhs, env_index);
	ASSERT_IS_NUMBER(right_nv, "/ argument");

	double left_val = eval_retrieve_number(left_nv);
	double right_val = eval_retrieve_number(right_nv);

	NovaValueType type = eval_dominant_type(left_nv.type, right_nv.type);

	NovaValue result = NovaValue{type};
	result.index = eval_push_number_from_type(type, left_val / right_val);
	
	return result;
}

NovaValue eval_if(AST_Node node, int env_index) {
	NovaValue cond = eval_rec(ast_nodes[node.child_start], env_index);
	ASSERT_TYPE("if condition", cond.type, E_CHARLIT, "char");
	char result = nova_chars[cond.index];

	if(result) {
		// TODO: remove this env after evaluating the result
		int local_env_index = envs_push(env_index);
		
		AST_Node block = ast_nodes[node.child_start+1];

		return eval_rec(block, local_env_index);
	} else if(node.child_num == 3) { // Have else/else if clause
		AST_Node clause = ast_nodes[node.child_start+2];
		
		return eval_else(clause, env_index);
	}
	
	return NovaValue{NovaValueType::E_UNKNOWN};
}

NovaValue eval_else(AST_Node node, int env_index) {
	AST_Node next = ast_nodes[node.child_start];

	if(next.op == OperationType::N_IF) {
		// Else if clause
		return eval_if(next, env_index);
	} else if(next.op == OperationType::N_BLOCK) {
		// Else clause
		// TODO: remove this env after evaluating the result
		int local_env_index = envs_push(env_index);

		return eval_rec(next, local_env_index);
	}
	
	return NovaValue{NovaValueType::E_UNKNOWN};
}

NovaValue eval_assign(AST_Node node, int env_index) {
	NovaValue result = NovaValue{NovaValueType::E_UNKNOWN};

	AST_Node left = ast_nodes[node.child_start];
	AST_Node right = ast_nodes[node.child_start+1];
	if(left.op == OperationType::N_IDENT) {
		char* name = tokens_ident[left.ttable_index];

		NovaValue nv = envs_search(name, env_index);
		ASSERT_UNBOUND(nv, name);

		result = eval_rec(right, env_index);

		env_set_by_name(env_index, name, result);
	} else if(left.op == OperationType::N_ARAC) {
		AST_Node ident_node = ast_nodes[left.child_start];
		AST_Node index_node = ast_nodes[left.child_start+1];

		NovaValue size_value = eval_rec(index_node, env_index);
		ASSERT_IS_NUMBER(size_value, "array access");
		
		char* name = tokens_ident[ident_node.ttable_index];
		// int index = nova_integers[size_value.index];
		int index = (int)eval_retrieve_number(size_value); 

		NovaValue nv = envs_search(name, env_index);
		ASSERT_UNBOUND(nv, name);

		std::vector<NovaValue> vec = nova_arrays[nv.index];
		
		result = eval_rec(right, env_index);

		// NOTE: seems like we dont need to update an env
		nova_arrays[nv.index][index] = result;
	}

	return result;
}

NovaValue eval_for(AST_Node node, int env_index) {
	// Variable declaration
	AST_Node declare = ast_nodes[node.child_start];
	// Loop termination condition
	AST_Node cond = ast_nodes[node.child_start+1];
	AST_Node post = ast_nodes[node.child_start+2];
	AST_Node block = ast_nodes[node.child_start+3];

	int local_env_index = envs_push(env_index);

	// Eval the declaration
	eval_rec(declare, local_env_index);
	for(;;) {
		NovaValue cond_nv = eval_rec(cond, local_env_index);
		ASSERT_TYPE("for condition", cond_nv.type, E_CHARLIT, "char");

		char val = nova_chars[cond_nv.index];

		// Condition is not accomplished
		if(val == 0) {
			break;
		}

		// Evaluated the loop body
		NovaValue result = eval_rec(block, local_env_index);
		switch(result.payload) {
		case NovaPayloadFlag::P_RETURN: {
			return result;
		} break;
		case NovaPayloadFlag::P_BREAK: {
			return NovaValue{NovaValueType::E_UNKNOWN};
		} break;
		case NovaPayloadFlag::P_CONTINUE: {
			// Do nothing, since result's evaluation is already stopped
		} break;
		};
		
		eval_rec(post, local_env_index);
	}

	return NovaValue{NovaValueType::E_UNKNOWN};
}

NovaValue eval_while(AST_Node node, int env_index) {
	AST_Node cond = ast_nodes[node.child_start];
	AST_Node block = ast_nodes[node.child_start+1];

	int local_env_index = envs_push(env_index);

	for(;;) {
		NovaValue cond_nv = eval_rec(cond, local_env_index);
		ASSERT_TYPE("for condition", cond_nv.type, E_CHARLIT, "char");

		char val = nova_chars[cond_nv.index];

		// Condition accomplished
		if(val != 0) {
			break;
		}

		NovaValue result = eval_rec(block, local_env_index);
		switch(result.payload) {
		case NovaPayloadFlag::P_RETURN: {
			return result;
		} break;
		case NovaPayloadFlag::P_BREAK: {
			return NovaValue{NovaValueType::E_UNKNOWN};
		} break;
		case NovaPayloadFlag::P_CONTINUE: {
			// Do nothing, since result's evaluation is already stopped
		} break;
		};
	}
	
	return NovaValue{NovaValueType::E_UNKNOWN};
}

// TODO:
NovaValue eval_foreach(AST_Node node, int env_index) {
	return NovaValue{NovaValueType::E_UNKNOWN};
}

NovaValue eval_array_access(AST_Node node, int env_index) {
	AST_Node ident_node = ast_nodes[node.child_start];
	AST_Node index_node = ast_nodes[node.child_start+1];

	char* name = tokens_ident[ident_node.ttable_index];
	
	// Eval the index in case of an ident or an expression
	NovaValue index_value = eval_rec(index_node, env_index);
	ASSERT_IS_NUMBER(index_value, "array access");
	int index = eval_retrieve_number(index_value); // nova_integers[index_value.index];
   
	NovaValue nv = envs_search(name, env_index);
	ASSERT_UNBOUND(nv, "name");

	return nova_arrays[nv.index][index];
}

NovaValue eval_greater_than(AST_Node node, int env_index) {
	AST_Node lhs = ast_nodes[node.child_start];
	AST_Node rhs = ast_nodes[node.child_start+1];

	NovaValue left_nv = eval_rec(lhs, env_index);
	ASSERT_IS_NUMBER(left_nv, "> operand");
	NovaValue right_nv = eval_rec(rhs, env_index);
	ASSERT_IS_NUMBER(right_nv, "> operand");

	double left_val = eval_retrieve_number(left_nv);
	double right_val = eval_retrieve_number(right_nv);

	NovaValue result = NovaValue{NovaValueType::E_CHARLIT};
	result.index = nova_chars_push(left_val > right_val);

	return result;
}

NovaValue eval_less_than(AST_Node node, int env_index) {
	AST_Node lhs = ast_nodes[node.child_start];
	AST_Node rhs = ast_nodes[node.child_start+1];

	NovaValue left_nv = eval_rec(lhs, env_index);
	ASSERT_IS_NUMBER(left_nv, "< operand");
	NovaValue right_nv = eval_rec(rhs, env_index);
	ASSERT_IS_NUMBER(right_nv, "< operand");

	double left_val = eval_retrieve_number(left_nv);
	double right_val = eval_retrieve_number(right_nv);

	NovaValue result = NovaValue{NovaValueType::E_CHARLIT};
	result.index = nova_chars_push(left_val < right_val);

	return result;
}

NovaValue eval_greater_than_or_equal(AST_Node node, int env_index) {
	AST_Node lhs = ast_nodes[node.child_start];
	AST_Node rhs = ast_nodes[node.child_start+1];

	NovaValue left_nv = eval_rec(lhs, env_index);
	ASSERT_IS_NUMBER(left_nv, ">= operand");
	NovaValue right_nv = eval_rec(rhs, env_index);
	ASSERT_IS_NUMBER(right_nv, ">= operand");

	double left_val = eval_retrieve_number(left_nv);
	double right_val = eval_retrieve_number(right_nv);

	NovaValue result = NovaValue{NovaValueType::E_CHARLIT};
	result.index = nova_chars_push(left_val >= right_val);

	return result;
}

NovaValue eval_less_than_or_equal(AST_Node node, int env_index) {
	AST_Node lhs = ast_nodes[node.child_start];
	AST_Node rhs = ast_nodes[node.child_start+1];

	NovaValue left_nv = eval_rec(lhs, env_index);
	ASSERT_IS_NUMBER(left_nv, "<= operand");
	NovaValue right_nv = eval_rec(rhs, env_index);
	ASSERT_IS_NUMBER(right_nv, "<= operand");

	double left_val = eval_retrieve_number(left_nv);
	double right_val = eval_retrieve_number(right_nv);

	NovaValue result = NovaValue{NovaValueType::E_CHARLIT};
	result.index = nova_chars_push(left_val <= right_val);

	return result;
}

NovaValue eval_equals(AST_Node node, int env_index) {
	AST_Node lhs = ast_nodes[node.child_start];
	AST_Node rhs = ast_nodes[node.child_start+1];

	NovaValue left_nv = eval_rec(lhs, env_index);
	ASSERT_IS_NUMBER(left_nv, "== operand");
	NovaValue right_nv = eval_rec(rhs, env_index);
	ASSERT_IS_NUMBER(right_nv, "== operand");

	double left_val = eval_retrieve_number(left_nv);
	double right_val = eval_retrieve_number(right_nv);

	NovaValue result = NovaValue{NovaValueType::E_CHARLIT};
	result.index = nova_chars_push(left_val == right_val);

	return result;
}

NovaValue eval_not_equals(AST_Node node, int env_index) {
	AST_Node lhs = ast_nodes[node.child_start];
	AST_Node rhs = ast_nodes[node.child_start+1];

	NovaValue left_nv = eval_rec(lhs, env_index);
	ASSERT_IS_NUMBER(left_nv, "!= operand");
	NovaValue right_nv = eval_rec(rhs, env_index);
	ASSERT_IS_NUMBER(right_nv, "!= operand");

	double left_val = eval_retrieve_number(left_nv);
	double right_val = eval_retrieve_number(right_nv);

	NovaValue result = NovaValue{NovaValueType::E_CHARLIT};
	result.index = nova_chars_push(left_val != right_val);

	return result;
}

NovaValue eval_not(AST_Node node, int env_index) {
	AST_Node exp = ast_nodes[node.child_start];

	NovaValue nv = eval_rec(exp, env_index);
	ASSERT_TYPE("! argument", nv.type, E_CHARLIT, "char");

	char val = nova_chars[nv.index];

	NovaValue result = NovaValue{NovaValueType::E_CHARLIT};
	// Invert the result
	if(val == 0) {
		result.index = nova_chars_push(1);
	} else {
		result.index = nova_chars_push(0);
	}

	return result;
}

NovaValue eval_and(AST_Node node, int env_index) {
	AST_Node left = ast_nodes[node.child_start];
	AST_Node right = ast_nodes[node.child_start+1];

	NovaValue left_nv = eval_rec(left, env_index);
	ASSERT_TYPE("and argument", left_nv.type, E_CHARLIT, "char");
	NovaValue right_nv = eval_rec(right, env_index);
	ASSERT_TYPE("and argument", right_nv.type, E_CHARLIT, "char");

	char left_result = nova_chars[left_nv.index];
	char right_result = nova_chars[right_nv.index];

	NovaValue result = NovaValue{NovaValueType::E_CHARLIT};
	if(left_result == 0 || right_result == 0) {
		result.index = nova_chars_push(0);
	} else {
		result.index = nova_chars_push(1);
	}

	return result;
}

NovaValue eval_or(AST_Node node, int env_index) {
	AST_Node left = ast_nodes[node.child_start];
	AST_Node right = ast_nodes[node.child_start+1];

	NovaValue left_nv = eval_rec(left, env_index);
	ASSERT_TYPE("or argument", left_nv.type, E_CHARLIT, "char");
	NovaValue right_nv = eval_rec(right, env_index);
	ASSERT_TYPE("or argument", right_nv.type, E_CHARLIT, "char");

	char left_result = nova_chars[left_nv.index];
	char right_result = nova_chars[right_nv.index];

	NovaValue result = NovaValue{NovaValueType::E_CHARLIT};
	if(left_result == 0 && right_result == 0) {
		result.index = nova_chars_push(0);
	} else {
		result.index = nova_chars_push(1);
	}

	return result;
}

NovaValue eval_post_increment(AST_Node node, int env_index) {
	// TODO: this may not be an identifier (5++)
	char* name = tokens_ident[node.ttable_index];
	
	// Search in the current env
	NovaValue nv = envs_search(name, env_index);
	ASSERT_UNBOUND(nv, name);
	
	NovaValue result = NovaValue{nv.type};
	ASSERT_IS_NUMBER(result, "increment value");
	double val = eval_retrieve_number(nv);

	// Update the value
	result.index = eval_push_number_from_type(nv.type, val+1);
	
	// Set the value
	env_set_by_name(env_index, name, result);

	return result;
}

NovaValue eval_post_decrement(AST_Node node, int env_index) {
	// TODO: this may not be an identifier (5--)
	char* name = tokens_ident[node.ttable_index];
	
	// Search in the current env
	NovaValue nv = envs_search(name, env_index);
	ASSERT_UNBOUND(nv, name);

	NovaValue result = NovaValue{nv.type};
	ASSERT_IS_NUMBER(result, "decrement value");
	double val = eval_retrieve_number(nv);

	// Update the value
	result.index = eval_push_number_from_type(nv.type, val-1);
	
	// Set the value
	env_set_by_name(env_index, name, result);

	return result;
}

NovaValue eval_print(AST_Node node, int env_index) {
	AST_Node child = ast_nodes[node.child_start];
	
	NovaValue nv = eval_rec(child, env_index);

	switch(nv.type) {
	case NovaValueType::E_INTLIT: {
		printf("%d\n", nova_integers[nv.index]);
	} break;
	case NovaValueType::E_FLOATLIT: {
		printf("%f\n", nova_floats[nv.index]);
	} break;
	case NovaValueType::E_CHARLIT: {
		printf("%c\n", nova_chars[nv.index]);
	} break;
	case NovaValueType::E_STRINGLIT: {
		printf("%s\n", nova_strings[nv.index].c_str());
	} break;
		// TODO: print an array
	};
	
	return NovaValue{NovaValueType::E_UNKNOWN};
}

NovaValueType eval_ottonvt(OperationType type) {
	switch(type) {
	case OperationType::N_INT: {
		return NovaValueType::E_INT;
	} break;
	case OperationType::N_CHAR: {
		return NovaValueType::E_CHAR;
	} break;
	case OperationType::N_FLOAT: {
		return NovaValueType::E_FLOAT;
	} break;
	case OperationType::N_STRING: {
		return NovaValueType::E_STRING;
	} break;
		// TODO: should there be an array?
	};

	return NovaValueType::E_UNKNOWN;
}

double eval_retrieve_number(NovaValue nv) {
	double val;
	switch(nv.type) {
	case NovaValueType::E_INTLIT: {
		val = nova_integers[nv.index];
	} break;
	case NovaValueType::E_FLOATLIT: {
		val = nova_floats[nv.index];
	} break;
	case NovaValueType::E_CHARLIT: {
		val = nova_chars[nv.index];
	} break;
	};

	return val;
}

int eval_push_number_from_type(NovaValueType type, double value) {
	switch(type) {
	case NovaValueType::E_INTLIT: {
		return nova_integers_push(value);
	} break;
	case NovaValueType::E_FLOATLIT: {
		return nova_floats_push(value);
	} break;
	case NovaValueType::E_CHARLIT: {
		return nova_chars_push(value);
	} break;
	};

	return -1;
}

NovaValueType eval_dominant_type(NovaValueType left, NovaValueType right) {
	if(left > right)
		return left;

	return right;
}
