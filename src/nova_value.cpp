#include "nova_value.h"

std::vector<NovaFunData> nova_funs;
std::vector<int> nova_integers;
std::vector<char> nova_chars;

void nova_funs_init(int length) {
	nova_funs.reserve(length);
}

int nova_funs_push(NovaFunData data) {
	nova_funs.push_back(data);

	return nova_funs.size()-1;
}

void nova_funs_clear() {
	nova_funs.clear();
}

void nova_integers_init(int length) {
	nova_integers.reserve(length);
}

int nova_integers_push(int value) {
	nova_integers.push_back(value);

	return nova_integers.size()-1;
}

void nova_integers_clear() {
	nova_integers.clear();
}

void nova_chars_init(int length) {
	nova_chars.reserve(length);
}

int nova_chars_push(char value) {
	nova_chars.push_back(value);

	return nova_chars.size()-1;
}

void nova_chars_clear() {
	nova_chars.clear();
}
