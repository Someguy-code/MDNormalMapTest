#ifndef _INPUT_MANAGER_H_
#define _INPUT_MANAGER_H_

#include <engine/input/inputcallback.h>

typedef struct InputManager
{
    void* m_pData;
    InputCallback* m_pInputCallback;
} InputManager;

void InitializeInputManager(InputManager* _pInputManager);
void SetInputManagerCallback(InputManager* _pInputManager, void* _pData, InputCallback _pInputCallback);

#endif //#ifndef _INPUT_MANAGER_H_