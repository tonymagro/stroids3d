// Quake2
#include "s_shared.h"

/* GLOBALS */
cvar*	cvar_list;

/* PROTOTYPES */
void	Cvar_fSet( void );
void	Cvar_fCvarls( void );
cvar*	Cvar_Set(char* name, char* value, sboolean force);
static	cvar*	Cvar_Find( char* name );
static	sboolean	Cvar_CheckInfo( char* name, char* value );


void Cvar_Init( void )
{
	printf( "Initializing Console Variables..\n" );

	Cmd_Add( "set", Cvar_fSet );
	Cmd_Add( "cvarls", Cvar_fCvarls );
}


cvar* Cvar_Add( char* name, char* value, int flags )
{
	cvar*	tCvar;
	
	// Make sure information is valid
	if( !Cvar_CheckInfo( name, value ) )
	{
		printf( " Invalid Format: %s = %s\n", name, value );
		return NULL;
	}

	// Find the variable
	tCvar = Cvar_Find( name );

	// If it is not found create it
	if( !tCvar )
	{
		return Cvar_Get( name, value, flags );
	}

	tCvar->changed = True;

	// The Cvar is already set so reset the info
	free( tCvar->string );

	tCvar->string = SHR_StrCpy( value );
	tCvar->value = (float)atof( tCvar->string );
	tCvar->flags = flags;

	return tCvar;
}

cvar* Cvar_Set(char* name, char* value, sboolean force)
{
	cvar* tCvar;

	tCvar = Cvar_Find( name );

	if( !tCvar )
	{
		return Cvar_Get( name, value, 0 );
	}

	if( !force )
	{
		if( tCvar->flags & CVAR_READONLY )
		{
			printf ("%s is readonly.\n", name);
			return tCvar;
		}
	}

	// See if value has changed
	if( !strcmp( value, tCvar->string))
		return tCvar;

	tCvar->changed = True;
	
	free( tCvar->string );
	
	tCvar->string = SHR_StrCpy( value );
	tCvar->value = (float)atof( tCvar->string );

	return tCvar;
}

void Cvar_SetValue( char *var_name, float value )
{
	char	val[32];

	if( value == (int)value )
		sprintf( val, "%i",(int)value );
	else
		sprintf( val, "%f", value );

	Cvar_Set( var_name, val, False );
}

cvar* Cvar_Get( char* name, char* value, int flags )
{
	cvar*	tCvar;

	tCvar = Cvar_Find( name );

	if( tCvar )
	{
		tCvar->flags |= flags;
		return tCvar;
	}

	// Cvar doesn't exist so create it
	tCvar = malloc( sizeof(*tCvar) );
	// Name
	tCvar->name = SHR_StrCpy( name );
	// String Value
	tCvar->string = SHR_StrCpy( value );
	// Float Value
	tCvar->value = (float)atof( tCvar->string );
	// Changed
	tCvar->changed = True;
	// Flags
	tCvar->flags = flags;

	// Link Cvar into linked list
	tCvar->next = cvar_list;
	cvar_list = tCvar;

	// return newly created cvar
	return tCvar;
}



static cvar* Cvar_Find( char* name )
{
	cvar* tCvar;

	for( tCvar = cvar_list; tCvar; tCvar = tCvar->next )
	{
		if( strcmp(tCvar->name, name) == 0 )
			return tCvar;
	}

	return NULL;
}

char* Cvar_VariableString( char* name )
{
	cvar* tCvar;
	
	tCvar = Cvar_Find( name );
	if( !tCvar )
		return "";
	return tCvar->string;
}

static	sboolean	Cvar_CheckInfo( char* name, char* value )
{
	return True;
}

void Cvar_OutPutVariables( char* path )
{
	cvar*	var;
	char	buffer[1024];
	FILE	*f;

	f = fopen (path, "a");
	for (var = cvar_list ; var ; var = var->next)
	{
		if (var->flags & CVAR_SAVE)
		{
			sprintf(buffer, "set %s \"%s\"\n", var->name, var->string);
			fprintf (f, "%s", buffer);
		}
	}
	fclose (f);
}

sboolean Cvar_IsCommand( void )
{

	cvar*	var;

	var = Cvar_Find( Cmd_argv(0) );
	if( !var )
		return False;

	if( Cmd_argc() == 1 )
	{
		printf( " \"%s\" = \"%s\"\n", var->name, var->string );
		return True;
	}

	Cvar_Set( var->name, Cmd_argv(1), False );

	return True;
}


float Cvar_GetValue (char *var_name)
{
	cvar	*var;
	
	var = Cvar_Find( var_name );
	if (!var)
		return 0;
	return (float)atof (var->string);
}

char*	Cvar_BitInfo( int bit )
{
	static char info[MAX_INFO_STRING];
	cvar*	var;

	info[0] = 0;

	for( var = cvar_list; var; var = var->next )
	{
		if( var->flags & bit )
			Info_SetValueForKey( info, var->name, var->string );
	}

	return info;
}

char*	Cvar_UserInfo( void )
{
	return Cvar_BitInfo( CVAR_USER );
}

char*	Cvar_ServerInfo( void )
{
	return Cvar_BitInfo( CVAR_SERVER );
}



/* ------------------
Commands
--------------------- */

void Cvar_fSet( void )
{
	int argc;

	argc = Cmd_argc();

	if( argc < 3 )
	{
		printf( "usage: set <cvar> <value>\n" );
	}

	Cvar_Set( Cmd_argv(1), Cmd_argv(2), False );
}

void Cvar_fCvarls( void )
{
	cvar*	var;

	for( var = cvar_list; var; var = var->next )
	{
		printf( " %s\n", var->name );
	}
}
