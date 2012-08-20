#include "game.h"

model_t		*mod_bullet;

/* ------------------------------
	BULLETS
---------------------------------*/
void Bullets_Init( void )
{
	int i;
	vec3_t origin;

	ZeroVector( origin );

	for( i = 0; i < MAX_BULLETS; i++ )
	{
		bullets[i].type = ET_BULLET;
		bullets[i].radius = 0.9f;
		bullets[i].scale = 0.8f;
		bullets[i].alive = False;
		bullets[i].counter1 = 6;		// MAX BULLET LIFE LENGTH
		bullets[i].counter2 = 0.0f;

		bullets[i].angles[YAW] = 0;
		bullets[i].angles[ROLL] = 0;
		bullets[i].angles[PITCH] = 0;

		G_Spawn( &bullets[i], origin, bullets[i].radius, bullets[i].scale ); 
	}

	mod_bullet		= Ren_LoadModel( "data/models/orb.ms3d" );


}

void Bullets_Move( void )
{
	int i, j, k;
	vec3_t velocity, forward, zero;
	static int lastframe;
	int thisframe;
	sboolean drawparticle = False;
	int traillength = 10;

	thisframe = OS_Milliseconds();

	ZeroVector( zero );

	if( (thisframe - lastframe) > traillength )
	{
		lastframe = thisframe;
		drawparticle = True;
	}

	for( i = 0; i < MAX_BULLETS; i++ )
	{
		if( bullets[i].alive )
		{
			bullets[i].counter2 += gi.framelen*5;
			bullets[i].angles[YAW] += gi.framelen*300;
			//bullets[i].angles[ROLL] += gi.framelen*300;

			//printf( "Bullet Position: (%f,%f,%f)", bullets[i].origin[0], bullets[i].origin[1], bullets[i].origin[2] );

			/* Check if bullet's life has expired */
			if( bullets[i].counter2 > bullets[i].counter1 )
			{
				bullets[i].alive = False;
				bullets[i].counter2 = 0.0f;
				G_StartExplosion( bullets[i].origin, bullets[i].velocity, fuelcolor, PT_FADE, 0.04f, 1.0f, 5 );
				continue;
			}

			/* Spawn a nice trail behind the bullet */
			if( drawparticle )
			{
				ZeroVector( velocity );
				Particles_Spawn( bullets[i].origin, velocity, bullets[i].color, PT_FADE, 0.04f, 3.0f );
			}

			G_MoveEntity( &bullets[i], gi.framelen );
			G_WorldWrap( &bullets[i] );

			/* Check for collisions */
			/* STROIDS */
			for( j = 0; j < MAX_STROIDS; j++ )
			{
				if( !stroids[j].alive )
					continue;

				if( G_Collision( &bullets[i], &stroids[j] ) )
				{
					bullets[i].alive = False;
					bullets[i].counter2 = 0.0f;

					ZeroVector( forward );
					G_StartExplosion( stroids[j].origin, forward, enginecolor, PT_FADE, 0.004f, 10.0f, 15 );

					/* give player points for killing stroid */
					playerinfo.score += (int)(playerinfo.stroidpoints*stroids[j].scale);
					
					Snd_PlaySound( snd_explode1->soundid );
					
					for( k = 0; k < stroids[j].scale; k++ )
					{
						/* When the Asteroid hits one kill it */
						if( (stroids[j].scale*0.5f) < 1 )
							continue;

						/* Spawn some baby stroids */
						velocity[0] = ( (float)RAND_NUM(1,20) ) - 10.0f;
						velocity[1] = ( (float)RAND_NUM(1,20) ) - 10.0f;
						velocity[2] = ( (float)RAND_NUM(1,20) ) - 10.0f;

						G_StroidSpawn( stroids[j].origin, velocity, stroids[j].radius * 0.5f, stroids[j].scale * 0.5f );
					}

					/* Kill the hit asteroid */
					stroids[j].alive = False;
				}
			}

			
			for( j = 0; j < MAX_STATIONS; j++ )
			{
				if( !stations[j].alive )
					continue;

				if( G_Collision( &bullets[i], &stations[j] ) )
				{
					
					if( &stations[j] == bullets[i].owner )
						continue;

					bullets[i].alive = False;
					bullets[i].counter2 = 0.0f;

					ZeroVector( forward );
					G_StartExplosion( bullets[i].origin, forward, enginecolor, PT_FADE, 0.004f, 5.0f, 15 );

					Snd_PlaySound( snd_explode1->soundid );

					stations[j].counter1 -= 3.0f;
				}
			}
			

			/* Our Ship */
			if( G_Collision( &bullets[i], &ship ) )
			{
				if( &ship != bullets[i].owner )
				{
				
					bullets[i].alive = False;
					bullets[i].counter2 = 0.0f;
					Snd_PlaySound( snd_explode1->soundid );
					playerinfo.health -= 10;
					G_StartExplosion( ship.origin, ship.velocity, enginecolor, PT_FADE, 0.004f, 10.0f, 15 );
					ship.angles[YAW] += ( (float)RAND_NUM(1,20) ) - 10.0f;
					ship.angles[PITCH] += ( (float)RAND_NUM(1,20) ) - 10.0f;
				}
			}
		}
	}
}

