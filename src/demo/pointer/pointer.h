#ifndef _POINTER_H_
#define _POINTER_H_

#include <genesis.h>

#include <demo/pointer/pointerlimits.h>

struct Engine;

//Data to manage a pointer that can move around the screen. Support a pad in port 1 and a mouse in port 2
typedef struct Pointer
{
    Sprite* m_pSprite;
    V2s16 m_oOffset;
    V2u16 m_oPosition;
    //Pointer speed contributed by the pad
    V2s16 m_oPadSpeed;
    PointerLimits m_oPointerLimits;
} Pointer;

extern PointerLimits NullPointerLimits;

void InitializePointer(Pointer* _pPointer, const SpriteDefinition* _pPointerSpriteDefinition, u8 _uPalIndex, const V2u16* _pPosition, struct Engine* _pEngine);
void ReleasePointer(Pointer* _pPointer);
void SetPointerLimits(Pointer* _pPointer, const PointerLimits* _pPointerLimits);
void SetPointerPosition(Pointer* _pPointer, const V2u16* _pPosition, bool _bPriority);
V2s16 GetPointerSpeed(const Pointer* _pPointer, bool _bCosumeMouseMovement);
void UpdatePointer(Pointer* _pPointer);
void PointerJoyEventHandler(Pointer* _pPointer, u16 _uJoyID, u16 _uChanged, u16 _uState);

#endif //#ifndef _POINTER_H_