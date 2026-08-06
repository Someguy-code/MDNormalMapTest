#ifndef _DEMO_H_
#define _DEMO_H_

#include <demo/pointer/pointer.h>
#include <demo/tutorial/tutorial.h>
#include <demo/statemachine/demostatemachine.h>
#include <demo/materialmodifiercontroller/materialmodifiercontroller.h>

#include <engine/material/material.h>

struct Engine;

typedef struct Demo
{
    Material m_oMaterial;
    Pointer m_oPointer;
    DemoStateMachine m_oStateMachine;
    Tutorial m_oTutorial;
    MaterialModifierController m_oMaterialModifierController;

    struct Engine* m_pEngine;
} Demo;

void InitializeDemo(Demo* _pDemo, struct Engine* _pEngine);
void UpdateDemo(Demo* _pDemo);

#endif //#ifndef _DEMO_H_
