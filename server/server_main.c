#include "server.h"

void Server_Shutdown( void )
{
	// Shutdown Game
	G_Shutdown();
}

void Server_Init( void )
{
	printf( "Initializing Server\n" );

	/* Initialize Game */
	G_Init();
}

void Server_RunGameFrame( void )
{
	G_Main();
}

void Server_Frame( int msec )
{
	Server_RunGameFrame();
}

