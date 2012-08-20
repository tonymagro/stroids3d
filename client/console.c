#include "client.h"
#include "console.h"

#if 0

console_t		con;

#define		MAXCMDLINE	256
char	key_lines[32][MAXCMDLINE];
int		key_linepos;
int		shift_down=False;
int	anykeydown;

int		edit_line=0;
int		history_line=0;

int		key_waiting;
char	*keybindings[256];
sboolean	consolekeys[256];	// if True, can't be rebound while in console
sboolean	menubound[256];	// if True, can't be rebound while in menu
int		keyshift[256];		// key to map to if shift held down in console
int		key_repeats[256];	// if > 1, it is autorepeating
sboolean	keydown[256];

/* PROTOTYPES */
void Con_DrawInput( void );

/* CVARS */
cvar*	con_trailtime;

/* KEYS.C */
#define		MAXCMDLINE	256
extern	char	key_lines[32][MAXCMDLINE];
extern	int		edit_line;
extern	int		key_linepos;

void Con_Init( void )
{
	con.linewidth = -1;

	Con_CheckResize();

	con_trailtime = Cvar_Get( "con_trailtime", "3", CVAR_SAVE );

	con.initialized = True;

	Cmd_Add( "contoggle", Con_fToggle );

	printf( "INIT: Console Initialized\n" );
}

void Con_ClearNotify( void )
{
	int		index;
	
	for( index = 0; index < NUM_CON_TIMES; index++ )
		con.times[index] = 0;
}

void Con_CheckResize( void )
{
	int	i, j;
	int width, oldwidth;
	int oldtotallines, numlines, numchars;

	char	tempbuf[CON_TEXTSIZE];

	// Total screen width divides by 2^3 = 8
	width = (vidinfo.width >> 4) - 2;

	// If the width has not changed then exit
	if( width == con.linewidth )
		return;

	if( width < 1 )
	{
		// Video is not set
		width = 38;
		con.linewidth = width;
		con.totallines = CON_TEXTSIZE / con.linewidth;
		memset( con.text, ' ', CON_TEXTSIZE );
	}
	else
	{
		oldwidth = con.linewidth;
		con.linewidth = width;

		oldtotallines = con.totallines;
		con.totallines = CON_TEXTSIZE / con.linewidth;

		numlines = oldtotallines;

		// See if new console is smaller then old console
		if( con.totallines < numlines )
			numlines = con.totallines;

		numchars = oldwidth;

		// See if new linwidth is small then old linewidth
		if( con.linewidth < numchars )
			numchars = con.linewidth;

		memcpy( tempbuf, con.text, CON_TEXTSIZE );
		memset( con.text, ' ', CON_TEXTSIZE );

		for( i = 0; i < numlines; i++ )
		{
			for( j =0; j < numchars; j++ )
			{
				con.text[(con.totallines - 1 - i) * con.linewidth + j] =
					tempbuf[((con.current - i + oldtotallines) % oldtotallines) * oldwidth + j];
			}
		}

		Con_ClearNotify();
	}

	con.current = con.totallines - 1;
	con.display = con.current;
}

void Con_DrawConsole( float consize )
{
	int		i, j, x, y, n;
	int		rows;
	char*	text;
	int		row;
	int		lines;
	//char	version[64];


	// consize is a float less then or equal to 1
	lines = vidinfo.height * consize;

	// Don't draw the console at all
	if( lines <= 0 )
		return;

	// consize was greater then 1.0
	if( lines > vidinfo.height )
		lines = vidinfo.height;
	
	//sprintf( filepath, "%s/art/congraphic.tga", Fsys_GetGameDir() );
	// Draw the background
	//Draw_StretchImage( 0, 0, vidinfo.width, lines, False, filepath );

	//FIXME: DRaw Console

	// FIXME: DRAW VERSION

	// Draw the text
	con.vislines = lines;

	#if 0
	rows = (lines-8)>>3;		// rows of text to draw

	y = lines - 24;
#else
	rows = (lines-22)>>3;		// rows of text to draw

	y = lines - 30;
#endif

// draw from the bottom up
	if( con.display != con.current )
	{
	// draw arrows to show the buffer is backscrolled
		for (x=0 ; x<con.linewidth ; x+=4)
		{
			Ren_DrawChar( (x+1)<<3, y, 1, '_' );
		}
	
		y -= 8;
		rows--;
	}
	
	row = con.display;
	for (i=0 ; i<rows ; i++, y-=8, row--)
	{
		if (row < 0)
			break;
		if (con.current - row >= con.totallines)
			break;		// past scrollback wrap point
			
		text = con.text + (row % con.totallines)*con.linewidth;

		for (x=0 ; x<con.linewidth ; x++)
		{
			//Draw_Char ( (x+1)<<3, y, text[x] ^ 0x80 );
			Ren_DrawChar( (x+1)<<3, y, 1, text[x] );
		}
	}



	// draw the input prompt, user text, and cursor if desired
	Con_DrawInput();
}

