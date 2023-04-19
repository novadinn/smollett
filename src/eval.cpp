#include "eval.h"

extern std::vector<int> tokens_intlit;
extern std::vector<char> tokens_charlit;
extern std::vector<char*> tokens_strlit;
extern std::vector<char*> tokens_ident;

extern std::vector<AST_Node> ast_nodes;
extern std::vector<Enviroinment> envs;

void eval(AST_Node program, int env_index) {
	eval_rec(program, env_index);
}

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
	case OperationType::N_VAR: {
		return eval_var(node, env_index);
	} break;
	case OperationType::N_IDENT: {
		return eval_ident(node, env_index);
	} break;
	case OperationType::N_INTLIT: {
		return eval_intlit(node, env_index);
	};
	case OperationType::N_CHARLIT: {
		return eval_charlit(node, env_index);
	};
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

NovaValue eval_block(AST_Node node, int env_index) {
	// Store the last evaluated element, and return it after
	NovaValue fin;
	// Evaluate every element
	for(int i = node.child_start; i < node.child_start + node.child_num; ++i) {
		fin = eval_rec(ast_nodes[i], env_index);
	}

	return fin;
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
	AST_Node return_value = ast_nodes[node.child_start-2];
	AST_Node block = ast_nodes[node.child_start-1];

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

	// TODO: check if function with that name does exists in envs
	
	// Search the function in the env
	NovaValue function = envs_search(fn_name, env_index);
	NovaFunData data = nova_funs[function.index];
	NovaValueType return_value_type = data.return_value;
	AST_Node block = data.block;
	int function_env_index = data.env_index;

	// Function argument count
	int argc = node.child_num-1;
	if(argc != data.types.size()) {
		// TODO: log error: wrong argument count
	}
	
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

	// TODO: clear the function env after

	return eval_result;
}

NovaValue eval_return(AST_Node node, int env_index) {
	AST_Node return_value = ast_nodes[node.child_start];
	NovaValue eval_value = eval_rec(return_value, env_index);

	return eval_value;
}

NovaValue eval_var(AST_Node node, int env_index) {
	// Store the last evaluated value
	NovaValue fin;
	// Loop over multiple definitions (var a = 0, b = 0;)
	for(int i = node.child_start; i < node.child_start + node.child_num; ++i) {
		AST_Node assign = ast_nodes[i];
		AST_Node lhs = ast_nodes[assign.child_start];	
		AST_Node rhs = ast_nodes[assign.child_start+1];

		char* var_name = tokens_ident[lhs.ttable_index];
		NovaValue result = eval_rec(rhs, env_index);
		if(lhs.child_num == 1) {
			AST_Node type_node = ast_nodes[lhs.child_start];
			// TODO: check if type is correct. If not, log error
		}

		env_push_value(env_index, var_name, result);

		fin = result;
	}

	printf("Created var with value: %d\n", nova_integers[fin.index]);
	
	return fin;
}

NovaValue eval_ident(AST_Node node, int env_index) {
	char* sym = tokens_ident[node.ttable_index];

	NovaValue result = envs_search(sym, env_index);
	if(result.type == NovaValueType::E_UNKNOWN) {
		// TODO: log error: unbound symbol
		return NovaValue{NovaValueType::E_UNKNOWN};
	}
	
	return result;
}

NovaValue eval_intlit(AST_Node node, int env_index) {
	NovaValue result = NovaValue{NovaValueType::E_INTLIT};
	// Get the value from the tokens table
	int value = tokens_intlit[node.ttable_index];
	result.index = nova_integers_push(value);

	return result;
}

NovaValue eval_charlit(AST_Node node, int env_index) {
	NovaValue result = NovaValue{NovaValueType::E_CHARLIT};
	// Get the value from the tokens table
	int value = tokens_charlit[node.ttable_index];
	result.index = nova_chars_push(value);

	return result;
}

