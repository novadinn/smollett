#ifndef LOG_H
#define LOG_H

#include "smol_value.h"

#define ASSERT(cond, fmt, ...)								\
    if (!(cond)) { printf(fmt, ##__VA_ARGS__); exit(1); }
#define ASSERT_TYPE(name, got, expected, expected_str)					\
	ASSERT(got == expected,												\
		   "smollett: %s is not a %s\n", name, expected_str)
#define ASSERT_FUN_TYPE(func, index, got, expected)						\
    ASSERT(got == expected,												\
		   "smollett: function '%s' passed incorrect type for argument %i: got %s, expected %s\n", \
		   func, index, ltype_to_str(got).c_str(), ltype_to_str(expected).c_str())
#define ASSERT_FUN_ARGC(func, got, expected)							\
    ASSERT(got == expected,												\
		   "smollett: function '%s' passed incorrect number of arguments: got %i, expected %i\n", \
		   func, got, expected)
#define ASSERT_UNBOUND(nv, symbol)				\
	ASSERT(nv.type != E_UNKNOWN,	\
		   "smollett: unbound symbol %s\n", symbol)
#define ASSERT_IS_NUMBER(nv, sym)				\
	ASSERT(nv.type == E_INTLIT || nv.type == E_FLOATLIT || nv.type == E_CHARLIT,	\
		   "smollett: %s is not a number\n", sym)
#define ASSERT_ARRAY_SIZE(name, got, max)		\
	ASSERT(got <= max,							\
		   "smollett: array %s too low for specifield initialization\n", name)

#endif // LOG_H
