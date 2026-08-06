#include "inputmanager.h"

static InputManager* m_pInputManager = NULL;

static void JoyEventCallbackInputManager(u16 _uJoyID, u16 _uChanged, u16 _uState);

void InitializeInputManager(InputManager* _pInputManager)
{
    *_pInputManager = (InputManager){};
    m_pInputManager = _pInputManager;
    JOY_setEventHandler(&JoyEventCallbackInputManager);
}

void SetInputManagerCallback(InputManager* _pInputManager, void* _pData, InputCallback _pInputCallback)
{
    //Prevent the callback from being called while changing data
    _pInputManager->m_pInputCallback = NULL;
    _pInputManager->m_pData = _pData;
    _pInputManager->m_pInputCallback = _pInputCallback;
}

static void JoyEventCallbackInputManager(u16 _uJoyID, u16 _uChanged, u16 _uState)
{
    if(m_pInputManager != NULL && m_pInputManager->m_pInputCallback != NULL)
        m_pInputManager->m_pInputCallback(m_pInputManager->m_pData, _uJoyID, _uChanged, _uState);
}