void Con_DrawTrail( void )
{
	int		x, v;
	char	*text;
	int		i;
	int		time;
	char	*s;
	int		skip;

	v = 0;
	for (i= con.current-NUM_CON_TIMES+1 ; i<=con.current ; i++)
	{
		if (i < 0)
			continue;
		time = con.times[i % NUM_CON_TIMES];
		if (time == 0)
			continue;
		time = cls.realtime - time;
		if (time > con_trailtime->value*1000)
			continue;
		text = con.text + (i % con.totallines)*con.linewidth;
		
		for (x = 0 ; x < con.linewidth ; x++)
			Ren_DrawChar( (x+1)<<3, v, 1, text[x]);

		v += 8;
	}
}

void Con_DrawInput( void )
{
	int		y;
	int		i;
	char	*text;

	if (cls.key_dest == key_menu)
		return;
	if (cls.key_dest != key_console )
		return;		// don't draw anything (always draw if not active)

	text = key_lines[edit_line];
	
// add the cursor frame
	text[key_linepos] = 10+((int)(cls.realtime>>8)&1);
	
// fill out remainder with spaces
	for (i=key_linepos+1 ; i< con.linewidth ; i++)
		text[i] = ' ';
		
//	prestep if horizontally scrolling
	if (key_linepos >= con.linewidth)
		text += 1 + key_linepos - con.linewidth;
		
// draw it
	y = con.vislines-16;

	for (i=0 ; i<con.linewidth ; i++)
		Ren_DrawChar( (i+1)<<3, con.vislines - 22, 1, text[i]);

// remove cursor
	key_lines[edit_line][key_linepos] = 0;
}

void Con_Linefeed (void)
{
	con.x = 0;
	if (con.display == con.current)
		con.display++;
	con.current++;
	memset (&con.text[(con.current%con.totallines)*con.linewidth]
	, ' ', con.linewidth);
}

void Key_ClearTyping (void)
{
	key_lines[edit_line][1] = 0;	// clear any typing
	key_linepos = 1;
}

void Con_Print (char *txt)
{
	int		y;
	int		c, l;
	static int	cr;
	int		mask;

	if (!con.initialized)
		return;

	if (txt[0] == 1 || txt[0] == 2)
	{
		mask = 128;		// go to colored text
		txt++;
	}
	else
		mask = 0;


	while ( (c = *txt) )
	{
	// count word length
		for (l=0 ; l< con.linewidth ; l++)
			if ( txt[l] <= ' ')
				break;

	// word wrap
		if (l != con.linewidth && (con.x + l > con.linewidth) )
			con.x = 0;

		txt++;

		if (cr)
		{
			con.current--;
			cr = False;
		}

		
		if (!con.x)
		{
			Con_Linefeed ();
		// mark time for transparent overlay
			if (con.current >= 0)
				con.times[con.current % NUM_CON_TIMES] = cls.realtime;
		}

		switch (c)
		{
		case '\n':
			con.x = 0;
			break;

		case '\r':
			con.x = 0;
			cr = 1;
			break;

		default:	// display character and advance
			y = con.current % con.totallines;
			con.text[y*con.linewidth+con.x] = c | mask | con.ormask;
			con.x++;
			if (con.x >= con.linewidth)
				con.x = 0;
			break;
		}
		
	}
}

void Con_fToggle( void )
{
	//SCR_EndLoadingPlaque ();	// get rid of loading plaque

	/*
	if (cl.attractloop)
	{
		Cbuf_AddText ("killserver\n");
		return;
	}
	*/

	/*
	if (cls.state == ca_disconnected)
	{	// start the demo loop again
		Cbuf_AddText ("d1\n");
		return;
	}
	*/

	Key_ClearTyping ();
	Con_ClearNotify ();

	if(cls.key_dest == key_console )
	{
		//M_ForceMenuOff ();
		Cvar_Set( "paused", "0", True );
		//IN_Active( True );
		//printf( "Turned off Console\n" );
		cls.key_dest = key_game;
	}
	else
	{
		//M_ForceMenuOff ();
		cls.key_dest = key_console;
		//IN_Active( False );

		/*
		if( Cvar_VariableValue ("maxclients") == 1 
			&& Com_ServerState ())
			Cvar_Set ("paused", "1");
		*/
	}
}

#endif
