#ifndef _ENGINE_H_
#define _ENGINE_H_

#include <genesis.h>
#include <engine/palettemanager/palettemanager.h>
#include <engine/input/inputmanager.h>

typedef struct Engine
{
    PaletteManager m_oPaletteManager;
    InputManager m_oInputManager;
} Engine;

void InitializeEngine(Engine* _pEngine);
void UpdateEngine(Engine* _pEngine);

#endif //#ifndef _ENGINE_H_