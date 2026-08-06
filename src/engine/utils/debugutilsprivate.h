#ifndef _DEBUG_UTILS_PRIVATE_H_
#define _DEBUG_UTILS_PRIVATE_H_

//Auxiliar private code for debugutils.h. DO NOT USE

#include <genesis.h>

typedef struct
{
    const char* m_sName;
} DebugFunctionInfo;

typedef struct 
{
    u16 m_uCurrentFunctionBlockIndentation;
} DebugUtilsData;

enum DebugUtilsConstants
{
    DEBUG_UTILS_TAB_WIDTH = 4
};
extern DebugUtilsData m_oDebugUtilsData;

[[maybe_unused]]
static void DebugLogFunctionEnd(DebugFunctionInfo* _sDebugFunctionInfo)
{
    --m_oDebugUtilsData.m_uCurrentFunctionBlockIndentation;
    (void)kprintf("%*s/%s", DEBUG_UTILS_TAB_WIDTH * m_oDebugUtilsData.m_uCurrentFunctionBlockIndentation, "", _sDebugFunctionInfo->m_sName);
}

[[maybe_unused]]
inline u32 GetStackPointer()
{
    return ({ register u32 arg0 asm("%sp"); arg0; });
}

#endif //#ifndef _DEBUG_UTILS_PRIVATE_H_