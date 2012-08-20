#define DIRECTINPUT_VERSION 0x0800
#include <windows.h>
#include <dinput.h>

#include "../game/game.h"


sboolean bEnableFeedback;

LPDIRECTINPUTDEVICE8  g_lpDIDevice = NULL;


void OS_InitForceFeedback( void )
{
	bEnableFeedback = True;
}

void OS_ShutdownForceFeedback( void )
{

}
