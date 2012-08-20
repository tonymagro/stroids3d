#include "game.h"

float fade;
sboolean endintro = False;
image_t *pic_intro;
model_t *shipmodel;

extern image_t *pic_title;
extern image_t *pic_block;
//extern image_t *pic_backdrop;
vec3_t rotation;

void Intro_KeyDown( int key, sboolean down )
{
	if( down == True )
		endintro = True;
}

void G_InitIntro( void )
{
	fade = 1.0f;

	shipmodel = Ren_LoadModel( "data/models/ship.ms3d" );
	
	rotation[ROLL] = -45;
	rotation[YAW] = 25;
	rotation[PITCH] = 0;

	//pic_intro = Ren_LoadImage( "data/intro.tga" );
}

void G_EndIntro( void )
{
  
}

void G_RunIntro( void )
{
	static int lastframe;
	int thisframe;
	float fadetime = 50;

	if( endintro )
  {
    fade -= 0.01f;

		if( fade < 0 ) {
			fade = 1.0f;
			endintro = False;
			gi.fadein = True;
			gi.gamestate = GS_MENU;
		}
  }
	
	thisframe = OS_Milliseconds();

	if( (thisframe - lastframe) > fadetime )
	{
		lastframe = thisframe;
		fade -= 0.01f;
	
      if( fade < 0 ) {
				fade = 1.0f;
				endintro = False;
				gi.fadein = True;
				gi.gamestate = GS_MENU;
			}
	}

}

void G_DrawIntro( void )
{
	Ren_Color( 0.0f, 0.0f, 0.0f, 1.0f );
	Ren_DrawImage( 0, 0, 10, 10, pic_block );
	Ren_Color( 1.0f, 1.0f, 1.0f, 1.0f );
	//Ren_DrawImage( 0, 0, 10, 10, pic_intro );
	Ren_DrawImage( 1.5f, 5.0f, 7.0f, 2.8f, pic_title );
	//Ren_Draw2DModel( 5.0f, 5.0f, rotation, shipmodel->modelid );
}
