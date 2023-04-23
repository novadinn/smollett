#include "nova_value.h"

std::vector<NovaFunData> nova_funs;
std::vector<int> nova_integers;
std::vector<float> nova_floats;
std::vector<char> nova_chars;
std::vector<std::string> nova_strings;
std::vector<std::vector<NovaValue>> nova_arrays;

int nova_funs_push(NovaFunData data) {
	nova_funs.push_back(data);

	return nova_funs.size()-1;
}

int nova_integers_push(int value) {
	nova_integers.push_back(value);

	return nova_integers.size()-1;
}

int nova_floats_push(float value) {
	nova_floats.push_back(value);

	return nova_floats.size()-1;
}

int nova_chars_push(char value) {
	nova_chars.push_back(value);

	return nova_chars.size()-1;
}

int nova_strings_push(std::string value) {
	nova_strings.push_back(value);

	return nova_strings.size()-1;
}

int nova_arrays_push(std::vector<NovaValue> value) {
	nova_arrays.push_back(value);

	return nova_arrays.size()-1;
}
