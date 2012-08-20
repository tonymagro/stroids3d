#include "game.h"

/* ------------------------------
	HUD
---------------------------------*/
void Hud_Init( void )
{
}

void Hud_Move( void )
{
}

void Hud_Draw( void )
{
	int i;
	float x;
	static float counter = 0.0f;
	static float increment = 0.1f;

	/* Make Things blink */
	if( (counter > 1.0f) || (counter < 0.0f))
			increment *= -1;
	counter += (increment);
	
	if( playerinfo.health < playerinfo.maxHealth*0.2 )
		Ren_Color( 1.0f, 0.0f, 0.0f, counter );
	Ren_printf( 0.09f, 0.13f, 0.4f,"Health: %2i", playerinfo.health );

	if( playerinfo.health < playerinfo.maxHealth*0.5 )
		Ren_Color( 1.0f, 1.0f, 0.0f, 1.0f );
	else
	Ren_Color( 0.0f, 1.0f, 0.1f, 0.5f );

	Ren_DrawImage( 8.0f, 0.5f, 1.0f, 1.3f, pic_hudship );

	if( playerinfo.health < playerinfo.maxHealth*0.5 )
	{
		Ren_Color( 1.0f, 0.0f, 0.0f, 1.0f );
		Ren_printf( 8.7f, 1.3f, 0.4f, "Fuel" );
		
		Ren_Color( 1.0f, 0.0f, 0.0f, counter );
		Ren_printf( 7.9f, 0.2f, 0.4f, "Fuel Leak" );
	}

	Ren_Color( 1.0f, 1.0f, 1.0f, 1.0f );
	for( i = 0, x = 0.2f; i < playerinfo.lives; i++ )
	{
		Ren_DrawImage( x, 0.7f, 0.5f, 0.5f, pic_ship );
		x += 0.5;
	}

	Ren_printf( 7.8f, 2.0f, 0.3f, "Ship Status" );
	Ren_printf( 0.1f, 9.7f, 0.4f, "Level: %i", playerinfo.level );
	Ren_printf( 0.1f, 0.4f, 0.4f, "Score: %2i", playerinfo.score );
	Ren_printf( 4.0f, 0.06f,0.4f, "Energy Supply");

	/* Draw The Energy Bar */
	Ren_BindImage( pic_block->textureid );
	Ren_Color( 1.0f, 0.0f, 0.0f, 0.6f );
	Ren_DrawRectangle( 4.0f, 0.26f, 2.0f, 0.4f );
	Ren_Color( 0.0f, 1.0f, 0.0f, 0.6f );
	Ren_DrawRectangle( 4.0f, 0.26f, (2.0f*playerinfo.energy), 0.4f );

	Ren_printf( 2.9f, 9.2f, 0.4f, "Stroids Remaining: %i", playerinfo.stroids_left );

	Ren_Color( 1.0f, 1.0f, 0.0f, 0.8f );
	Ren_printf( 0.1f, 8.0f, 0.4f, "Weapon: %s", ((playerinfo.weapon_selection == 0) ? ("Plasma") : ("Energy Bomb")) ); 


	/* Draw the Reticle */
	Ren_Color( 1.0f, 0.0f, 0.0f, 0.5f );

	if( playerinfo.cameratype == 0 )
		Ren_DrawImage( 4.8f, 4.7f, 0.5f, 0.63f, pic_reticle ); 
}
/* ------------------------------
	HUD
---------------------------------*/
