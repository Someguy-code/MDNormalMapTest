#include "palettemanagerutils.h"

#include <engine/constants/paletteconstants.h>

void UploadPaletteChanges(const u16* _uPaletteColors, const bool* _pColorChangedFlags)
{
    u8 uFirstChangedColorIndex = 0;
    bool bLastColorChanged = false;
    for(u8 uColorIndex = 0; uColorIndex < ALL_COLORS_COUNT; ++uColorIndex)
    {
        const bool bColorChanged = _pColorChangedFlags[uColorIndex];
        if(!bColorChanged && bLastColorChanged)
            PAL_setColors(uFirstChangedColorIndex, &_uPaletteColors[uFirstChangedColorIndex], uColorIndex - uFirstChangedColorIndex, DMA_QUEUE);
        else if(bColorChanged && !bLastColorChanged)
            uFirstChangedColorIndex = uColorIndex;

        bLastColorChanged = bColorChanged;
    }

    if(bLastColorChanged)
        PAL_setColors(uFirstChangedColorIndex, &_uPaletteColors[uFirstChangedColorIndex], ALL_COLORS_COUNT - uFirstChangedColorIndex, DMA_QUEUE);
}