void Bullets_Fire( vec3_t origin, vec3_t velocity, vec4_t color, entity_t *owner, float life )
{
	int i;

	vec3_t f, u, r;

	for( i = 0; i < MAX_BULLETS; i++ )
	{
		if( bullets[i].alive == False )
		{
			/* Spawn a new Bullets */
			bullets[i].alive = True;

			bullets[i].radius = 0.9f;
			bullets[i].scale = 0.8f;
			bullets[i].counter1 = life;		// MAX BULLET LIFE LENGTH
			bullets[i].counter2 = 0.0f;

			bullets[i].owner = owner;

			bullets[i].angles[YAW] = 0;
			bullets[i].angles[ROLL] = 0;
			bullets[i].angles[PITCH] = 0;

			bullets[i].color[RED]	= color[RED];
			bullets[i].color[GREEN] = color[GREEN];
			bullets[i].color[BLUE]	= color[BLUE];
			bullets[i].color[ALPHA] = color[ALPHA];
			
			VectorCopy( origin, bullets[i].origin );
			VectorCopy( velocity, bullets[i].velocity );

			AngleVectors( ship.angles, f, r, u );
			
			VectorScale( f, 4, f );

			VectorAdd( f, bullets[i].origin, bullets[i].origin );

			//bullets[i].origin[0] += 3;

			return;
		}
	}
}

void Bullets_Draw( void )
{
	int i, j, k, l;
	vec3_t neworigin, addorigin;

	i = j = k = 0;

	//Ren_BindImage( pic_stroid->textureid );

	Ren_Color( 0.95f, 0.5f, 0.1f, 0.7f );

	for( l = 0; l < MAX_BULLETS; l++ )
	{
		if( bullets[l].alive )
		{
			for( i = -1; i < 2; i++ )
			{
				for( j = -1; j < 2; j++ )
				{
					for( k = -1; k < 2; k++ )
					{
						ZeroVector( addorigin );
						ZeroVector( neworigin );

						addorigin[0] = (float)WORLD_WIDTH_MAX*2*i;
						addorigin[1] = (float)WORLD_HEIGHT_MAX*2*j;
						addorigin[2] = (float)WORLD_DEPTH_MAX*2*k;

						VectorAdd( bullets[l].origin, addorigin, neworigin );

						//printf( "SO(%f,%f,%f)\n", stroids[l].origin[0], stroids[l].origin[1], stroids[l].origin[2] );
						//printf( "NO(%f,%f,%f)\n", neworigin[0], neworigin[1], neworigin[2] );

						//Ren_BindImage( pic_particle->textureid );
						//Ren_DrawEntity( &bullets[l], &bullets_rd );
						Ren_DrawModelLocation( mod_bullet->modelid, &bullets[l], neworigin );
					}
				}
			}
		}
	}
	/*
	int i;

	
	for( i = 0; i < MAX_BULLETS; i++ )
	{
		if( bullets[i].alive )
		{
			/* Load the bullet texture 
			Ren_Color( 0.0f, 1.0f, 0.0f, 1.0f );
			Ren_BindImage( pic_particle->textureid );
			Ren_DrawEntity( &bullets[i], &bullets_rd );
			Ren_DrawModel( mod_bullet->modelid, &bullets[i] );
		}
	}
	*/

	Ren_Color( 1.0f, 1.0f, 1.0f, 1.0f );

}
/* ------------------------------
	BULLETS
---------------------------------*/

