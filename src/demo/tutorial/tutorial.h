#ifndef _TUTORIAL_H_
#define _TUTORIAL_H_

#include <demo/tutorial/tutorialitem.h>

#include <engine/sequence/sequence.h>
#include <engine/sequence/sequencestep.h>

struct Engine;

typedef struct Tutorial
{
    TutorialItem m_oTutorialItems[3];
    SequenceStep m_oSequenceElements[3];
    Sequence m_oSequence;
} Tutorial;

u16 InitializeTutorial(Tutorial* _pTutorial, struct Engine* _pEngine, u8 _uPaletteIndex, u16 _uBaseTileIndex);
void ReleaseTutorial(Tutorial* _pTutorial);
void UpdateTutorial(Tutorial* _pTutorial);

#endif //#ifndef _TUTORIAL_H_