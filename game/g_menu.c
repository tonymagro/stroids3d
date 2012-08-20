#include "game.h"

#define MAX_MENUNAME 32

typedef float rect_t[4];

image_t * pic_backdrop;
image_t * pic_cursor;
image_t * pic_title;

typedef struct menu_item_s
{
	char name[MAX_MENUNAME];
	void (*function) ( void );
	rect_t rect;
	int id;
} menu_item_t;

typedef struct input_item_s
{
	char input[128];
	int	 index;
	void (*function) ( void );
	rect_t rect;
} input_item_t;

int Menu_GetInput( char *input );

void menu_newgame_f( void );
void menu_restart_f( void );
void menu_options_f( void );
void menu_exit_f( void );
void menu_exitm_f( void );
void menu_controls_f( void );
void menu_netgame_f( void );

void net_host_f( void );
void net_join_f( void );


void menu_video_f( void );
void menu_audio_f( void );
void menu_exitop_f( void );

menu_item_t apply  = { "", NULL, {5.0f, 4.0f, 2.4f, 0.5f}, 0};
menu_item_t ok  = { "", NULL, {3.0f, 4.0f, 2.4f, 0.5f}, 0};

menu_item_t main_menu[] = { 
	{"Single Player",	menu_newgame_f, 0, 0},
	{"Network Game", menu_netgame_f, 0, 0 },
	{"Restart",		menu_restart_f, 0, 1},
	{"Options",		menu_options_f, 0, 2},
	{"Controls",	menu_controls_f, 0, 2},
	{"_SPACE_",		NULL,			0, 3},
	{"Exit Menu",	menu_exitm_f,	0, 3},
	{"Exit Game",	menu_exit_f,	0, 4},
	{ "",			NULL,			0, 5}
};
menu_item_t options_menu[] = { 
	{"Video",		menu_video_f,	0, 0},
	{"Audio",		menu_audio_f,	0, 1},
	{"_SPACE_",		NULL,			0, 2},
	{"Exit Menu",	menu_exitop_f,	0, 3},
	{ "",			NULL,			0, 5}
};
menu_item_t controls_menu[] = { 
	{ "",			NULL,			0, 5}
};
menu_item_t video_options_menu[] = {
	{"",			NULL,			0, 5}
};
menu_item_t ip_input_menu[] = {
	{"",			NULL,			0, 5}
};
menu_item_t net_menu[] = {
	{"Host Game", net_host_f, 0, 0 },
	{"Join Game", net_join_f, 0, 0 },
	{"_SPACE_",		NULL,	0, 2},
	{"Exit Menu",	menu_exitop_f,	0, 3},
	{"",			NULL, 0, 0 }
};
int item_selected = 0;


menu_item_t *current_menu;

void Menu_CreateInput( input_item_t *input_item, int low_char, int high_char )
{
	int current_char;

	if( Menu_GetInput( &input_item->input[input_item->index] ) == -1 ){
		//input_item->input[input_item->index+1] = '\0';
		return;
	}
	
	current_char = input_item->input[input_item->index];


	if( current_char == K_BACKSPACE ) {
		
		if( --input_item->index < 0 )
			input_item->index = 0;
		
		input_item->input[input_item->index] = '\0';		
		
		return;
	}
	
	if( (current_char >= low_char) && (current_char <= high_char) ) {
		input_item->index++;
		input_item->input[input_item->index] = '\0';
	}
}

void Menu_MouseToScreen( float*x, float*y )
{
	int mx, my;
	float cx, cy;

	OS_GetMousePosition( &mx, &my );

	my = vidinfo.height - my;

	cx = (float)mx / vidinfo.width * 10;
	cy = (float)my / vidinfo.height * 10;

	*x = cx;
	*y = cy;

}

