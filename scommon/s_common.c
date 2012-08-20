#include "../server/server.h"
#include "../client/client.h"

#include "s_shared.h"

/* Version Info */
char*	versioninfo;
cvar*	version;

void Main_Init( void )
{

	// Initialize Swap
	Swap_Init();
	
	// Initialize Command Buffer
	Cbuf_Init();

	// Initialize Commands
	Cmd_Init();

	// Initialize Console Variables
	Cvar_Init();

	// Initialize File System
	Fsys_Init();


	// Set up Cvars
	// set the version
	versioninfo = va("%s - %s", S_VERSION, __DATE__ );
	// Set the actual Cvar
	version = Cvar_Get( "version", versioninfo, CVAR_READONLY );


	// Initialize memory
	mem_init();

	/* initialize input devices */
	Input_Init();

	// Add Config files to command buffer
	Cbuf_Add( "exec config.cfg\n" );

	// Execute Commands
	Cbuf_Exec();

	Client_Init();
	Server_Init();
	//Client_Init();


	/* Print Memory usage */
	printf( "MEMORY: %i bytes allocated from %i allocs\n", mem_malloc_bytes(), mem_malloc_number() );
}

void Main_Shutdown( void )
{

	//Client_Shutdown();
	Server_Shutdown();
	Client_Shutdown();
	Input_Shutdown();

	mem_shutdown();
}

void Main_Frame( int msec )
{
	/* Update Mouse/Joystick motion*/
	Input_Frame();
	Server_Frame( msec );
	Client_Frame( msec );
}
