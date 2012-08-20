//#include "../game/game.h"
#include "../scommon/s_common.h"
#include "../scommon/keys.h"
#include "s_sdl.h"
#include "../game/game.h"

/* This is our SDL surface */
SDL_Surface *surface;

vid_info_t vidinfo;

vid_mode_t vidmode[VID_MODES] = {
	{ "640x480x32", 640, 480, 32 },
	{ "800x600x32", 800, 600, 32 },
	{ "1024x768x32", 1024, 768, 32 },
	{ "1280x1024x32", 1280, 1024, 32 },
	{ "1600x1200x32", 1600, 1200, 32 },
};

sboolean window_created;

sboolean OS_CreateWindow( int width, int height, int bpp, sboolean fullscreen );


/* function to release/destroy our resources and restoring the old desktop */
void OS_Quit( int returnCode )
{
	Main_Shutdown();

    /* clean up the window */
    SDL_Quit();

    exit( returnCode );
}

/* This is for fatal errors */
void OS_Error( int type, char * message )
{

	printf( "FATAL ERROR: %s\n", message );

	OS_Quit( type );
}

int curtime;
int OS_Milliseconds( void )
{
	curtime = SDL_GetTicks();
	return curtime;
}

void OS_SwapBuffers( void )
{
	SDL_GL_SwapBuffers( );
}

void OS_ShowCursor( sboolean show )
{
	SDL_ShowCursor(show);

	if( show )
		SDL_WM_GrabInput(SDL_GRAB_OFF);
	else
		SDL_WM_GrabInput(SDL_GRAB_ON);
}


static sboolean cursor_released = False;
void OS_ToggleCursor( void )
{
	cursor_released ? (cursor_released = False) : (cursor_released = True);

	cursor_released ? OS_ShowCursor( True ) : OS_ShowCursor( False );
	
}

void OS_GetMousePosition( int *x, int *y )
{
	SDL_GetMouseState( x, y );
}

/*
 * Handle game input
 */
SDLMod modkeys;
void OS_HandleKeyEvent( SDL_KeyboardEvent *key )
{
	sboolean down;

	modkeys = SDL_GetModState();

	/* FIXME: HACK FOR ':' to work */
	if(modkeys & KMOD_SHIFT)
		if( key->keysym.sym == ';' )
			key->keysym.sym = ':';

	(key->type == SDL_KEYDOWN) ? (down = True) : (down = False);

	switch( key->keysym.sym )
	{
	case SDLK_LSHIFT:
	case SDLK_RSHIFT:
		G_KeyEvent( K_SHIFT, down );
		break;
	case SDLK_KP_PLUS:
		G_KeyEvent( K_KP_PLUS, down );
		break;
	case SDLK_KP_MINUS:
		G_KeyEvent( K_KP_MINUS, down );
		break;
	case SDLK_BACKSPACE:
		G_KeyEvent( K_BACKSPACE, down );
		break;
	case SDLK_ESCAPE:
		G_KeyEvent( K_ESCAPE, down );
		break;
	case SDLK_RETURN:
		G_KeyEvent( K_ENTER, down );
		break;
	case SDLK_KP1:
		G_KeyEvent( K_KP_END, down );
		break;
	case SDLK_KP2:
		G_KeyEvent( K_KP_DOWNARROW, down );
		break;
	case SDLK_KP3:
		G_KeyEvent( K_KP_PGDN, down );
		break;
	case SDLK_KP9:
		if( down )
			OS_Quit( 0 );
		G_KeyEvent( K_KP_PGUP, down );
	case SDLK_g:
		if( down )
			OS_ToggleCursor();
	default:
		G_KeyEvent( key->keysym.sym, down );
	}
}


void OS_HandleMouseButton( SDL_MouseButtonEvent *event )
{
	sboolean down;

	(event->state == SDL_PRESSED) ? (down = True) : (down = False);

	switch( event->button )
	{
	case SDL_BUTTON_LEFT:
		G_KeyEvent( K_MOUSE1, down );
		break;
	case SDL_BUTTON_MIDDLE:
		G_KeyEvent( K_MOUSE3, down );
		break;
	case SDL_BUTTON_RIGHT:
		G_KeyEvent( K_MOUSE2, down );
		break;
	case SDL_BUTTON_WHEELUP:
		G_KeyEvent( K_MWHEELUP, down );
		break;
	case SDL_BUTTON_WHEELDOWN:
		G_KeyEvent( K_MWHEELDOWN, down );
		break;
	default:
		break;
	}
}

void OS_HandleJoyMotion( SDL_JoyAxisEvent *jaxis )
{
	/* If axis has left the dead zone */
	if( ( jaxis->value < -3200 ) || (jaxis->value > 3200 ) ) 
    {
		//keystate.axis = jaxis->axis;
		//keystate.value = jaxis->value;

		//if( jaxis->axis = 1 )
		//{
		//	if( jaxis->value < 0 )
		//		keystate.mouse_cy -= jaxis->value;
		//	else
		//		keystate.mouse_cy += jaxis->value;
		//}
    }

}