/* ------------------------------
	BOMB
---------------------------------*/

#define BOMB_DEAD		0
#define BOMB_FLYING		1
#define BOMB_SONICMODE	2

void W_BombInit( void )
{
	int i;
	vec3_t origin;

	ZeroVector( origin );

	for( i = 0; i < MAX_BOMBS; i++ )
	{
		bombs[i].type = ET_BOMB;
		bombs[i].radius = 1.0f;
		bombs[i].scale = 10.0f;
		bombs[i].alive = False;
		bombs[i].counter1 = 50.0f;		// scale
		bombs[i].counter2 = 0.0f;		// lifespan
		bombs[i].maxcount1 = 20.0f;		// maxscale
		bombs[i].maxcount2 = 6.0f;		// maxlifespan
		bombs[i].color[RED] = 1.0f;
		bombs[i].color[GREEN] = 1.0f;
		bombs[i].color[BLUE] = 1.0f;
		bombs[i].color[ALPHA] = 1.0f;

		bombs[i].angles[YAW] = 0;
		bombs[i].angles[ROLL] = 0;
		bombs[i].angles[PITCH] = 0;

		bombs[i].flag1 = BOMB_DEAD;

		ZeroVector( bombs[i].origin );
		ZeroVector( bombs[i].velocity );
	}


}

void W_SonicExplosion( vec3_t origin )
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

		scalar = ((float)rand() / RAND_MAX) * 200.0f;

		AngleVectors( angles, forward, right, up );
		VectorNormalize( forward, forward );
		VectorScale( forward, scalar, forward );
		//VectorAdd( forward, velocity, forward );

		Particles_Spawn( origin, forward, enginecolor, PT_FADE, 0.001f, 20.0f );
	}
}

