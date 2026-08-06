#include "pointer.h"

#include <engine/engine.h>
#include <engine/input/mouseutils.h>
#include <engine/palettemanager/palettemanager.h>

PointerLimits NullPointerLimits = {};

static void SetSpritePosition(Sprite* _pSprite, const V2s16* _pOffset, const V2u16* _pPosition, bool _bPriority);

void InitializePointer(Pointer* _pPointer, const SpriteDefinition* _pPointerSpriteDefinition, u8 _uPalIndex, const V2u16* _pPosition, struct Engine* _pEngine)
{
    s16 iOffsetX = -_pPointerSpriteDefinition->w / 2;
    s16 iOffsetY = -_pPointerSpriteDefinition->h / 2;
    Sprite* pPointerSprite = SPR_addSprite(_pPointerSpriteDefinition, iOffsetX, iOffsetY, TILE_ATTR(_uPalIndex, TRUE, FALSE, FALSE));
    SetSpritePosition(pPointerSprite, &(V2s16){iOffsetX, iOffsetY}, _pPosition, false);
    const Palette* pPalette = _pPointerSpriteDefinition->palette;
    SetColorsInPaletteManager(&_pEngine->m_oPaletteManager, _uPalIndex * 16, pPalette->data, pPalette->length);
    *_pPointer = (Pointer){
        .m_pSprite = pPointerSprite,
        .m_oOffset = {iOffsetX, iOffsetY},
        .m_oPosition = *_pPosition,
        .m_oPadSpeed = {0, 0},
        .m_oPointerLimits = NullPointerLimits
    };
}

void ReleasePointer(Pointer* _pPointer)
{
    SPR_releaseSprite(_pPointer->m_pSprite);
}

void SetPointerLimits(Pointer* _pPointer, const PointerLimits* _pPointerLimits)
{
    _pPointer->m_oPointerLimits = *_pPointerLimits;
}

void SetPointerPosition(Pointer* _pPointer, const V2u16* _pPosition, bool _bPriority)
{
    _pPointer->m_oPosition = *_pPosition;
    SetSpritePosition(_pPointer->m_pSprite, &_pPointer->m_oOffset, _pPosition, _bPriority);
}

V2s16 GetPointerSpeed(const Pointer* _pPointer, bool _bCosumeMouseMovement)
{
    const V2s16 oMouseSpeed = GetMouseSpeed(JOY_2, _bCosumeMouseMovement);
    return (V2s16){
        _pPointer->m_oPadSpeed.x + oMouseSpeed.x,
        _pPointer->m_oPadSpeed.y + oMouseSpeed.y
    };
}

void UpdatePointer(Pointer* _pPointer)
{
    const V2s16 oPointerSpeed = GetPointerSpeed(_pPointer, true);
    if(oPointerSpeed.x != 0 || oPointerSpeed.y != 0)
    {
        const V2u16* pPoistion = &_pPointer->m_oPosition;
        const V2u16 oNewPosition = GetPointerLimitedPosition(&_pPointer->m_oPointerLimits, &(V2s16){
            pPoistion->x + oPointerSpeed.x, pPoistion->y + oPointerSpeed.y});
        SetPointerPosition(_pPointer, &oNewPosition, true);
    }
}

//Compute the pad speed contribution
void PointerJoyEventHandler(Pointer* _pPointer, u16 _uJoyID, u16 _uChanged, u16 _uState)
{
    switch(_uJoyID)
    {
        case JOY_1:
            const s8 iSpeedMultiplier = 1;
            _pPointer->m_oPadSpeed = (V2s16){
                .x = 
                    (_uState & BUTTON_RIGHT) != 0 ? iSpeedMultiplier :
                    (_uState & BUTTON_LEFT) != 0 ? -iSpeedMultiplier : 0,
                .y =
                    (_uState & BUTTON_UP) != 0 ? -iSpeedMultiplier :
                    (_uState & BUTTON_DOWN) != 0 ? iSpeedMultiplier : 0
            };
            break;
        default:
            break;
    }
}

static void SetSpritePosition(Sprite* _pSprite, const V2s16* _pOffset, const V2u16* _pPosition, bool _bPriority)
{
    SPR_setPosition(_pSprite, _pPosition->x + _pOffset->x, _pPosition->y + _pOffset->y);   
    SPR_setPriority(_pSprite, _bPriority);
}