#include "env.h"

#include <cstdio>

std::vector<Enviroinment> envs;

int envs_push(int parent_index) {
	Enviroinment env;
	env.index = envs.size();
	env.parent_index = parent_index;
	
	envs.push_back(env);

	return env.index;
}

NovaValue envs_search(char* sym, int index) {
	Enviroinment cur = envs[index];
	for(int i = 0; i < cur.values.size(); ++i) {
		EnviroinmentValue val = cur.values[i];
		if(strcmp(sym, val.sym) == 0) {
			return val.value;
		}
	}

	if(cur.parent_index != -1) {
		// Recursively search in the parent env
		return envs_search(sym, cur.parent_index);
	}

	return NovaValue{NovaValueType::E_UNKNOWN};
}

void env_push_value(int env_index, char* sym, NovaValue value) {
	Enviroinment* env = &envs[env_index];
	env->values.push_back(EnviroinmentValue{value, sym});
}

void env_push_unbound(int env_index, char* sym, NovaValueType type) {
	Enviroinment* env = &envs[env_index];
	env->values.push_back(EnviroinmentValue{NovaValue{type, -1}, sym});
}

void env_set_unbound(int env_index, int value_index, NovaValue value) {
	Enviroinment* env = &envs[env_index];
	if(value_index < 0 || value_index > env->values.size()-1) {
		// TODO: return false
		return;
	}
	
	EnviroinmentValue* env_val = &env->values[value_index];
	env_val->value = value;

	// TODO: return true
}

void env_set_by_name(int env_index, char* sym, NovaValue value) {
	Enviroinment* cur = &envs[env_index];
	for(int i = 0; i < cur->values.size(); ++i) {
		EnviroinmentValue* val = &cur->values[i];
		if(strcmp(sym, val->sym) == 0) {
			val->value = value;
			return;
		}
	}

	if(cur->parent_index != -1) {
		// Recursively search in the parent env
		env_set_by_name(cur->parent_index, sym, value);
	}
}
