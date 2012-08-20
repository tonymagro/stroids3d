#include "game.h"

/* ------------------------------
	STROIDS
---------------------------------*/

model_t		*mod_stroid;

void G_StroidSpawn( vec3_t origin, vec3_t velocity, float radius, float scale )
{
	int i;

	for( i = 0; i < MAX_STROIDS; i++ )
	{
		if( !stroids[i].alive )
		{
			VectorCopy( origin, stroids[i].origin );
			VectorCopy( velocity, stroids[i].velocity );

			stroids[i].scale = scale;
			stroids[i].radius = radius;
			stroids[i].angles[YAW] = 0;
			stroids[i].angles[PITCH] = 0;
			stroids[i].angles[ROLL] = 0;
			//printf( "Spawning Stroids @: (%f, %f)\n", stroids[i].origin[0], stroids[i].origin[1] );
			stroids[i].alive = True;

			return;
		}
	}
}

void Stroids_Move( void )
{
	int i;

	for( i = 0; i < MAX_STROIDS; i++ )
	{
		if( stroids[i].alive )
		{
			G_RotateEntity( &stroids[i], YAW, stroids[i].counter2*gi.framelen );
			G_RotateEntity( &stroids[i], PITCH, stroids[i].counter2*gi.framelen );
			G_MoveEntity( &stroids[i], gi.framelen );
			G_WorldWrap( &stroids[i] );
		}
	}
}

void Stroids_Draw( entity_t *self, vec3_t origin )
{
	Ren_DrawModelLocation( mod_stroid->modelid, self, origin );
}

void Stroids_KillAll( void )
{
	int i;

	for( i = 0; i < MAX_STROIDS; i++ )
	{
		stroids[i].alive = False;
	}

}

void Stroids_Shutdown( void )
{
}

void Stroids_Init( void )
{
	int i;
	vec3_t origin;

	for( i = 0; i < MAX_STROIDS; i++ )
	{
		origin[0] = (float)RAND_NUM( WORLD_WIDTH_MIN, WORLD_WIDTH_MAX );
		origin[1] = (float)RAND_NUM( WORLD_HEIGHT_MIN, WORLD_HEIGHT_MAX );
		origin[2] = (float)RAND_NUM( WORLD_DEPTH_MIN, WORLD_DEPTH_MAX );

		stroids[i].type = ET_STROID;
		stroids[i].alive = False;

		stroids[i].draw = Stroids_Draw;

		G_Spawn( &stroids[i], origin, 1.5f, (float)RAND_NUM(1,4) ); 
	}

	for( i = 0; i < (MAX_STROIDS*0.6); i++ )
	{
		stroids[i].alive = True;
		stroids[i].angles[YAW] = (float)RAND_NUM(1,360);
		stroids[i].counter2 = ((float)RAND_NUM(100,300)) - 150.0f;	// spinning
		ZeroVector( origin );
		origin[0] = ( (float)RAND_NUM(1,20) ) - 10.0f;
		origin[1] = ( (float)RAND_NUM(1,20) ) - 10.0f;
		origin[2] = ( (float)RAND_NUM(1,20) ) - 10.0f;
		VectorCopy( origin, stroids[i].velocity );
	}

	mod_stroid	= Ren_LoadModel( "data/models/asteroid.ms3d" );

	//stroids[1].scale = 60;

}

/* ------------------------------
	STROIDS
---------------------------------*/
