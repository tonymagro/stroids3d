#include "game.h"

/* ------------------------------
	POWERUPS
---------------------------------*/
void PowerUps_Init()
{
	int i;
	vec3_t origin;

	for( i = 0; i < MAX_POWERUPS; i++ )
	{
		origin[0] = (float)RAND_NUM( WORLD_WIDTH_MIN, WORLD_WIDTH_MAX );
		origin[1] = (float)RAND_NUM( WORLD_HEIGHT_MIN, WORLD_HEIGHT_MAX );
		origin[2] = (float)RAND_NUM( WORLD_DEPTH_MIN, WORLD_DEPTH_MAX );

		powerups[i].type = ET_POWERUP;
		powerups[i].alive = False;

		G_Spawn( &powerups[i], origin, 1.0f, 0.7f );

		powerups[i].scale = 10.0f;
	}

	for( i = 0; i < (MAX_POWERUPS*0.6); i++ )
	{
		powerups[i].alive = True;
		powerups[i].angles[YAW] = 0;
		powerups[i].counter2 = ( ( (float)rand() / RAND_MAX * 0.08f ) );
		ZeroVector( origin );
		origin[0] = ( ( (float)rand() / RAND_MAX * 5.0f ) );
		origin[1] = ( ( (float)rand() / RAND_MAX * 5.0f ) );
		VectorCopy( origin, powerups[i].velocity );
	}
	
}

void PowerUps_Draw()
{
	int i;
	
	Ren_BindImage( pic_armorpowup->textureid );
	
	for( i = 0; i < MAX_POWERUPS; i++ )
	{
		if( powerups[i].alive )
		{
			Ren_DrawEntity( &powerups[i], 0 );
		}
	}
}

void PowerUps_Move()
{
	int i;

	for( i = 0; i < MAX_POWERUPS; i++ )
	{
		G_MoveEntity( &powerups[i], gi.framelen );
		G_WorldWrap( &powerups[i] );
	}

}

/* ------------------------------
	POWERUPS
---------------------------------*/

