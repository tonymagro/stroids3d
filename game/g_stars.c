#include "game.h"

/* ------------------------------
	STARS
---------------------------------*/

void Stars_Init( void )
{
	int i;
	vec3_t origin;

	for( i = 0; i < MAX_STARS; i++ )
	{
		origin[0] = (float)RAND_NUM( WORLD_WIDTH_MIN, WORLD_WIDTH_MAX );
		origin[1] = (float)RAND_NUM( WORLD_HEIGHT_MIN, WORLD_HEIGHT_MAX );
		origin[2] = (float)RAND_NUM( WORLD_DEPTH_MIN, WORLD_DEPTH_MAX );

		/*
		origin[0] = (float)RAND_NUM( WORLD_WIDTH_MAX, WORLD_WIDTH_MAX+10 );
		origin[1] = (float)RAND_NUM( WORLD_HEIGHT_MIN, WORLD_HEIGHT_MAX );
		origin[2] = (float)RAND_NUM( WORLD_DEPTH_MAX, WORLD_DEPTH_MAX+10 );
		*/

		stars[i].type = ET_STAR;
		stars[i].counter2 = (float)(1+rand()%10)/10;	// total life
		stars[i].maxcount2 = 0.008f;	// fade value

		stars[i].alive = True;

		G_Spawn( &stars[i], origin, 0.9f, (float)RAND_NUM(1,5) ); //(float)(5+rand()%20)/10 ); 
	}

}

void Stars_Move( void )
{
	int i;

	for( i = 0; i < MAX_STARS; i++ )
	{
		stars[i].counter2 += stars[i].maxcount2;
		if( (stars[i].counter2 > 1.0f) || (stars[i].counter2 < 0.1f) )
		{
			stars[i].maxcount2 *= -1;
		}
	}
}

void Stars_Draw( void )
{
	int i;
	
	Ren_BindImage( pic_particle->textureid );

	for( i = 0; i < MAX_STARS; i++ )
	{
		Ren_Color( 0.9f, 0.9f, 1.0f, stars[i].counter2 );
		Ren_DrawEntity( &stars[i], REN_OVERLAY );
	}

	Ren_Color( 1.0f, 1.0f, 1.0f, 1.0f );
}

/* ------------------------------
	STARS
---------------------------------*/
