#ifndef _TUTORIAL_ITEM_H_
#define _TUTORIAL_ITEM_H_

#include <engine/sequence/sequence.h>
#include <engine/sequence/sequencestep.h>
#include <engine/types/int16interpolator.h>

#include <genesis.h>

struct Engine;
struct Test
{
    int x;
};

//Represents a piece of the tutorial. It's comprised by a sprite that appears from a specific position with a fade-in, moves towards a certain position, and finally fades out.
//NOTE: Colors are expected to be used only by this item
typedef struct TutorialItem
{
    bool m_bIsEnabled;
    Sprite* m_pSprite;
    V2s16 m_oTargetPosition;
    V2s16 m_oFadePositionDelta;
    s16Interpolator m_oXInterpolator;
    s16Interpolator m_oYInterpolator;
    u8 m_uAbsoluteBaseColorIndex;
    u8 m_uColorsCount;

    SequenceStep m_oSequenceElements[3];
    Sequence m_oSequence;

    struct Engine* m_pEngine;
} TutorialItem;

u16 InitializeTutorialItem(TutorialItem* _pTutorialItem, struct Engine* _pEngine, const SpriteDefinition* _pSpriteDefinition, const V2s16* _pTargetPosition, const V2s16* _pDirection, u8 _uPaletteIndex, u8 _uBaseColorIndex, u8 _uColorsCount, u16 _uBaseTileIndex);
void ReleaseTutorialItem(TutorialItem* _pTutorialItem);
void EnableTutorialItem(TutorialItem* _pTutorialItem);
void UpdateTutorialItem(TutorialItem* _pTutorialItem);

#endif //#ifndef _TUTORIAL_ITEM_H_