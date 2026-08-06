#include "engine.h"

void InitializeEngine(Engine* _pEngine)
{
    SPR_init();
    *_pEngine = (Engine){};
    InitializeInputManager(&_pEngine->m_oInputManager);
    InitializePaletteManager(&_pEngine->m_oPaletteManager);
}

void UpdateEngine(Engine* _pEngine)
{
    UpdatePaletteManager(&_pEngine->m_oPaletteManager);
    SPR_update();
}