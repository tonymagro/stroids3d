#include "s_shared.h"

sboolean		wait;	// Wait until next frame to execute buffer
void Cmd_fWait( void )
{
	wait = True;
}


buffer_t	commandbuf;
byte	command_data[8192];
byte	commands_defer_data[8192];

void Cbuf_Init( void )
{
	printf( "Initializing Command Buffer..\n" );
	// Create the command buffer_t
	Buf_Init( &commandbuf, command_data, sizeof( command_data ) ); 
}

/* --
Adds Command to the end of the command buffer
----- */
void Cbuf_Add( char* text )
{
	int length;
	length = (int)strlen( text );

	// check if there is overflow
	if( commandbuf.currentsize + length >= commandbuf.maxsize )
	{
		printf( "Cbuf_Add - Overflow\n" );
		return;
	}

	Buf_Write( &commandbuf, text, (int)strlen( text ) );
}

/* --
Adds command directly after the current command
----- */
void Cbuf_Insert( char* text )
{
	char	*temp;
	int		templength;
	
	// Store anything that is currently in the buffer and clear it
	templength = commandbuf.currentsize;
	if( templength )
	{
		temp = malloc( templength );
		memcpy( temp, commandbuf.data, templength );
		Buf_Clear( &commandbuf );
	}
	else
	{
		temp = NULL;
	}
		
	// Add the text to the end of the empty buffer
	Cbuf_Add( text );
	
	// ReAdd Stored Data back into the buffer so its after the text
	if( templength )
	{
		Buf_Write( &commandbuf, temp, templength );
		free( temp );
	}
}

/* --
Execute Commands in buffer
----- */
void Cbuf_Exec( void )
{
	int		index;
	char	*text;
	char	line[1024];
	int		quotes;

	alias_count = 0;		// don't allow infinite alias loops

	while( commandbuf.currentsize )
	{
		text = (char*)commandbuf.data;

		quotes = 0;

		// Find a newline or ;
		for( index = 0; index < commandbuf.currentsize; index++ )
		{
			if( text[index] == '"' )
				quotes++;
			if( !(quotes&1) && text[index] == ';' )
				break;	// Quoted string can have any character
			if( text[index] == '\n' )
				break;	// newline character
		}

		memcpy( line, text, index );
		line[index] = 0;

		if( index == commandbuf.currentsize )
			commandbuf.currentsize = 0;
		else
		{
			index++;
			commandbuf.currentsize -= index;
			memmove( text, text + index, commandbuf.currentsize );
		}

		Cmd_ExecuteString( line );

		if( wait )
		{
			wait = False;
			break;
		}
	}
}
