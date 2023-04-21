#ifndef NOVA_VALUE_H
#define NOVA_VALUE_H

#include "ast.h"

enum NovaValueType {
    E_UNKNOWN, E_FUN, E_IDENT, E_CHAR, E_INT, E_FLOAT,
    E_CHARLIT, E_INTLIT, E_FLOATLIT
};

struct NovaValue {
    NovaValueType type = NovaValueType::E_UNKNOWN;
    
    int index = -1;
    int env_index = -1;
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

void nova_funs_init(int length);
int nova_funs_push(NovaFunData data);
void nova_funs_clear();

void nova_integers_init(int length);
int nova_integers_push(int value);
void nova_integers_clear();

void nova_floats_init(int length);
int nova_floats_push(float value);
void nova_floats_clear();

void nova_chars_init(int length);
int nova_chars_push(char value);
void nova_chars_clear();

#endif // NOVA_VALUE_H