void OS_HandleJoyButton( SDL_JoyButtonEvent *jbutton )
{
	sboolean down;

	(jbutton->state == SDL_PRESSED) ? (down = True) : (down = False);
	
	switch( jbutton->button )
	{
	case 0:
		G_KeyEvent( K_JOY1, down );
		break;
	case 1:
		G_KeyEvent( K_JOY2, down );
		break;
	case 2:
		G_KeyEvent( K_JOY3, down );
		break;
	case 3:
		G_KeyEvent( K_JOY4, down );
		break;
	case 4:
		G_KeyEvent( K_AUX1, down );
		break;
	case 5:
		G_KeyEvent( K_AUX2, down );
		break;
	case 6:
		G_KeyEvent( K_AUX3, down );
		break;
	case 7:
		G_KeyEvent( K_AUX4, down );
		break;
	case 8:
		G_KeyEvent( K_AUX5, down );
		break;
	case 9:
		G_KeyEvent( K_AUX6, down );
		break;
	default:
		break;
	}
}

unsigned char *keystatearray;
SDLMod modkeys;

/*
void OS_UpdateKeyBoard( void )
{
	keystatearray = SDL_GetKeyState(NULL);
	modkeys = SDL_GetModState();

	if( keystatearray[SDLK_BACKSPACE] )
		OS_Quit(0);

	if( keystatearray[SDLK_RETURN] && (modkeys & KMOD_ALT) ) {
		vidinfo.fullscreen ? (vidinfo.fullscreen = 0) : (vidinfo.fullscreen = 1);
		OS_CreateWindow( vidinfo.width, vidinfo.height, vidinfo.bpp, vidinfo.fullscreen );
		Ren_ReloadTextures();
		//SDL_WM_ToggleFullScreen( surface );
	}

	if( keystatearray[SDLK_g] )
		OS_ToggleCursor();

	keystate.escape		= keystatearray[SDLK_ESCAPE];
	keystate.jetting	= keystatearray[SDLK_w];
	keystate.rotleft	= keystatearray[SDLK_a];
	keystate.rotright	= keystatearray[SDLK_d];
	keystate.shield		= keystatearray[SDLK_z];
	keystate.fire		= keystatearray[SDLK_SPACE];
	keystate.bombfire	= keystatearray[SDLK_b];
	keystate.enter		= keystatearray[SDLK_RETURN];
	keystate.kp1		= keystatearray[SDLK_KP1];
	keystate.kp2		= keystatearray[SDLK_KP2];
	keystate.kp3		= keystatearray[SDLK_KP3];
	keystate.up			= keystatearray[SDLK_UP];
	keystate.down		= keystatearray[SDLK_DOWN];
	keystate.left		= keystatearray[SDLK_LEFT];
	keystate.right		= keystatearray[SDLK_RIGHT];
	keystate.thirdperson= keystatearray[SDLK_c];
	keystate.backward	= keystatearray[SDLK_s];
}
*/


sboolean OS_CreateWindow( int width, int height, int bpp, sboolean fullscreen )
{
	// Video Init Flags
	int videoFlags;
    /* this holds some info about our display */
    const SDL_VideoInfo *videoInfo;

	if( window_created )
	{
		//SDL_QuitSubSystem( SDL_INIT_VIDEO );
		//SDL_FreeSurface( surface );
		//surface = NULL;
		//SDL_InitSubSystem( SDL_INIT_VIDEO );
	}


    /* Fetch the video info */
    videoInfo = SDL_GetVideoInfo( );

    if ( !videoInfo )
    {
        fprintf( stderr, "Video query failed: %s\n",
		     SDL_GetError( ) );
        OS_Error( 1, va("Video query failed: %s\n",
		     SDL_GetError()) );
    }

    /* the flags to pass to SDL_SetVideoMode */
    videoFlags  = SDL_OPENGL;          /* Enable OpenGL in SDL */
    videoFlags |= SDL_GL_DOUBLEBUFFER; /* Enable double buffering */
    videoFlags |= SDL_HWPALETTE;       /* Store the palette in hardware */
    //videoFlags |= SDL_RESIZABLE;       /* Enable window resizing */

    if( fullscreen )
        videoFlags |= SDL_FULLSCREEN;

    /* This checks to see if surfaces can be stored in memory */
    if ( videoInfo->hw_available )
	videoFlags |= SDL_HWSURFACE;
    else
	videoFlags |= SDL_SWSURFACE;

    /* This checks if hardware blits can be done */
    if ( videoInfo->blit_hw )
	videoFlags |= SDL_HWACCEL;

    /* Sets up OpenGL double buffering */
    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

    /* get a SDL surface */
    surface = SDL_SetVideoMode( width, height, bpp, videoFlags );

    /* Verify there is a surface */
    if ( !surface )
	{
	    fprintf( stderr,  "Video mode set failed: %s\n", SDL_GetError( ) );
		if( window_created )
			return False;
		else
			OS_Error( 1 , va("Video mode set failed: %s\n", SDL_GetError()));
	}
	
	
	Ren_WindowSize( width, height );

    window_created = True;

    return True;

}

