#define	NUM_CON_TIMES 4

#define		CON_TEXTSIZE	32768
typedef struct console_s
{
	int		initialized;

	char	text[CON_TEXTSIZE];
	int		current;		// line where next message will be printed
	int		x;				// offset in current line for next print
	int		display;		// bottom of console displays this line

	int		ormask;			// high bit mask for colored characters

	int 	linewidth;		// characters across screen
	int		totallines;		// total lines in console scrollback

	float	cursorspeed;

	int		vislines;

	float	times[NUM_CON_TIMES];	// cls.realtime time the line was generated
								// for transparent notify lines
} console_t;

extern	console_t	 con;

void	Con_Init( void );
void	Con_CheckResize( void );
void	Con_DrawConsole( float screensize );
void	Con_DrawTrail( void );
void	Con_fToggle( void );

