#include "smol_value.h"

std::vector<SmolFunData> smol_funs;
std::vector<int> smol_integers;
std::vector<float> smol_floats;
std::vector<char> smol_chars;
std::vector<std::string> smol_strings;
std::vector<std::vector<SmolValue>> smol_arrays;

int smol_funs_push(SmolFunData data) {
	smol_funs.push_back(data);

	return smol_funs.size()-1;
}

int smol_integers_push(int value) {
	smol_integers.push_back(value);

	return smol_integers.size()-1;
}

int smol_floats_push(float value) {
	smol_floats.push_back(value);

	return smol_floats.size()-1;
}

int smol_chars_push(char value) {
	smol_chars.push_back(value);

	return smol_chars.size()-1;
}

int smol_strings_push(std::string value) {
	smol_strings.push_back(value);

	return smol_strings.size()-1;
}

int smol_arrays_push(std::vector<SmolValue> value) {
	smol_arrays.push_back(value);

	return smol_arrays.size()-1;
}