sboolean Menu_MouseInItem( menu_item_t *menu )
{
	int mx, my;
	float cx, cy;
	float rectright, recttop;

	OS_GetMousePosition( &mx, &my );

	my = vidinfo.height - my;

	cx = (float)mx / vidinfo.width * 10;
	cy = (float)my / vidinfo.height * 10;

	rectright = menu->rect[0] + menu->rect[2];
	recttop	  = menu->rect[1] + menu->rect[3];

	if( (cx > menu->rect[0]) && (cx < rectright) && (cy > menu->rect[1]) && (cy < recttop) )
		return True;
	else
		return False;
}

void Menu_PrintControlInfo( void )
{
	Ren_Color( 1.0f, 1.0f, 1.0f, 1.0f );
	Ren_printf( 1, 5, 1, "HIT K_KP_9" );
}

input_item_t ip_box = { "127.0.0.1:5050", 0, NULL, {5.0f, 6.0f, 2.4f, 0.5f} };
menu_item_t ip_input_box = { "", NULL, {5.5f, 7.0f, 2.4f, 0.5f}, 0};
void ip_input_run( void )
{
	sboolean mouseover_ip = False;
	sboolean mouseover_app = False;
	sboolean mouseover_ok  = False;

	if( Menu_MouseInItem( &ip_input_box ) )
		mouseover_ip = True;

	if( Menu_MouseInItem( &apply ) )
		mouseover_app = True;

	if( Menu_MouseInItem( &ok ) )
		mouseover_ok = True;

	if( G_KeyDown( K_MOUSE1 ) )
	{	
		if( mouseover_app ){
			//OS_SetVidMode( selected_mode, True );
		}
		else if( mouseover_ok ) {
#if NETWORK_SUPPORT
			G_JoinGame( ip_box.input );
			menu_newgame_f();
			current_menu = &main_menu[0];
#endif

		}
	}

	if( G_KeyDown( K_ENTER ) ) {
#if NETWORK_SUPPORT
		G_JoinGame( ip_box.input );
		menu_newgame_f();
		current_menu = &main_menu[0];
#endif
	}

	/* valid input '!' through 'z' in ascii table */
	Menu_CreateInput( &ip_box, 21, 122 );

}
void ip_input_draw( void )
{
	Ren_Color( 1.0f, 1.0f, 1.0f, 1.0f );
	Ren_printf( 2.6f, 6.0f, 0.4f, "IP Address: " );
	Ren_printf( 4.3f, 6.0f, 0.4f, "%s", ip_box.input );

	if( Menu_MouseInItem( &apply ) )
		Ren_Color( 1.0f, 0.0f, 0.0f, 1.0f );
		
	Ren_printf( apply.rect[0]+0.3f, apply.rect[1], 0.6f, "Apply" );

	Ren_Color( 1.0f, 1.0f, 1.0f, 1.0f );

	if( Menu_MouseInItem( &ok ) )
		Ren_Color( 1.0f, 0.0f, 0.0f, 1.0f );

	Ren_printf( ok.rect[0]+0.5f, ok.rect[1], 0.6f, "OK" );
}


int selected_mode = DEFAULT_VIDMODE;
menu_item_t resbox = { "", NULL, {5.5f, 7.0f, 2.4f, 0.5f}, 0};
void video_options_run( void )
{
	sboolean mouseover_res = False;
	sboolean mouseover_app = False;
	sboolean mouseover_ok  = False;

	if( Menu_MouseInItem( &resbox ) )
		mouseover_res = True;

	if( Menu_MouseInItem( &apply ) )
		mouseover_app = True;

	if( Menu_MouseInItem( &ok ) )
		mouseover_ok = True;

	if( G_KeyDown( K_ENTER ) || G_KeyDown( K_MOUSE1 ) )
	{	
		if( mouseover_res )
			selected_mode++;
		else if( mouseover_app )
			OS_SetVidMode( selected_mode, True );
		else if( mouseover_ok ) {
			if( selected_mode != vidinfo.mode )
				OS_SetVidMode( selected_mode, True );
			current_menu = &main_menu[0];
		}
	}

	if( selected_mode >= VID_MODES )
		selected_mode = 0;
}
void video_options_draw( void )
{
	Ren_Color( 1.0f, 1.0f, 1.0f, 1.0f );
	Ren_printf( 2.5f, 7.0f, 0.6f, "Resolution: " );
	Ren_printf( 5.7f, 7.0f, 0.6f, "%s", vidmode[selected_mode].name );

	if( Menu_MouseInItem( &apply ) )
		Ren_Color( 1.0f, 0.0f, 0.0f, 1.0f );
		
	Ren_printf( apply.rect[0]+0.3f, apply.rect[1], 0.6f, "Apply" );

	Ren_Color( 1.0f, 1.0f, 1.0f, 1.0f );

	if( Menu_MouseInItem( &ok ) )
		Ren_Color( 1.0f, 0.0f, 0.0f, 1.0f );

	Ren_printf( ok.rect[0]+0.5f, ok.rect[1], 0.6f, "OK" );
}


