#ifndef _PALETTE_POST_PROCESS_H_
#define _PALETTE_POST_PROCESS_H_

#include <genesis.h>

#include <engine/constants/paletteconstants.h>
#include <engine/types/int16interpolator.h>

struct PaletteManager;
struct PaletteManagerPostProcessRange;
typedef void PalettePostProcessCallback(struct PaletteManager* _pPaletteManager, struct PaletteManagerPostProcessRange* _pRange);

typedef struct PaletteManagerPostProcessDescriptor
{
    PalettePostProcessCallback* OnPalettePostProcess;
    u16 m_uMaxValue;
} PaletteManagerPostProcessDescriptor;

//Range of palette entries affected by a post-process
typedef struct PaletteManagerPostProcessRange
{
    PalettePostProcessCallback* OnPalettePostProcess;
    const void* m_pSetting;
    u16Interpolator m_oPostProcessValueInterpolator;
    u8 m_uFirstColorIndex;
    u8 m_uColorsCount;
} PaletteManagerPostProcessRange;

enum PaletteManagerPostProcessConstants
{
    MAX_PALETTE_POST_PROCESSING_RANGES = ALL_COLORS_COUNT
};

typedef struct PaletteManagerPostProcess
{
    //Actual ranges data
    PaletteManagerPostProcessRange m_oRanges[MAX_PALETTE_POST_PROCESSING_RANGES];
    //An partitioned array containing used ranges in the front partition and inactive ones in the back
    PaletteManagerPostProcessRange* m_oRangesUsagePartition[MAX_PALETTE_POST_PROCESSING_RANGES];
    u8 m_uActiveRangesCount;
    u16 m_oPaletteColorsPostProcess[ALL_COLORS_COUNT];
} PaletteManagerPostProcess;

void InitializePaletteManagerPostProcess(PaletteManagerPostProcess* _pPaletteManagerPostProcess);
u8 AddRangeToPaletteManagerPostProcess(PaletteManagerPostProcess* _pPaletteManagerPostProcess, const void* _pSettings, u8 _uBaseColorIndex, u8 _uColorsCount, const PaletteManagerPostProcessDescriptor* _pPaletteManagerPostProcessDescriptor, u16 _uPostProcessTime);
void RemoveRangeFromPaletteManagerPostProcess(PaletteManagerPostProcess* _pPaletteManagerPostProcess, u8 _uIndex);
bool UpdatePaletteManagerPostProcess(PaletteManagerPostProcess* _pPaletteManagerPostProcess, struct PaletteManager* _pPaletteManager);

#endif //#ifndef _PALETTE_POST_PROCESS_H_