void W_BombMove( void )
{
	int i, j, k;
	vec3_t velocity, forward;
	static int lastframe;
	int thisframe;
	sboolean drawparticle = False;
	int traillength = 20;

	thisframe = OS_Milliseconds();

	if( (thisframe - lastframe) > traillength )
	{
		lastframe = thisframe;
		drawparticle = True;
	}

	for( i = 0; i < MAX_BOMBS; i++ )
	{
		if( bombs[i].alive )
		{
			bombs[i].counter2 += gi.framelen*5;
			bombs[i].angles[YAW] += gi.framelen*300;
			

			if( ((G_KeyDown('b') || G_KeyDown(K_MOUSE2) || G_KeyDown(K_JOY3)) && bombs[i].flag1 == BOMB_FLYING))
			{
				bombs[i].flag1 = BOMB_SONICMODE;
				bombs[i].radius = 1.5f;
				W_SonicExplosion( bombs[i].origin );

				bombs[i].counter2 = 0.0f;
				Snd_PlaySound( snd_sonic->soundid );
			}

						
			/* Check if the bomb should explode */
			if( (bombs[i].counter2 > bombs[i].maxcount2) && (bombs[i].flag1 == BOMB_FLYING) )
			{
				bombs[i].flag1 = BOMB_SONICMODE;
				bombs[i].radius = 1.5f;
				W_SonicExplosion( bombs[i].origin );
				//ZeroVector( bombs[i].velocity );

				bombs[i].counter2 = 0.0f;
				//bombs[i].alive = False;
				Snd_PlaySound( snd_sonic->soundid );
				//continue;
			}

			if( bombs[i].flag1 == BOMB_FLYING ) {
				/* Spawn a nice trail behind the bullet */
				if( drawparticle )
				{
					ZeroVector( velocity );
					Particles_Spawn( bombs[i].origin, velocity, enginecolor, PT_FADE, 0.04f, 6.0f );
				}

				G_MoveEntity( &bombs[i], gi.framelen );
				G_WorldWrap( &bombs[i] );
			}

			if( bombs[i].flag1 == BOMB_SONICMODE )
			{
				bombs[i].scale += bombs[i].counter1*gi.framelen;

				if( bombs[i].scale > bombs[i].maxcount1 )
				{
					//bombs[i].alive = False;
					bombs[i].counter2 = 0.0f;
					bombs[i].flag1 = BOMB_DEAD;
				}
			}

			if( bombs[i].flag1 == BOMB_DEAD )
			{
				bombs[i].color[ALPHA] -= gi.framelen;
				bombs[i].scale += bombs[i].counter1*gi.framelen;

				if( bombs[i].color[ALPHA] <= 0 ) {
					bombs[i].alive = False;
				}
			}

			/* Check for collisions */
			/* STROIDS */
			for( j = 0; j < MAX_STROIDS; j++ )
			{
				if( !stroids[j].alive )
					continue;

				if( G_Collision( &bombs[i], &stroids[j] ) )
				{
					if( bombs[i].flag1 == BOMB_FLYING )
						bombs[i].flag1 = BOMB_SONICMODE;

					ZeroVector( forward );
					G_StartExplosion( stroids[j].origin, forward, enginecolor, PT_FADE, 0.004f, 10.0f, 15 );

					/* give player points for killing stroid */
					playerinfo.score += (int)(playerinfo.stroidpoints*stroids[j].scale);
					
					//Snd_PlaySound( snd_sonic->soundid );
					Snd_PlaySound( snd_explode1->soundid );
					if( bombs[i].flag1 == BOMB_FLYING )
						W_SonicExplosion( bombs[i].origin );
					
					for( k = 0; k < stroids[j].scale; k++ )
					{
						/* When the Asteroid hits one kill it */
						if( (stroids[j].scale*0.5f) < 1 )
							continue;

						/* Spawn some baby stroids */
						velocity[0] = ( (float)RAND_NUM(1,20) ) - 10.0f;
						velocity[1] = ( (float)RAND_NUM(1,20) ) - 10.0f;
						velocity[2] = ( (float)RAND_NUM(1,20) ) - 10.0f;

						G_StroidSpawn( stroids[j].origin, velocity, stroids[j].radius * 0.5f, stroids[j].scale * 0.5f );
					}

					/* Kill the hit asteroid */
					stroids[j].alive = False;
				}
			}
			
			/* SPACE STATIONS */
			/*
			for( j = 0; j < MAX_STATIONS; j++ )
			{
				if( !stations[j].alive )
					continue;

				if( G_Collision( &bombs[i], &stations[j] ) )
				{
					
					if( &stations[j] == bombs[i].owner )
						continue;

					if( bombs[i].flag1 != BOMB_SONICMODE )
						Snd_PlaySound( snd_sonic->soundid );


					bombs[i].flag1 = BOMB_SONICMODE;
					//bullets[i].counter2 = 0.0f;

					ZeroVector( forward );
					G_StartExplosion( bullets[i].origin, forward, &enginecolor, PT_FADE, 0.004f, 5.0f, 15 );

					
					stations[j].counter1 -= 1.0f;
				}
			}
			*/

		}
	}
}

void W_BombFire( vec3_t origin, vec3_t velocity, int type )
{
	int i;

	vec3_t f, u, r;

	for( i = 0; i < MAX_BOMBS; i++ )
	{
		if( bombs[i].alive == False )
		{
			/* Spawn a new bombs */
			bombs[i].alive = True;
			

			bombs[i].scale = 1.0f;
			bombs[i].radius = 0.8f;
			bombs[i].counter1 = 40.0f;		// scale
			bombs[i].counter2 = 0.0f;		// lifespan
			bombs[i].maxcount1 = 100.0f;		// maxscale
			bombs[i].maxcount2 = 10.0f;		// maxlifespan
			bombs[i].color[ALPHA] = 1.0f;

			bombs[i].angles[YAW] = 0;
			bombs[i].angles[ROLL] = 0;
			bombs[i].angles[PITCH] = 0;

			ZeroVector( bombs[i].origin );
			ZeroVector( bombs[i].velocity );

			bombs[i].flag1 = BOMB_FLYING;
			
			VectorCopy( origin, bombs[i].origin );
			VectorCopy( velocity, bombs[i].velocity );

			AngleVectors( ship.angles, f, r, u );
			
			VectorScale( f, 4, f );

			VectorAdd( f, bombs[i].origin, bombs[i].origin );

			//bombs[i].origin[0] += 3;

			return;
		}
	}
}
	
/* ------------------------------
	BOMB
---------------------------------*/
