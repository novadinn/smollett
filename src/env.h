#ifndef ENV_H
#define ENV_H

#include "nova_value.h"

#include <vector>

struct EnviroinmentValue {
	NovaValue value;
	char* sym;
};

struct Enviroinment {
	int index = -1;
	int parent_index = -1;

	// FIXME: SLOW!!
	std::vector<EnviroinmentValue> values;
};

extern std::vector<Enviroinment> envs;

int envs_push(int parent_index);
NovaValue envs_search(char* sym, int index);
void env_push_value(int env_index, char* sym, NovaValue value);
void env_push_unbound(int env_index, char* sym, NovaValueType type);
void env_set_unbound(int env_index, int value_index, NovaValue value);
// TODO: do we really need that function or not?
// we can just directly set the table value from the nv.index
void env_set_by_name(int env_index, char* sym, NovaValue value);

#endif // ENV_H
