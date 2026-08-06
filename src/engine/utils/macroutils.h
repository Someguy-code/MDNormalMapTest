#ifndef _MACRO_UTILS_H_
#define _MACRO_UTILS_H_

//Concatenates all the provided tokens
#define CONCAT(a, ...) CONCAT_INNER(a, __VA_ARGS__)
#define CONCAT_INNER(a, ...) a ## __VA_ARGS__

//Creates a macro local object name to mitigate collisions with the calling context
#define GET_MACRO_VARIABLE_NAME(VariableName) CONCAT(VariableName, __LINE__)

//Dummy code for macros defined only under certain compilation options
#define DUMMY_MACRO do {} while (0)

#endif //#define _MACRO_UTILS_H_