void G_InitMenu( void )
{
	float y, x, w, h;
	menu_item_t *menu;

	printf( "Initializing Menu\n" );

	x = 3.0f;
	w = 2.4f;
	h = 0.5f;

	/* Create Main Menu */
	for( y = 5.5f, menu = &main_menu[0]; menu->name[0]; y -= 0.5f, menu++ )
	{
		menu->rect[0] = x;
		menu->rect[1] = y;
		menu->rect[2] = w;
		menu->rect[3] = h;
	}

	/* Create Controls Menu */
	for( y = 5.5f, menu = &options_menu[0]; menu->name[0]; y -= 0.5f, menu++ )
	{
		menu->rect[0] = x;
		menu->rect[1] = y;
		menu->rect[2] = w;
		menu->rect[3] = h;
	}

	/* Create Network Menu */
	for( y = 5.5f, menu = &net_menu[0]; menu->name[0]; y -= 0.5f, menu++ )
	{
		menu->rect[0] = x;
		menu->rect[1] = y;
		menu->rect[2] = w;
		menu->rect[3] = h;
	}


	/* Create Video Menu */
	video_options_menu[0].rect[0] = 3.5f;
	video_options_menu[0].rect[1] = 3.5f;
	video_options_menu[0].rect[2] = w;
	video_options_menu[0].rect[3] = h;

	current_menu = &main_menu[0];

	pic_backdrop = Ren_LoadImage( "data/menuback.tga" );
	pic_cursor   = Ren_LoadImage( "data/cursor.tga" );
	pic_title	 = Ren_LoadImage( "data/title.tga" );
}

vec3_t angles = { 0, 0, 0 };
void G_RunMenu( void )
{
	int i = 0;
	menu_item_t *menu;
	sboolean mouseover = False;


	angles[YAW] += 10.0f*gi.framelen;
	angles[PITCH] += 10.0f*gi.framelen;
	
	Ren_CameraSetAngles( angles );

	menu = current_menu;

	if( menu == &video_options_menu[0] )
		video_options_run();
	else if( menu == &ip_input_menu[0] )
		ip_input_run();
	else {

		for( i = 0, menu = current_menu; menu->name[0]; menu++, i++ )
		{
			if( Menu_MouseInItem( menu ) ) {
				item_selected = i;
				mouseover = True;
			}
		}

		//if( mouseover == False )
		//	item_selected = -1;

		menu = current_menu;

		if( (G_KeyDown( K_ENTER ) || G_KeyDown( K_MOUSE1 )) && (item_selected != -1))
		{
			menu += item_selected;
			if( menu->function )
				menu->function();
		}
	}

	
	if( G_KeyDown( K_UPARROW ) )
		item_selected -= 1;

	if( G_KeyDown( K_DOWNARROW ) )
		item_selected += 1;

	if( item_selected < 0 )
		item_selected = i;
	
	if( item_selected > i  )
		item_selected = 0;
	
}