NovaValue eval_plus(AST_Node node, int env_index) {
	AST_Node lhs = ast_nodes[node.child_start];
	AST_Node rhs = ast_nodes[node.child_start+1];

	NovaValue left_nv = eval_rec(lhs, env_index);
	NovaValue right_nv = eval_rec(rhs, env_index);

	double left_val = eval_retrieve_number(left_nv);
	double right_val = eval_retrieve_number(right_nv);

	NovaValueType type = eval_dominant_type(left_nv.type, right_nv.type);

	NovaValue result = NovaValue{type};
	// TODO: remove previous 2 from the nova_integers array
	result.index = eval_push_number_from_type(type, left_val + right_val);
	
	return result;
}

NovaValue eval_minus(AST_Node node, int env_index) {
	AST_Node lhs = ast_nodes[node.child_start];
	AST_Node rhs = ast_nodes[node.child_start+1];

	NovaValue left_nv = eval_rec(lhs, env_index);
	NovaValue right_nv = eval_rec(rhs, env_index);

	double left_val = eval_retrieve_number(left_nv);
	double right_val = eval_retrieve_number(right_nv);

	NovaValueType type = eval_dominant_type(left_nv.type, right_nv.type);

	NovaValue result = NovaValue{type};
	// TODO: remove previous 2 from the nova_integers array
	result.index = eval_push_number_from_type(type, left_val - right_val);
	
	return result;
}

NovaValue eval_mult(AST_Node node, int env_index) {
	AST_Node lhs = ast_nodes[node.child_start];
	AST_Node rhs = ast_nodes[node.child_start+1];

	NovaValue left_nv = eval_rec(lhs, env_index);
	NovaValue right_nv = eval_rec(rhs, env_index);

	double left_val = eval_retrieve_number(left_nv);
	double right_val = eval_retrieve_number(right_nv);

	NovaValueType type = eval_dominant_type(left_nv.type, right_nv.type);

	NovaValue result = NovaValue{type};
	// TODO: remove previous 2 from the nova_integers array
	result.index = eval_push_number_from_type(type, left_val * right_val);
	
	return result;
}

NovaValue eval_div(AST_Node node, int env_index) {
	AST_Node lhs = ast_nodes[node.child_start];
	AST_Node rhs = ast_nodes[node.child_start+1];

	NovaValue left_nv = eval_rec(lhs, env_index);
	NovaValue right_nv = eval_rec(rhs, env_index);

	double left_val = eval_retrieve_number(left_nv);
	double right_val = eval_retrieve_number(right_nv);

	NovaValueType type = eval_dominant_type(left_nv.type, right_nv.type);

	NovaValue result = NovaValue{type};
	// TODO: remove previous 2 from the nova_integers array
	result.index = eval_push_number_from_type(type, left_val / right_val);
	
	return result;
}

NovaValue eval_if(AST_Node node, int env_index) {
	NovaValue cond = eval_rec(ast_nodes[node.child_start], env_index);
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

NovaValue eval_greater_than(AST_Node node, int env_index) {
	AST_Node lhs = ast_nodes[node.child_start];
	AST_Node rhs = ast_nodes[node.child_start+1];

	NovaValue left_nv = eval_rec(lhs, env_index);
	NovaValue right_nv = eval_rec(rhs, env_index);

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
	NovaValue right_nv = eval_rec(rhs, env_index);

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
	NovaValue right_nv = eval_rec(rhs, env_index);

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
	NovaValue right_nv = eval_rec(rhs, env_index);

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
	NovaValue right_nv = eval_rec(rhs, env_index);

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
	NovaValue right_nv = eval_rec(rhs, env_index);

	double left_val = eval_retrieve_number(left_nv);
	double right_val = eval_retrieve_number(right_nv);

	NovaValue result = NovaValue{NovaValueType::E_CHARLIT};
	result.index = nova_chars_push(left_val != right_val);

	return result;
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
	};

	return NovaValueType::E_UNKNOWN;
}

double eval_retrieve_number(NovaValue nv) {
	double val;
	switch(nv.type) {
	case NovaValueType::E_INTLIT: {
		val = nova_integers[nv.index];
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
