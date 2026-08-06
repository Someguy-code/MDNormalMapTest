#include "palettepostprocess.h"

#include <engine/palettemanager/palettemanager.h>
#include <engine/palettemanager/palettemanagerutils.h>
#include <engine/utils/arrayutils.h>

static bool UpdateRange(PaletteManagerPostProcessRange* _pRange, PaletteManager* _pPaletteManager);

void InitializePaletteManagerPostProcess(PaletteManagerPostProcess* _pPaletteManagerPostProcess)
{
    //Register all ranges as inactive
    PaletteManagerPostProcessRange* pRanges = _pPaletteManagerPostProcess->m_oRanges;
    PaletteManagerPostProcessRange** pRangesUsagePartition = _pPaletteManagerPostProcess->m_oRangesUsagePartition;
    for(u16 uRangeIndex = 0; uRangeIndex < MAX_PALETTE_POST_PROCESSING_RANGES; ++uRangeIndex)
        pRangesUsagePartition[uRangeIndex] = &pRanges[uRangeIndex];
    _pPaletteManagerPostProcess->m_uActiveRangesCount = 0;
}

u8 AddRangeToPaletteManagerPostProcess(PaletteManagerPostProcess* _pPaletteManagerPostProcess, const void* _pSettings, u8 _uBaseColorIndex, u8 _uColorsCount, const PaletteManagerPostProcessDescriptor* _pPaletteManagerPostProcessDescriptor, u16 _uPostProcessTime)
{
    PaletteManagerPostProcessRange** pRangesUsagePartition = _pPaletteManagerPostProcess->m_oRangesUsagePartition;
    u8* pActiveRangesCount = &_pPaletteManagerPostProcess->m_uActiveRangesCount;
    assert(*pActiveRangesCount < MAX_PALETTE_POST_PROCESSING_RANGES);
    const u8 uNewRangeIndex = ARRAY_PARTITION_BACK_TO_FRONT_LAST(*pActiveRangesCount);
    PaletteManagerPostProcessRange* pNewRange = pRangesUsagePartition[uNewRangeIndex];
    pNewRange->m_uFirstColorIndex = _uBaseColorIndex;
    pNewRange->m_uColorsCount = _uColorsCount;
    pNewRange->OnPalettePostProcess = _pPaletteManagerPostProcessDescriptor->OnPalettePostProcess;
    pNewRange->m_pSetting = _pSettings;
    InitializeU16Interpolator(&pNewRange->m_oPostProcessValueInterpolator, 0, _pPaletteManagerPostProcessDescriptor->m_uMaxValue, _uPostProcessTime);

    return uNewRangeIndex;
}

void RemoveRangeFromPaletteManagerPostProcess(PaletteManagerPostProcess* _pPaletteManagerPostProcess, u8 _uIndex)
{
    PaletteManagerPostProcessRange** pRangesUsagePartition = _pPaletteManagerPostProcess->m_oRangesUsagePartition;
    u8* pActiveRangesCount = &_pPaletteManagerPostProcess->m_uActiveRangesCount;
    assert(*pActiveRangesCount > 0);
    (void) ARRAY_PARTITION_FRONT_TO_BACK_UNORDERED(pRangesUsagePartition, MAX_PALETTE_POST_PROCESSING_RANGES, *pActiveRangesCount, _uIndex);
}

bool UpdatePaletteManagerPostProcess(PaletteManagerPostProcess* _pPaletteManagerPostProcess, PaletteManager* _pPaletteManager)
{
    u8* pActiveRangesCount = &_pPaletteManagerPostProcess->m_uActiveRangesCount;
    if(*pActiveRangesCount == 0)
        return false;

    PaletteManagerPostProcessRange** pActiveRanges = _pPaletteManagerPostProcess->m_oRangesUsagePartition;
    u16 uRangeIndex = 0;
    bool oColorChangedFlag[ALL_COLORS_COUNT] = {0};
    bool* pPaletteColorChangedFlags = _pPaletteManager->m_oPaletteColorChangedFlags;
    while(uRangeIndex < *pActiveRangesCount)
    {
        PaletteManagerPostProcessRange* pRange = pActiveRanges[uRangeIndex];
        const u16 uBaseColorIndex = pRange->m_uFirstColorIndex;
        const u16 uColorsCount = pRange->m_uColorsCount;
        //Set the modification flag in the post-process colors
        memset(&oColorChangedFlag[uBaseColorIndex], true, uColorsCount * sizeof(bool));
        //Make sure to clear possible modification flags for the palette
        memset(&pPaletteColorChangedFlags[uBaseColorIndex], false, uColorsCount * sizeof(bool));
        if(UpdateRange(pRange, _pPaletteManager))
            ++uRangeIndex;
        else
            (void) ARRAY_PARTITION_FRONT_TO_BACK_UNORDERED(*pActiveRanges, MAX_PALETTE_POST_PROCESSING_RANGES, *pActiveRangesCount, uRangeIndex);
    }

    UploadPaletteChanges(_pPaletteManagerPostProcess->m_oPaletteColorsPostProcess, oColorChangedFlag);

    return *pActiveRangesCount > 0;
}

static bool UpdateRange(PaletteManagerPostProcessRange* _pRange, PaletteManager* _pPaletteManager)
{
    _pRange->OnPalettePostProcess(_pPaletteManager, _pRange);

    return !IsU16InterpolatorFinished(&_pRange->m_oPostProcessValueInterpolator);
}