void G_DrawMenu( void )
{
	int i;
	float x;
	menu_item_t *menu;

	float mx, my;

	x = 3.5f;

	menu = current_menu;

	Ren_SetGL2D();

	if( menu == &controls_menu[0] ) {
		Menu_PrintControlInfo();
	} else if( menu == &video_options_menu[0] ) {
		video_options_draw();
	} else if( menu == &ip_input_menu[0] ) {
		ip_input_draw();
	}
	else {
		Ren_Color( 0.0f, 0.0f, 0.0f, 0.5f );
		//Ren_DrawRectangle(2.0f, 1.0f, 5.5f, 6.4f);
		//Ren_DrawImage( 2.0f, 1.0f, 5.5f, 6.4f, pic_backdrop );

		Ren_Color( 0.0f, 0.0f, 0.8f, 1.0f );
		Ren_printf( x-0.3f, 6.6f, 0.8f, "MAIN MENU" );

		for( i = 0, menu = current_menu; menu->name[0]; menu++, i++ )
		{
			if( strcmp( menu->name, "_SPACE_" ) == 0)
				continue;

			if( (i != item_selected) || (i == -1)) {
				Ren_Color( 0.8f, 0.0f, 0.0f, 1.0f );
				Ren_printf( menu->rect[0]+0.2f, menu->rect[1]+0.1f, 0.6f, menu->name );
			}
			else {
				Ren_Color( 0.0f, 0.0f, 0.8f, 0.2f );
				//Ren_BindImage( pic_block->textureid );
				//Ren_DrawRectangle( menu->rect[0], menu->rect[1], menu->rect[2], menu->rect[3] );
				Ren_Color( 1.0f, 0.0f, 0.0f, 1.0f );
				Ren_printf( menu->rect[0]+0.2f, menu->rect[1]+0.1f, 0.6f, menu->name );
			}
		}
	}

	Ren_Color( 1.0f, 1.0f, 1.0f, 1.0f );
	Ren_DrawImage( 1.5f, 7.0f, 7.0f, 2.8f, pic_title );

	/* Draw the Mouse */
	Menu_MouseToScreen( &mx, &my );
	Ren_BindImage( pic_cursor->textureid );
	Ren_Color( 1.0f, 1.0f, 1.0f, 1.0f );
	Ren_DrawRectangle( mx, my-0.63f, 0.5f, 0.63f );
}

void menu_newgame_f( void )
{
	G_Restart();
	G_LoadLevel( 1 );
	gi.gamestate = GS_RUNNING;
	gi.fadein = True;
}

void menu_restart_f( void )
{
	G_Restart();
	G_LoadLevel( 1 );
	gi.gamestate = GS_RUNNING;
	gi.fadein = True;
}

void menu_options_f( void )
{
	current_menu = &options_menu[0];
}

void menu_exitm_f( void )
{
	gi.gamestate = gi.lastgamestate;
}

void menu_exit_f( void )
{
	OS_Quit(0);
}

void menu_controls_f( void )
{
	current_menu = &controls_menu[0];
}

void menu_video_f( void )
{
	current_menu = &video_options_menu[0];
}
void menu_audio_f( void )
{
}
void menu_exitop_f( void )
{
	current_menu = &main_menu[0];
}

void menu_netgame_f( void )
{
	current_menu = &net_menu[0];
}

void net_join_f( void )
{
#if NETWORK_SUPPORT
	current_menu = &ip_input_menu[0];
	//G_JoinGame();
	//menu_newgame_f();
#endif
}
void net_host_f( void )
{
#if NETWORK_SUPPORT
	G_HostGame();
	menu_newgame_f();
#endif
}


//-------------------------------------------------

int character;
int size = 0;
int got_input = False;
void Menu_KeyDown( int key, sboolean down )
{	
	if( key == K_SHIFT ) {
		got_input = False;
		return;
	}
	
	if( down ) {
		//printf( "Key Down: %c\n", key );
		character = key;
		got_input = True;
	}
	else {
		got_input = False;
	}
}

int Menu_GetInput( char *input )
{
	if( !got_input )
		return -1;
	
	*input = (char)character;
	character = '\0';

	got_input = False;
	
	return 0;
}
