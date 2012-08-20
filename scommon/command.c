// Quake 2
#include "s_shared.h"
#include "s_common.h"

/* PROTOTYPES */
void	Cmd_fExec( void );
void	Cmd_fCmdls( void );

#define	ALIAS_LOOP_COUNT	16
int		alias_count;		// for detecting runaway loops

/* Command Alias */
typedef struct alias_struct
{
	struct alias_struct*	next;

	char	name[32];
	char*	value;
}alias;

/* Command */
typedef struct command_struct
{
	struct command_struct*	next;	// Linked List
	char*	name;					// Name of command
	fCommand function;		// Pointer to Function to call 
} command;

static	int			cmd_argc;						// Argument count
static	char*		cmd_argv[MAX_STRING_TOKENS];	// Arbument value
static	char*		cmd_null = "";					// Null String
static	char		cmd_args[MAX_STRING_CHARS];		// Argument string

static	command*	cmd_functions;		// possible commands to execute
static	alias*		cmd_alias;

void Cmd_Init( void )
{
	printf( "Initializing Commands..\n" );

	Cmd_Add( "exec", Cmd_fExec );
	Cmd_Add( "cmdls", Cmd_fCmdls );
}

/* --
Returns number of arguments passed to command
----- */
int Cmd_argc( void )
{
	return cmd_argc;
}

/* --
Returns argument at place of number
----- */
char*	Cmd_argv( int arg )
{
	// Check if argument number is greater then total number of arguments
	if( arg >= cmd_argc )
		return cmd_null;

	// Return argument
	return cmd_argv[arg];
}

/* --
Returns arguments as one long string
----- */
char*	Cmd_args( void )
{
	return cmd_args;
}


char* Cmd_MacroExpandString( char* text )
{
	int		i, j, count, len;
	sboolean	inquote;
	char	*scan;
	static	char	expanded[MAX_STRING_CHARS];
	char	temporary[MAX_STRING_CHARS];
	char	*token, *start;

	inquote = False;
	scan = text;

	len = (int)strlen (scan);
	if (len >= MAX_STRING_CHARS)
	{
		printf ("Line exceeded %i chars, discarded.\n", MAX_STRING_CHARS);
		return NULL;
	}

	count = 0;

	for (i=0 ; i<len ; i++)
	{
		if (scan[i] == '"')
			inquote ^= 1;
		if (inquote)
			continue;	// don't expand inside quotes
		if (scan[i] != '$')
			continue;
		// scan out the complete macro
		start = scan+i+1;
		token = SHR_Parse (&start);
		if (!start)
			continue;
	
		token = Cvar_VariableString (token);

		j = (int)strlen(token);
		len += j;
		if (len >= MAX_STRING_CHARS)
		{
			printf ("Expanded line exceeded %i chars, discarded.\n", MAX_STRING_CHARS);
			return NULL;
		}

		strncpy (temporary, scan, i);
		strcpy (temporary+i, token);
		strcpy (temporary+i+j, start);

		strcpy (expanded, temporary);
		scan = expanded;
		i--;

		if (++count == 100)
		{
			printf ("Macro expansion loop, discarded.\n");
			return NULL;
		}
	}

	if (inquote)
	{
		printf ("Line has unmatched quote, discarded.\n");
		return NULL;
	}

	return scan;
}


void Cmd_TokenizeString( char *text, sboolean macroExpand )
{
	int		i;
	char	*com_token;

// clear the args from the last string
	for (i=0 ; i<cmd_argc ; i++)
		free (cmd_argv[i]);
		
	cmd_argc = 0;
	cmd_args[0] = 0;
	
	// macro expand the text
	if (macroExpand)
		text = Cmd_MacroExpandString( text );
	if (!text)
		return;

	while (1)
	{
// skip whitespace up to a /n
		while (*text && *text <= ' ' && *text != '\n')
		{
			text++;
		}
		
		if (*text == '\n')
		{	// a newline seperates commands in the buffer
			text++;
			break;
		}

		if (!*text)
			return;

		// set cmd_args to everything after the first arg
		if (cmd_argc == 1)
		{
			int		l;

			strcpy (cmd_args, text);

			// strip off any trailing whitespace
			l = (int)strlen(cmd_args) - 1;
			for ( ; l >= 0 ; l--)
				if (cmd_args[l] <= ' ')
					cmd_args[l] = 0;
				else
					break;
		}
			
		com_token = SHR_Parse (&text);
		if (!text)
			return;

		if (cmd_argc < MAX_STRING_TOKENS)
		{
			cmd_argv[cmd_argc] = malloc (strlen(com_token)+1);
			strcpy (cmd_argv[cmd_argc], com_token);
			cmd_argc++;
		}
	}
}

void Cmd_ExecuteString( char *text )
{	
	command		*cmd;
	alias		*a;

	Cmd_TokenizeString( text, True );
			
	// execute the command line
	if( !Cmd_argc() )
		return;		// no tokens

	// check functions
	for( cmd=cmd_functions; cmd ; cmd=cmd->next )
	{
		if( !strcmp( cmd_argv[0], cmd->name ))
		{
			if ( !cmd->function )
			{
				Cmd_ExecuteString( va( "cmd %s", text ) );
			}
			else
				cmd->function();
			return;
		}
	}

	// check alias
	for( a=cmd_alias; a ;a=a->next )
	{
		if(!strcmp(cmd_argv[0], a->name))
		{
			if(++alias_count == ALIAS_LOOP_COUNT)
			{
				printf ("ALIAS_LOOP_COUNT\n");
				return;
			}
			Cbuf_Insert( a->value );
			return;
		}
	}

	// check cvars
	if( Cvar_IsCommand() )
		return;
}

void Cmd_Add( char* name, fCommand function )
{
	command		*cmd;

	if( Cvar_VariableString( name )[0] )
	{
		printf( "Cmd_Add - %s is a variable\n" );
		return;
	}

	for( cmd = cmd_functions; cmd; cmd = cmd->next )
	{
		if( !strcmp( name, cmd->name ) )
		{
			printf( "Cmd_Add - %s is already a command\n", name );
			return;
		}
	}

	// There are no problems so create the command
	cmd = malloc( sizeof(command) );
	cmd->name = name;
	cmd->function = function;
	cmd->next = cmd_functions;
	cmd_functions = cmd;
}


void Cmd_fExec( void )
{
	char	*f, *f2;
	int		len;

	if( Cmd_argc() != 2)
	{
		printf ("exec <file>\n");
		return;
	}

	len = Fsys_LoadFile( Cmd_argv(1), (void **)&f );
	if( !f )
	{
		printf( "couldn't exec %s\n", Cmd_argv(1) );
		return;
	}

	printf( "execing %s ..\n",Cmd_argv(1) );
	
	// the file doesn't have a trailing 0, so we need to copy it off
	f2 = malloc(len+1);
	memcpy( f2, f, len );
	f2[len] = 0;

	Cbuf_Insert( f2 );

	free(f2);
	Fsys_FreeFile( f );
}

void Cmd_fCmdls( void )
{
	command*	cmd;

	for( cmd = cmd_functions; cmd; cmd = cmd->next )
	{
		printf( " %s\n", cmd->name );
	}
}
	
