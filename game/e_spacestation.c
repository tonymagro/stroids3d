/* Enemy Space Shuttle */
#include "game.h"

model_t		*mod_station;

vec4_t trailcolor = { 1.0f, 0.3f, 0.0f, 1.0f };

void Stations_Explosion( vec3_t origin )
{
	int numparticles = 10;
	int i;

	for( i = 0; i < 20; i++ )
	{
		vec3_t angles;
		float scalar;
		vec3_t forward, right, up;

		angles[YAW] = (float)(rand()%359);
		angles[PITCH] = (float)(rand()%359);
		angles[ROLL] = (float)(rand()%359);

		scalar = ((float)rand() / RAND_MAX) * 50.0f;

		AngleVectors( angles, forward, right, up );
		VectorNormalize( forward, forward );
		VectorScale( forward, scalar, forward );
		//VectorAdd( forward, velocity, forward );

		Particles_Spawn( origin, forward, enginecolor, PT_FADE, 0.005f, 40.0f );
	}
}

void Stations_Draw( entity_t *self, vec3_t origin )
{
	Ren_DrawModelLocation( mod_station->modelid, self, origin );
}

void Stations_Move( void )
{
	int i;
	vec3_t up, right;
	vec3_t forward, dist_vect;
	float distance;
	static int lastframe;
	int thisframe;
	int shottime = 1000;
	float shotdistance = 350.0f;
	static sboolean fire = False;

	//trailcolor.r = 0.0f;
	//trailcolor.g = 0.3f;
	//trailcolor.b = 1.0f;


	AngleVectors( ship.angles, forward, right, up );

	thisframe = OS_Milliseconds();

	if( (thisframe - lastframe) > shottime)
	{
			lastframe = thisframe;
			fire = True;
	}
	
	for( i = 0; i < MAX_STATIONS; i++ )
	{
		if( !stations[i].alive )
			continue;
		
		if( stations[i].counter1 <= 0.0f )
		{
			stations[i].alive = False;
			Stations_Explosion( stations[i].origin );
			Snd_PlaySound( snd_explode1->soundid );
		}

		//VectorScale( ship.velocity, 20.0f, dist_vect );
		VectorSubtract( ship.origin, stations[i].origin, dist_vect );
		distance = VectorLength( dist_vect );
		VectorAdd( ship.origin, ship.velocity, dist_vect );

		VectorSubtract( dist_vect, stations[i].origin, dist_vect );
		VectorNormalize( dist_vect, forward );
		VectorScale( forward, 150, forward );
		

		//Ren_DrawLine( stations[i].origin, ship.origin, distance );

		G_RotateEntity( &stations[i], YAW, 20*gi.framelen );

		if( (distance < shotdistance) && (fire) )
		{
			Bullets_Fire( stations[i].origin, forward, trailcolor, &stations[i], 10.0f );
			forward[1] += 10;
			Bullets_Fire( stations[i].origin, forward, trailcolor, &stations[i], 10.0f );
			forward[1] -= 20;
			Bullets_Fire( stations[i].origin, forward, trailcolor, &stations[i], 10.0f );
			forward[1] += 10;
			forward[2] += 10;
			Bullets_Fire( stations[i].origin, forward, trailcolor, &stations[i], 10.0f );
			forward[2] -= 20;
			Bullets_Fire( stations[i].origin, forward, trailcolor, &stations[i], 10.0f );
		}

		//AngleVectors( stations[i].angles, forward, NULL, NULL );
		//VectorScale( forward, stations[i].speed, forward );
		//VectorCopy( forward, stations[i].velocity );
		//G_MoveEntity( &stations[i], gi.framelen );
		G_WorldWrap( &stations[i] );
	}

	fire = False;
}

void Stations_Init( void )
{
	int i;

	for( i = 0; i < MAX_STATIONS; i++ )
	{
		stations[i].alive			= True;
		stations[i].angles[YAW]		= 0.0f; //(float)RAND_NUM(1,360);
		stations[i].angles[PITCH]	= 0.0f; //(float)RAND_NUM(1,360);
		stations[i].angles[ROLL]	= 90.0f; //(float)RAND_NUM(1,360);
		stations[i].counter1		= 100.0f;		// Health;
		stations[i].counter2		= 0.0f;
		stations[i].maxcount1		= 100.0f;		// MaxHealth;
		stations[i].maxcount2		= 0.0f;

		stations[i].speed			= 45.0f;
		stations[i].radius			= 15.0f;
		stations[i].scale			= 1.0f;
		stations[i].type			= ET_STATION;

		stations[i].hitdamage		= 10.0f;

		stations[i].owner			= &stations[i];

		//printf( "Entity Type %i: %i\n", i, stations[i].type );

		ZeroVector( stations[i].origin );
		ZeroVector( stations[i].velocity );
		
		// Give the station random origin in the world
		stations[i].origin[0] = (float)RAND_NUM( WORLD_WIDTH_MIN, WORLD_WIDTH_MAX );
		stations[i].origin[1] = (float)RAND_NUM( WORLD_HEIGHT_MIN, WORLD_HEIGHT_MAX );
		stations[i].origin[2] = (float)RAND_NUM( WORLD_DEPTH_MIN, WORLD_DEPTH_MAX );

		//stations[i].velocity[0] = 1.0f;
		//stations[i].velocity[1] = 1.0f;
		//stations[i].velocity[2] = 1.0f;

		//stations[i].think = E_ShipMove;
		stations[i].draw	= Stations_Draw;
	}

	mod_station	= Ren_LoadModel( "data/models/station.ms3d" );
}
