#ifndef NOVA_VALUE_H
#define NOVA_VALUE_H

#include "ast.h"

#include <string>

enum SmolValueType {
    E_UNKNOWN, E_FUN, E_IDENT, E_CHAR, E_INT, E_FLOAT, E_STRING, E_ARRAY,
	// TODO: those are not needed: they are stored in token tables
    E_CHARLIT, E_INTLIT, E_FLOATLIT, E_STRINGLIT
};

// TODO: instead of this, just use combined flags
enum SmolPayloadFlag {
	P_NONE, P_RETURN, P_BREAK, P_CONTINUE
};

struct SmolValue {
    SmolValueType type = SmolValueType::E_UNKNOWN;
    int index = -1;
	// TODO: instead of this, just use combined flags
	SmolPayloadFlag payload = SmolPayloadFlag::P_NONE;
};

struct SmolFunData {
    SmolValueType return_value = SmolValueType::E_UNKNOWN;
    AST_Node block;
    int env_index = -1;

    // FIXME: SLOW!!
    std::vector<SmolValueType> types;
};

extern std::vector<SmolFunData> smol_funs;
extern std::vector<int> smol_integers;
extern std::vector<float> smol_floats;
extern std::vector<char> smol_chars;
extern std::vector<std::string> smol_strings;
// FIXME: SLOW!! Use an ArrayData (like for funs) instead
extern std::vector<std::vector<SmolValue>> smol_arrays;

int smol_funs_push(SmolFunData data);
int smol_integers_push(int value);
int smol_floats_push(float value);
int smol_chars_push(char value);
int smol_strings_push(std::string value);
int smol_arrays_push(std::vector<SmolValue> value);

#endif // NOVA_VALUE_H
