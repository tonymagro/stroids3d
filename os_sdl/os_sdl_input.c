/*
 * JoyStick and Mouse input
 */
#include "../game/game.h"
#include "s_sdl.h"

#define XAXIS	0
#define YAXIS	1

#define MAX_JOYSTICKS	4

SDL_Joystick *joysticks[MAX_JOYSTICKS];

int num_joysticks = 0;

int selected_joystick = 0;

sboolean disable_joysticks = False;

sboolean update_input = True;
int input_delay = 1000;

int mouse_dx, mouse_dy;

void Input_Frame( void )
{
	Input_UpdateMouse();
	Input_UpdateJoyStick();
}

void Input_UpdateJoyStick()
{
	//int i;

	if( disable_joysticks )
		return;

	if( !update_input )
		return;

	//for( i = 0; i < 1; i++ )
	//{
	G_JoyStickAxis( SDL_JoystickGetAxis( joysticks[0], XAXIS ), -SDL_JoystickGetAxis( joysticks[0], YAXIS ) );
	//}
}


void Input_UpdateMouse( void )
{
	
	SDL_GetRelativeMouseState( &mouse_dx, &mouse_dy );
	G_MouseMove( mouse_dx, mouse_dy );
}

void Input_Init( void )
{
	int i;

	printf( "Initializing Joystick and Mouse input\n" );

	if( !JOYSTICK_SUPPORT )
	{
		disable_joysticks = True;
		printf( "Joystick support disabled\n" );
		return;
	}

	num_joysticks = SDL_NumJoysticks();

	printf( "Number of Joysticks: %i\n", num_joysticks );

	if( num_joysticks == 0 ) {
		gi.joystick = False;
		return;
	}

	gi.joystick = True;

	/* Enable JoyStickEven Handling */
	SDL_JoystickEventState( SDL_ENABLE );

	if( num_joysticks > MAX_JOYSTICKS )
	{
		printf( "Only %i joysticks supported\n", MAX_JOYSTICKS );
		num_joysticks = MAX_JOYSTICKS;
	}

	// Check for joystick
	for( i = 0; i < num_joysticks; i++ ) 
	{
		// Open joystick
		joysticks[i] = SDL_JoystickOpen(i);
  
		if( joysticks[i] )
		{
			printf("Opened Joystick %i\n", i );
			printf("Name: %s\n", SDL_JoystickName(i) );
			printf("Number of Axes: %d\n", SDL_JoystickNumAxes(joysticks[i]) );
			printf("Number of Buttons: %d\n", SDL_JoystickNumButtons(joysticks[i]) );
			printf("Number of Balls: %d\n", SDL_JoystickNumBalls(joysticks[i]) );
		}
		else 
		{
			printf("Couldn't open Joystick 0\n");
  
			// Close if opened
			if( SDL_JoystickOpened(i) )
				SDL_JoystickClose(joysticks[i]);
		}
	}

}

void Input_Shutdown( void )
{
	int i;

	printf( "Shutting Down Joystick and Mouse input\n" );
	
	for( i = 0; i < num_joysticks; i++ ) 
	{
		// Close if opened
		if( SDL_JoystickOpened(i) )
			SDL_JoystickClose(joysticks[i]);
	}
}
