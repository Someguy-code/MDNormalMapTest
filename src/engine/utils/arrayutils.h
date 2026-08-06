#ifndef _ARRAY_UTILS_H_
#define _ARRAY_UTILS_H_

#include <engine/utils/macroutils.h>

#define ARRAY_GET_LENGTH_STATIC(_oArray) (sizeof(_oArray) / sizeof((_oArray)[0]))

//Removes element at _uIndex in array _oArray of length _uArrayLength. The order of _oArray changes
#define ARRAY_REMOVE_UNORDERED(_oArray, _uArrayLength, _uIndex) \
    (_oArray)[_uIndex] = (_oArray)[(_uArrayLength)--];

#define ARRAY_PUSH_BACK(_oArray, _uArrayLength, _oElement) (_oArray)[(_uArrayLength)++] = _oElement

//Move element at index _uIndex un the front partition to the end of the back partition of array _oArray. The order of _oArray is changed
//Returns the absolute index of the the relocated _uIndex
#define ARRAY_PARTITION_FRONT_TO_BACK_UNORDERED(_oArray, _uArrayLength, _uFrontPartitionLength, _uIndex) ({\
    const __auto_type GET_MACRO_VARIABLE_NAME(oLastFrontElement) = (_oArray)[_uFrontPartitionLength - 1]; \
    const u16 GET_MACRO_VARIABLE_NAME(uNewAbsoluteIndex) = --(_uFrontPartitionLength); \
    (_oArray)[GET_MACRO_VARIABLE_NAME(uNewAbsoluteIndex)] = (_oArray)[_uIndex]; \
    (_oArray)[_uIndex] = GET_MACRO_VARIABLE_NAME(oLastFrontElement); \
    GET_MACRO_VARIABLE_NAME(uNewAbsoluteIndex); \
})

//Move element at index _uIndex un the back partition to the end of the front partition of array _oArray. The order of _oArray is changed.
//Returns the absolute index of the the relocated _uIndex
#define ARRAY_PARTITION_BACK_TO_FRONT_UNORDERED(_oArray, _uArrayLength, _uFrontPartitionLength, _uIndex) ({\
    const u16 GET_MACRO_VARIABLE_NAME(uAbsoluteIndex) = (_uArrayLength) - 1 - (_uIndex); \
    const __auto_type GET_MACRO_VARIABLE_NAME(oLastBackElement) = (_oArray)[_uFrontPartitionLength]; \
    const u16 GET_MACRO_VARIABLE_NAME(uNewAbsoluteIndex) = (_uFrontPartitionLength)++; \
    (_oArray)[GET_MACRO_VARIABLE_NAME(uNewAbsoluteIndex)] = (_oArray)[GET_MACRO_VARIABLE_NAME(uAbsoluteIndex)]; \
    (_oArray)[GET_MACRO_VARIABLE_NAME(uAbsoluteIndex)] = GET_MACRO_VARIABLE_NAME(oLastBackElement); \
    GET_MACRO_VARIABLE_NAME(uNewAbsoluteIndex); \
})

//Move element the last element in front partition to the end of the back partition
//Returns the absolute index of the the relocated element
#define ARRAY_PARTITION_FRONT_TO_BACK_LAST(_uFrontPartitionLength) ({ \
    --(_uFrontPartitionLength); \
})

//Move element the last element in back partition to the end of the front partition
//Returns the absolute index of the the relocated element
#define ARRAY_PARTITION_BACK_TO_FRONT_LAST(_uFrontPartitionLength) ({ \
    (_uFrontPartitionLength)++; \
})

#endif //#ifndef _ARRAY_UTILS_H_