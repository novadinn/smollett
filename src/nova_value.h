#ifndef NOVA_VALUE_H
#define NOVA_VALUE_H

#include "ast.h"

#include <string>

enum NovaValueType {
    E_UNKNOWN, E_FUN, E_IDENT, E_CHAR, E_INT, E_FLOAT, E_STRING, E_ARRAY,
	// TODO: those are not needed: they are stored in token tables
    E_CHARLIT, E_INTLIT, E_FLOATLIT, E_STRINGLIT
};

// TODO: instead of this, just use combined flags
enum NovaPayloadFlag {
	P_NONE, P_RETURN, P_BREAK, P_CONTINUE
};

struct NovaValue {
    NovaValueType type = NovaValueType::E_UNKNOWN;
    
    int index = -1;
    int env_index = -1;
	// TODO: instead of this, just use combined flags
	NovaPayloadFlag payload = NovaPayloadFlag::P_NONE;
};

struct NovaFunData {
    NovaValueType return_value = NovaValueType::E_UNKNOWN;
    AST_Node block;
    int env_index = -1;

    // FIXME: SLOW!!
    std::vector<NovaValueType> types;
};

extern std::vector<NovaFunData> nova_funs;
extern std::vector<int> nova_integers;
extern std::vector<float> nova_floats;
extern std::vector<char> nova_chars;
extern std::vector<std::string> nova_strings;
// FIXME: SLOW!! Use an ArrayData (like for funs) instead
extern std::vector<std::vector<NovaValue>> nova_arrays;

int nova_funs_push(NovaFunData data);
int nova_integers_push(int value);
int nova_floats_push(float value);
int nova_chars_push(char value);
int nova_strings_push(std::string value);
int nova_arrays_push(std::vector<NovaValue> value);

#endif // NOVA_VALUE_H
