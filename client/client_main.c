#include "client.h"
#include "console.h"

void Client_Init( void )
{
	printf( "Initializing Client\n" );

	// Initialize Renderer
	Ren_Init();

	/* resize the initial window */
    Ren_WindowSize( vidinfo.width, vidinfo.height );

	/* Initialize sound */
	Snd_Init();
}

void Client_Frame( int msec )
{
}

void Client_Shutdown( void )
{
	printf( "Shutting Down Client\n" );

	Snd_Shutdown();
	Ren_Shutdown();
}

