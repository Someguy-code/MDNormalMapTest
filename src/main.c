#include <genesis.h>

#include <demo/demo.h>

#include <engine/engine.h>

static Engine m_oEngine;
static Demo m_oDemo;

[[noreturn]] int main()
{   
    InitializeEngine(&m_oEngine);

    InitializeDemo(&m_oDemo, &m_oEngine);

    while(true)
    {   
        UpdateDemo(&m_oDemo);
        UpdateEngine(&m_oEngine);

        SYS_doVBlankProcess();
    }
}