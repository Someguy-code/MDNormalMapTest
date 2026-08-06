#ifndef _MOUSE_UTILS_H_
#define _MOUSE_UTILS_H_

#include <genesis.h>

void EnableMouseSupport(u16 _uJoyID);
V2s16 GetMouseSpeed(u16 _uJoyID, bool _bCosumeMouseMovement);
bool CheckMouseButtons(u16 _uJoyID, u16 _uChanged, u16 _uState, u16 _uButtons);
void ResetMouseSpeed(u16 _uJoyID);

#endif //#ifndef _MOUSE_UTILS_H_