// quake 2
#include "s_common.h"

#define MAX_GAMEPATH 128
#define MAX_FILEPATH 128

#define GAMEDIR "."

char	fsys_gamedir[MAX_GAMEPATH];
cvar*	fsys_game;

void Fsys_Init( void )
{
	printf( "Initializing File System..\n" );
}

void Fys_Shutdown( void )
{
}

void Fsys_SetGameDir( char* dir )
{
	fsys_game = Cvar_Add( ".", dir, CVAR_READONLY );
}

char* Fsys_GetGameDir( void )
{

	if( fsys_game )
	{
		char* strGamePath;
		strGamePath = SHR_StrCpy( fsys_game->string );
		return strGamePath;
	}
	else
		return GAMEDIR;
}


/* -- 
This will load a file from directory or dat file
----- */ 
int Fsys_LoadFile( char* path, void** buffer_t )
{
	FILE	*h;
	byte	*buf;
	int		length;

	buf = NULL;

	// Look for file is directory or dat file
	length = Fsys_OpenFile( path, &h );

	if( !h )
	{
		if( buffer_t )
			*buffer_t = NULL;
		return -1;
	}

	if( !buffer_t )
	{
		fclose( h );
		return length;
	}

	buf = malloc( length );
	*buffer_t = buf;

	// This is just here so I can do cool progress bars
	Fsys_Read( buf, length, h );

	fclose( h );

	return length;
}

void Fsys_FreeFile( void* buffer_t )
{
	free( buffer_t );
}

/* -- 
This will Find the file in a directory or dat file 
----- */
int Fsys_OpenFile( char* filename, FILE **file )
{
	char			netpath[MAX_FILEPATH];
		
	sprintf( netpath, "%s/%s", Fsys_GetGameDir(), filename );
			
	*file = fopen (netpath, "rb");
	if( !*file )
	{
		printf ("Can't find file: %s\n", filename);
	
		*file = NULL;
		return -1;
	}
			
	printf( "OpenFile - %s\n", netpath );

	return Fsys_FileLength( *file );
}

#define	MAX_READ	0x10000		// read in blocks of 64k
void Fsys_Read( void* buffer_t, int len, FILE* f )
{
	int		block, remaining;
	int		read;
	byte	*buf;
	int		tries;

	buf = (byte *)buffer_t;

	// read in chunks for progress bar
	remaining = len;
	tries = 0;
	while(remaining)
	{
		block = remaining;
		if (block > MAX_READ)
			block = MAX_READ;
		read = (int)fread( buf, 1, block, f );
		if (read == 0)
		{
			// FIXME: THIS DOES NOT CHECK FOR SLOWER MEDIA
			OS_Error( 1, "Fsys_Read - 0 bytes read" ); // fatal
		}

		if(read == -1)
			OS_Error( 1, "Fsys_Read: -1 bytes read" ); // fatal

		// prog bar
		printf("*");

		remaining -= read;
		buf += read;
	}

	printf( "\n" );
}

int Fsys_FileLength( FILE* f )
{
	int		pos;
	int		end;

	pos = ftell (f);
	fseek (f, 0, SEEK_END);
	end = ftell (f);
	fseek (f, pos, SEEK_SET);

	return end;
}
