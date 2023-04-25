#ifndef ENV_H
#define ENV_H

#include "smol_value.h"

#include <vector>

struct EnviroinmentValue {
	SmolValue value;
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
SmolValue envs_search(char* sym, int index);
void env_push_value(int env_index, char* sym, SmolValue value);
void env_push_unbound(int env_index, char* sym, SmolValueType type);
void env_set_unbound(int env_index, int value_index, SmolValue value);
// TODO: do we really need that function or not?
// we can just directly set the table value from the nv.index
void env_set_by_name(int env_index, char* sym, SmolValue value);

#endif // ENV_H