void OS_SetVidMode( int mode, sboolean fullscreen )
{
	sboolean result;
	result = OS_CreateWindow( vidmode[mode].width, vidmode[mode].height, vidmode[mode].bpp, fullscreen );

	/* If success Set new window resolution */
	if( result )
	{
		vidinfo.width	= vidmode[mode].width;
		vidinfo.height	= vidmode[mode].height;
		vidinfo.bpp		= vidmode[mode].bpp;
		vidinfo.fullscreen = fullscreen;
		vidinfo.mode = mode;
	}
	else	/* Failed so go back to what was working */
	{
		OS_CreateWindow( vidinfo.width, vidinfo.height, vidinfo.bpp, vidinfo.fullscreen );
	}

	
	if( window_created )
            Ren_ReloadTextures();
			
}


/* Main Entry into program */
int main( int argc, char **argv )
{
	int				time, oldtime, newtime;

	/* whether or not the window is active */
    int isActive = True;
	/* main loop variable */
    int done = False;
	/* used to collect events */
    SDL_Event event;
	/* Flags to pass to SDL_SetVideoMode */
	int initFlags;

	/* Timing */
	int thisframe = 0;
	static int lastframe = 0;

	/* check endian */
    if( Swap_SysBigEndian() )
		printf( "System Byte Order: Big Endian\n" );
    else
		printf( "System Byte Order: Little Endian\n" );

	

	window_created = False;

	vidinfo.height		=  DEFAULT_VIEWPORT_WIDTH;
	vidinfo.width		=  DEFAULT_VIEWPORT_HEIGHT;
	vidinfo.bpp			=  DEFAULT_VIEWPORT_BPP;

	
	vidinfo.fullscreen	=  DEFAULT_VIEWPORT_FULLSCREEN;
	vidinfo.mode		=  DEFAULT_VIDMODE;

	initFlags = SDL_INIT_VIDEO;

	if( JOYSTICK_SUPPORT )
		initFlags |= SDL_INIT_JOYSTICK;

    /* initialize SDL */
    if ( SDL_Init( initFlags ) < 0 )
	{
	    fprintf( stderr, "Video initialization failed: %s\n",
		     SDL_GetError( ) );
	    OS_Error( 1, va("Video initialization failed: %s\n",
		     SDL_GetError()) );
	}

	OS_SetVidMode( vidinfo.mode, vidinfo.fullscreen );

	/* Name our Window */
	SDL_WM_SetCaption( S_VERSION, 0 );

		/* set up keyboard delay */
	//SDL_EnableKeyRepeat( 0, 0 );


	/* Hide cursor and grab input */
	SDL_ShowCursor(0);
	SDL_WM_GrabInput(SDL_GRAB_ON);
	SDL_WarpMouse( (unsigned short)(vidinfo.width/2), (unsigned short)(vidinfo.height/2) );
	
	Main_Init();

	oldtime = OS_Milliseconds ();

    /* wait for events */
    while ( !done )
	{
		thisframe = OS_Milliseconds();

	    /* handle the events in the queue */
	    while ( SDL_PollEvent( &event ) )
		{
		    switch( event.type )
			{
			case SDL_ACTIVEEVENT:
			    /* Something's happend with our focus
			     * If we lost focus or we are iconified, we
			     * shouldn't draw the screen
			     */
/*			    if ( event.active.gain == 0 )
				isActive = FALSE;
			    else
				isActive = True;
*/			    break;
				/*
			case SDL_VIDEORESIZE:
			    // handle resize event 
			    surface = SDL_SetVideoMode( event.resize.w,
							event.resize.h,
							VIEWPORT_BPP, videoFlags );
			    if ( !surface )
				{
				    fprintf( stderr, "Could not get a surface after resize: %s\n", SDL_GetError( ) );
					G_Shutdown();
				    OS_Quit( 1 );
				}
			    Ren_WindowSize( event.resize.w, event.resize.h );
			    break;
				*/
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				OS_HandleKeyEvent( &event.key );
				break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				OS_HandleMouseButton( &event.button );
				break;
		//	case SDL_JOYAXISMOTION:
		//		OS_HandleJoyMotion( &event.jaxis );
		//		break;
			case SDL_JOYBUTTONDOWN:
			case SDL_JOYBUTTONUP:
				OS_HandleJoyButton( &event.jbutton );
				break;
			case SDL_QUIT:
			    /* handle quit requests */
			    done = True;
			    break;
			default:
			    break;
			}
		}

		do
		{
			newtime = OS_Milliseconds ();
			time = newtime - oldtime;
		} while (time < 1);

		Main_Frame(time);
		
	}

    /* clean ourselves up and exit */
    OS_Quit( 0 );

    /* Should never get here */
    return( 0 );

}
