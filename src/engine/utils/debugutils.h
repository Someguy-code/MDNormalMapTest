#ifndef _DEBUG_UTILS_H_
#define _DEBUG_UTILS_H_

#include <genesis.h>

#include <engine/utils/debugutilsprivate.h>
#include <engine/utils/macroutils.h>

#define DEBUG_LOG_V2_INT(Variable) _Generic((Variable.x), \
    s16: KLog_S2, \
    s32: KLog_S2, \
    u16: KLog_U2, \
    u32: KLog_U2 \
)(#Variable " = ", Variable.x, ", ", Variable.y)

#define DEBUG_LOG_V3_INT(Variable) _Generic((Variable.x), \
    s8: KLog_S3, \
    s16: KLog_S3, \
    s32: KLog_S3, \
    u8: KLog_U3 \
)(#Variable " = ", Variable.x, ", ", Variable.y, ", ", Variable.z)

#define DEBUG_LOG_V3_FIXED(Variable) _Generic((Variable.x), \
    f16: KLog_F3, \
    f32: KLog_F3 \
)(#Variable " = ", Variable.x, ", ", Variable.y, ", ", Variable.z)

#define DEBUG_LOG_INT(Variable) _Generic((Variable), \
    s8: KLog_S1, \
    s16: KLog_S1, \
    s32: KLog_S1, \
    unsigned int: KLog_S1, \
    u8: KLog_U1, \
    u16: KLog_U1, \
    u32: KLog_U1, \
    int: KLog_U1 \
)(#Variable " = ", Variable)


#define DEBUG_LOG_FIXED(Variable) _Generic((Variable), \
    f16: KLog_F1, \
    f32: KLog_F1 \
)(#Variable " = ", Variable)

#define DEBUG_LOG_POINTER(Variable) KLog_U1(#Variable " = ", (u32)Variable)

#define DEBUG_LOG_FUNCTION() (void)kprintf(__func__)

#define DEBUG_LOG_BOOL(Variable) (void)kprintf("%s = %s", #Variable , (Variable) ? "TRUE" : "FALSE")

//Prints the name of the function at the start and at the end (Prefixed by "/"). Nested calls will be tabulated.
#define DEBUG_LOG_FUNCTION_BLOCK() \
    (void)kprintf("%*s%s", DEBUG_UTILS_TAB_WIDTH * m_oDebugUtilsData.m_uCurrentFunctionBlockIndentation, "", __func__); \
    ++m_oDebugUtilsData.m_uCurrentFunctionBlockIndentation; \
    __attribute__((cleanup(DebugLogFunctionEnd))) DebugFunctionInfo GET_MACRO_VARIABLE_NAME(DebugFunctionName) = {__func__};

//NOTE: Consider using MEM_checkIntegrity instead when the new SGDK version is released
[[maybe_unused]]
inline bool CheckStackPointer()
{
    bool bIsStackValid = true;
    const u32 uStackPointer = GetStackPointer();
    if(uStackPointer < MEMORY_HIGH)
    {
        KLog_U1("STACK OVERFLOW ", uStackPointer);
        bIsStackValid = false;
    }
    else if(uStackPointer >= (MEMORY_HIGH + STACK_SIZE))
    {
        KLog_U1("STACK UNDERFLOW ", uStackPointer);
        bIsStackValid = false;
    }
    return bIsStackValid;
}

#endif //#ifndef _DEBUG_UTILS_H_