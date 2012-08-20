#include "game.h"

int key_repeats[K_LAST];
int key_down[K_LAST];

void G_InitKeys( void )
{
	int i;

	for( i = 0; i < K_LAST; i++ )
	{
		key_repeats[i] = 0;
		key_down[i] = 0;
	}
}

sboolean G_KeyDown( int key )
{
	return key_down[key];
}

void G_GameKey( int key, sboolean down )
{
	key_down[key] = down;
}

void G_IntroKey( int key, sboolean down )
{
	Intro_KeyDown( key, down );
	key_down[key] = down;
}


void G_MenuKey( int key, sboolean down )
{
	Menu_KeyDown( key, down );
	key_down[key] = down;
}

void G_KeyEvent( int key, sboolean down )
{
	switch( gi.gamestate )
	{
	case GS_RUNNING:
		G_GameKey( key, down );
		break;
	case GS_MENU:
		G_MenuKey( key, down );
		break;
	case GS_INTRO:
		G_IntroKey( key, down );
		break;
	default:
		break;
	}
}

void G_MouseMove( int deltax, int deltay )
{
	playerinfo.mouse_dx = deltax;
	playerinfo.mouse_dy = deltay;
}

void G_JoyStickAxis( int deltax, int deltay )
{
	playerinfo.joy_dx = deltax;
	playerinfo.joy_dy = deltay;
}

void G_KeyFrame( void )
{
	switch( gi.gamestate )
	{
		case GS_MENU:
			key_down[K_MOUSE1] = 0;
			break;
		case GS_RUNNING:
			key_down['j'] = 0;
			key_down['e'] = 0;
			key_down['k'] = 0;
			key_down['l'] = 0;
			//key_down['o'] = 0;
			//key_down['p'] = 0;
			//key_down[K_KP_PLUS] = 0;
			//key_down[K_KP_MINUS] = 0;
			break;
		default:
			break;
	}
}
