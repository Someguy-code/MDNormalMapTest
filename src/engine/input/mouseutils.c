#include "mouseutils.h"

static bool IsMouseConnected(u16 _uJoyID);

void EnableMouseSupport(u16 _uJoyID)
{
    JOY_setSupport(_uJoyID, JOY_SUPPORT_MOUSE);
}

V2s16 GetMouseSpeed(u16 _uJoyID, bool _bCosumeMouseMovement)
{
    s16 iMouseSpeedX = 0;
    s16 iMouseSpeedY = 0;

    if(IsMouseConnected(_uJoyID))
    {
        iMouseSpeedX = JOY_readJoypadX(_uJoyID);
        iMouseSpeedY = JOY_readJoypadY(_uJoyID);
        if(_bCosumeMouseMovement)
            ResetMouseSpeed(_uJoyID);
    }
    return (V2s16){iMouseSpeedX, iMouseSpeedY};
}

bool CheckMouseButtons(u16 _uJoyID, u16 _uChanged, u16 _uState, u16 _uButtons)
{
    return IsMouseConnected(_uJoyID) ?
        _uChanged & _uState & _uButtons :
        false;
}

void ResetMouseSpeed(u16 _uJoyID)
{
    JOY_writeJoypadX(_uJoyID, 0);
    JOY_writeJoypadY(_uJoyID, 0);
}

static bool IsMouseConnected(u16 _uJoyID)
{
    const u8 uJoyType = JOY_getPortType(_uJoyID);
    return uJoyType == JOY_TYPE_TRACKBALL || uJoyType == JOY_TYPE_MOUSE;
}