#include "game.h"
//#include "../scommon/keys.h"

/* Count Frames Per Second */
float lastframe,thisframe;
int ff, fps;

/* Frame Length */
static int	T0;
float framelen;

sboolean bDrawCollision = False;

entity_t*   ent_lsthead;

entity_t	ship;
entity_t	ship2;

entity_t	stations[MAX_STATIONS];

entity_t	shield;
entity_t	stroids[MAX_STROIDS];
entity_t	bullets[MAX_BULLETS];
entity_t	bombs[MAX_BOMBS];
entity_t	powerups[MAX_POWERUPS];
/* Particle Engine */
entity_t   	particles[MAX_PARTICLES];
/* Stars */
entity_t	stars[MAX_STARS];

/* images */
image_t		*pic_ship;
//image_t		*pic_stroid;
image_t		*pic_particle;
image_t		*pic_hudship;
image_t		*pic_block;
image_t		*pic_reticle;
image_t		*pic_ripple;

image_t		*pic_armorpowup;

/* sounds */
sound_t		*snd_laser1;
sound_t		*snd_explode1;
sound_t		*snd_music1;
sound_t		*snd_sonic;

vec3_t distance;

/* Player State Information */
player_info_t		playerinfo;

plane_t      frustum[6];

/* Game State */
game_info_t	gi;


/* ------------------------------
	Entities
---------------------------------*/

/* This functin acutally serves no purpose at this point in time*/
void G_Spawn( entity_t *ent, vec3_t origin, float radius, float scale )
{
	VectorCopy( origin, ent->origin );
	ent->radius = radius;
	ent->scale	= scale;
	//ent->state	= ES_ALIVE;
	ZeroVector( ent->velocity );
	//ZeroVector( ent->origin );
}

int	G_Collision( entity_t *ent1, entity_t *ent2 )
{

	// Find the distance between the centers
	float dist;
	float radii;
	dist = VectorDistance( ent1->origin, ent2->origin );
	// Square the distance
	//dist = dist * dist;
	// find the sum of the radii squared
	radii = (ent1->radius*ent1->scale) + (ent2->radius*ent2->scale);
	radii = radii * radii;

	// Test for collision
	if( dist < radii )
		return True;

	return False;

	// This old way used a sqrt function which was costly
	/*
	vec3_t dist;

	VectorSubtract( ent1->origin, ent2->origin, dist );

	if( VectorLength( dist ) <= ((ent1->radius*ent1->scale) + (ent2->radius*ent2->scale)) )
		return True;
	*/
}

void G_RotateEntity( entity_t *ent, int axis, float angle ) {
	ent->angles[axis] += angle;
	ent->angles[axis] /= 360.0f;
	
	ent->angles[axis] -= (int)ent->angles[axis]; // drop whole no.
	
	ent->angles[axis] *= 360.0f;

/* Tony Magro is a fucking retard.

	if ( ent->angles[axis] > 359 )
		ent->angles[axis] = 0;
	if( ent->angles[axis] < 0 )
		ent->angles[axis] = 359;
*/// By Travis Bemann
}

void G_MoveEntity( entity_t *ent, float scalar ) {
	int i;
	vec3_t vector;
	
	VectorScale( ent->velocity, scalar, vector );
			
	for ( i = 0; i < 3; i++ )
		ent->origin[i] += vector[i];
}

void G_AccelEntity( entity_t *ent, float scalar ) {
	float vmag;
	vec3_t forward;
	vec3_t right, up;

	AngleVectors( ent->angles, forward, right, up );
	
	// speed em up in the proper direction
	VectorMA( ent->velocity, scalar, forward, ent->velocity );

	vmag = VectorLength( ent->velocity );

	
	if ( vmag >= ent->maxVelocity ) {
		// dont perform a sqrt calc twice, use scale.
		VectorScale( ent->velocity, 1.0f / vmag, ent->velocity );
		VectorScale( ent->velocity, ent->maxVelocity, ent->velocity );
	}
		
}

void G_WorldWrap( entity_t *ent )
{
	if( ent->origin[0] < WORLD_WIDTH_MIN )
		ent->origin[0] = WORLD_WIDTH_MAX + ent->origin[0] - WORLD_WIDTH_MIN;
	else if( ent->origin[0] > WORLD_WIDTH_MAX )
		ent->origin[0] = WORLD_WIDTH_MIN - ent->origin[0] + WORLD_WIDTH_MAX;

	if( ent->origin[1] < WORLD_HEIGHT_MIN )
		ent->origin[1] = WORLD_HEIGHT_MAX + ent->origin[1] - WORLD_HEIGHT_MIN;
	else if( ent->origin[1] > WORLD_HEIGHT_MAX )
		ent->origin[1] = WORLD_HEIGHT_MIN - ent->origin[1] + WORLD_HEIGHT_MAX;

	if( ent->origin[2] < WORLD_DEPTH_MIN )
		ent->origin[2] = WORLD_DEPTH_MAX + ent->origin[2] - WORLD_DEPTH_MIN;
	else if( ent->origin[2] > WORLD_DEPTH_MAX )
		ent->origin[2] = WORLD_DEPTH_MIN - ent->origin[2] + WORLD_DEPTH_MAX;
}


/* Cull spheres of radius CULLING_SLOP */
char G_SphereVisible( vec3_t c, vec_t r )
{
	int i;
	for( i = 0; i < 6; i++ )
		if( frustum[i].normal[0] * c[0] +
			frustum[i].normal[1] * c[1] +
			frustum[i].normal[2] * c[2] +
			frustum[i].dist < - r )
			return 0;
	return 1;
}

/* Cull cubes */
char G_CubeVisible( vec3_t c )
{
	vec3_t cnr;
	for(         cnr[0] = c[0] - WORLD_WDTH / 2; cnr[0] < c[0] + WORLD_WDTH; cnr[0] += WORLD_WDTH / 2 )
		for(     cnr[1] = c[1] - WORLD_HGHT / 2; cnr[1] < c[1] + WORLD_HGHT; cnr[1] += WORLD_HGHT / 2 )
			for( cnr[2] = c[2] - WORLD_DPTH / 2; cnr[2] < c[2] + WORLD_DPTH; cnr[2] += WORLD_DPTH / 2 )
				if( G_SphereVisible( cnr, 0.0f ) ) return 1;
	return 0;
}

void G_DrawWorld( void )
{
	vec3_t off;
	vec3_t neworigin;
	entity_t* ent;
//	vec3_t tmp;
	

	entity_type current;

#if DRAW_WORLD_BOXEN
	static vec3_t world_color  = {1.0f, 0.0f, 0.0f};
	static vec3_t draw_color   = {0.0f, 1.0f, 0.0f};
	static vec3_t nodraw_color = {0.0f, 0.0f, 1.0f};
#endif

	Ren_DrawSky();

    /* To draw an arbitrary number of cubes, we will need to change the three loops below */
	for(         off[0] = - num_worlds*WORLD_WDTH; off[0] <= num_worlds*WORLD_WDTH; off[0] += WORLD_WDTH )
		for(     off[1] = - num_worlds*WORLD_HGHT; off[1] <= num_worlds*WORLD_HGHT; off[1] += WORLD_HGHT )
			for( off[2] = - num_worlds*WORLD_DPTH; off[2] <= num_worlds*WORLD_DPTH; off[2] += WORLD_DPTH )
				if( (off[0] == 0.0f && off[1] == 0.0f && off[2] == 0.0f) ||
#if QUICK_WORLD_CULLING
					G_CubeVisible( off ) )
#else
					G_SphereVisible( off, 0.866 * WORLD_WDTH ) )
#endif
				{
#if DRAW_WORLD_BOXEN
					if( off[0] == 0.0f && off[1] == 0.0f && off[2] == 0.0f )
						Ren_DrawWorldBox(off, world_color);
					else
						Ren_DrawWorldBox(off, draw_color);
#endif
					current = ET_NULL;

					for( ent = ent_lsthead; ent; ent = ent->next )
						if( ent->alive )
						{
							ZeroVector( neworigin );
							VectorAdd( ent->origin, off, neworigin );
							if( G_SphereVisible( neworigin, CULLING_SLOP ) ) switch( ent->type )
							{
							  case ET_BOMB:
								if( current != ET_BOMB )
								{
									current = ET_BOMB;
									Ren_Color( ent->color[RED], ent->color[GREEN], ent->color[BLUE], ent->color[ALPHA] );
									Ren_BindImage( pic_ripple->textureid );
								}
								if( (ent->flag1 == 2) || (ent->flag1 == 0)) {
									//Ren_Color( 1.0f, 1.0f, 1.0f, 0.2f );
									//Ren_SetPolyMode( True );
									//Ren_DrawModelLocation( mod_stroid->modelid, ent, neworigin );
									//Ren_SetPolyMode( False );
									//Ren_DrawSphere( neworigin, ent->scale );
									//Ren_DrawImage3d( neworigin, ent->scale*1.5 );
									Ren_DrawImage3dPlane( neworigin, 0, 1, ent->scale*1.5f );
								}
								else 
									Ren_DrawModelLocation( mod_bullet->modelid, ent, neworigin );
								break;
							  case ET_BULLET:
								if( current != ET_BULLET )
								{
									current = ET_BULLET;
								}
								if( ent->owner->type == ET_STATION )
									Ren_Color( 0.0f, 0.3f, 1.0f, 1.0f );
								else
									Ren_Color( 0.95f, 0.3f, 0.0f, 0.2f );
								Ren_DrawModelLocation( mod_bullet->modelid, ent, neworigin );
								break;
							  case ET_PARTICLE:
								if( current != ET_PARTICLE )
								{
									current = ET_PARTICLE;
									Ren_BindImage( pic_particle->textureid );
								}
								Ren_Color( ent->u.particle.color[RED],
										   ent->u.particle.color[GREEN],
										   ent->u.particle.color[BLUE],
										   ent->maxcount2 );
								Ren_DrawParticle( neworigin, ent->scale );
								break;
							  case ET_POWERUP:
								if( current != ET_POWERUP )
								{
									current = ET_POWERUP;
									Ren_Color( 1.0f, 1.0f, 1.0f, 1.0f );
									Ren_BindImage( pic_armorpowup->textureid );
								}
								/* TEMP hack to make this work */
								VectorSwap( ent->origin, neworigin );
								Ren_DrawEntity( ent, 0 );
								VectorSwap( ent->origin, neworigin );
								break;
							  case ET_SHIP:
								//if( off[0] == 0.0f && off[1] == 0.0f && off[2] == 0.0f ) break;
								if( current != ET_SHIP )
								{
									current = ET_SHIP;
									Ren_Color( 1.0f, 1.0f, 1.0f, 1.0f );
									Ren_BindImage( pic_ship->textureid );
								}
								/* TEMP hack to make this work */
								VectorSwap( ent->origin, neworigin );
								ent->draw( ent, neworigin );
								VectorSwap( ent->origin, neworigin );
								break;
							  case ET_STROID:
								if( current != ET_STROID )
								{
									current = ET_STROID;
									Ren_Color( 1.0f, 1.0f, 1.0f, 1.0f );
								}
								ent->draw( ent, neworigin );
								break;
							  case ET_STATION:
								if( current != ET_STATION )
								{
									current = ET_STATION;
									Ren_Color( 1.0f, 1.0f, 1.0f, 1.0f );
								}
								ent->draw( ent, neworigin );
								break;
							  default:
								break;
							}
							
							if( bDrawCollision ) {
								switch( ent->type )
								{
									case ET_BULLET:
										break;
									default:
										Ren_Color( 0.0f, 0.0f, 1.0f, 0.4f );
										Ren_DrawSphere( neworigin, ent->scale*ent->radius );
										break;
								}
							}
						}
#if DRAW_WORLD_BOXEN
				} else {
					Ren_DrawWorldBox(off, nodraw_color);
#endif
				}
}
/* ------------------------------
	Entities
---------------------------------*/



/* ------------------------------
	INFO SCREEN
---------------------------------*/
void InfoScreen_Init( void )
{
}

void InfoScreen_Move( void )
{
}

void InfoScreen_Draw( void )
{
	if( playerinfo.state == PS_NEXTLEVEL )
	{
		Ren_BindImage( pic_block->textureid );
		Ren_Color( 0.3f, 0.3f, 1.0f, 0.4f );
		Ren_DrawRectangle( 300, 100, 500, 500 );
		Ren_Color( 1.0f, 1.0f, 1.0f, 1.0f );
		Ren_printf( 3.91f, 5.47f, 0.4f, "LEVEL: %i", playerinfo.level );
		Ren_printf( 3.91f, 5.08f, 0.4f, "SCORE: %i", playerinfo.score );
		Ren_printf( 3.91f, 4.82f, 0.4f, "LIVES: %i", playerinfo.lives );
		Ren_Color( 0.0f, 0.0f, 1.0f, 1.0f );
		Ren_printf( 3.0f, 4.0f, 0.4f, "Press Enter when ready" );
	}
	else if( playerinfo.state == PS_DEAD )
	{
		Ren_Color( 1.0f, 0.0f, 0.0f, 1.0f );
		Ren_printf( 3.91f, 5.73f, 0.4f, "GAME OVER");
		Ren_Color( 1.0f, 1.0f, 1.0f, 1.0f );
		Ren_printf( 3.91f, 5.34f, 0.4f, "LEVEL: %i", playerinfo.level );
		Ren_printf( 3.91f, 5.08f, 0.4f, "SCORE: %i", playerinfo.score );

		Ren_Color( 0.0f, 0.0f, 1.0f, 1.0f );
		Ren_printf( 3.42f, 3.91f, 0.4f, "GAME WILL RESTART" );
	}
	else {
		return;
	}

	Ren_Color(1.0f, 1.0f, 1.0f, 1.0f );
}

/* ------------------------------
	INFO SCREEN
---------------------------------*/
/*
void G_PlayerShowScore( void )
{
	static float counter = 0.0f;
	static float increment = 0.1f;
	int ready;

	ready = 0;
	
	while( ready++ < (300) )
	{
		// Make Things blink 
		if( (counter > 1.0f) || (counter < 0.0f))
			increment *= -1;
		counter += (increment);
		
		if( playerinfo.lives > 0 )
		{
			Ren_Begin();
				Ren_SetGL2D();
				Ren_Color( 1.0f, 1.0f, 1.0f, 1.0f );
				Ren_printf( 400, 420, "LEVEL: %i", playerinfo.level );
				Ren_printf( 400, 390, "SCORE: %i", playerinfo.score );
				Ren_printf( 400, 370, "LIVES: %i", playerinfo.lives );
				Ren_Color( 0.0f, 0.0f, 1.0f, counter );
				Ren_printf( 392, 300, "GET READY" );
			Ren_End();
		}
		else
		{
			Ren_Begin();
				Ren_SetGL2D();
				Ren_Color( 1.0f, 0.0f, 0.0f, 1.0f );
				Ren_printf( 400, 440, "GAME OVER");
				Ren_Color( 1.0f, 1.0f, 1.0f, 1.0f );
				Ren_printf( 400, 410, "LEVEL: %i", playerinfo.level );
				Ren_printf( 400, 390, "SCORE: %i", playerinfo.score );

				Ren_Color( 0.0f, 0.0f, 1.0f, counter );
				Ren_printf( 350, 300, "GAME WILL RESTART" );
			Ren_End();
		}
	}

	Ren_Color(1.0f, 1.0f, 1.0f, 1.0f );
}
*/

void G_PlayerDied( void )
{

	//G_PlayerShowScore();

	/* Set Gameplay Type Based on Level */
	playerinfo.counter	= 0;
	playerinfo.health	= 100;
	playerinfo.maxHealth = 100;
	playerinfo.paused	= False;
	playerinfo.r_count	= 0;
	playerinfo.r_counter = 0;
	playerinfo.shield_count = 0;
	playerinfo.win		= 0;
	playerinfo.win_counter = 0;
	playerinfo.shieldon = False;
	playerinfo.rate_of_fire = 100;
	playerinfo.maxEnergy = 1.0f;
	playerinfo.energy = 1.0f;
	
	playerinfo.stroiddamage = 10;		// damage taken from a stroid
	playerinfo.stroidpoints = 10;		// points for killing a stroid

	ZeroVector( ship.velocity );
	ZeroVector( ship.origin );

	Ship_Init();

	playerinfo.state = PS_ALIVE;
}

/*
 * Restart the game
 */
void G_Restart( void )
{	
	/* Initialize Player Input */
	playerinfo.ammo		= 0;
	playerinfo.counter	= 0;
	playerinfo.health	= 100;
	playerinfo.maxHealth = 100;
	playerinfo.paused	= False;
	playerinfo.r_count	= 0;
	playerinfo.r_counter = 0;
	playerinfo.shield_count = 0;
	playerinfo.state	= PS_ALIVE;
	playerinfo.win		= 0;
	playerinfo.win_counter = 0;
	playerinfo.shieldon = False;
	playerinfo.rate_of_fire = 100;
	playerinfo.maxEnergy = 1.0f;
	playerinfo.energy = 1.0f;

	playerinfo.level	= 1;
	playerinfo.lives	= 3;
	playerinfo.score	= 0;
	
	playerinfo.stroiddamage = 10;		// damage taken from a stroid
	playerinfo.stroidpoints = 10;		// points for killing a stroid

	/* Initialize Game Entities */
	Stars_Init();
	Stroids_Init();
	Stations_Init();
	Bullets_Init();
	W_BombInit();
	Particles_Init();
	Ship_Init();
	PowerUps_Init();
	Hud_Init();
	InfoScreen_Init();

}

void G_LoadLevel( int level )
{
	/* Set Gameplay Type Based on Level */
	playerinfo.counter		= 0;
	playerinfo.health		= 100;
	playerinfo.maxHealth 	= 100;
	playerinfo.paused		= False;
	playerinfo.r_count		= 0;
	playerinfo.r_counter 	= 0;
	playerinfo.shield_count = 0;
	playerinfo.state		= PS_ALIVE;
	playerinfo.win			= 0;
	playerinfo.win_counter 	= 0;
	playerinfo.shieldon 	= False;
	playerinfo.rate_of_fire = 100;
	playerinfo.maxEnergy 	= 1.0f;
	playerinfo.energy 		= 1.0f;
	
	playerinfo.stroiddamage = 10*level;		// damage taken from a stroid
	playerinfo.stroidpoints = 10*level;		// points for killing a stroid



	/* Initialize Game Entities */
	Stars_Init();
	Stroids_Init();
	Stations_Init();
	Bullets_Init();
	W_BombInit();
	Particles_Init();
	Ship_Init();
	PowerUps_Init();
	InfoScreen_Init();
	Hud_Init();
}


/*
 * Initialize Game
 */
void G_Init( void )
{
	int i;

	printf( "Initializing Game\n" );

	T0 = 0;

	/* Seed the random number generator */
	srand( OS_Milliseconds() );

	/* Load Textures */
	pic_ship		= Ren_LoadImage( "data/ship1.tga" );
	//pic_stroid		= Ren_LoadImage( "data/asteroid.tga" );
	pic_particle	= Ren_LoadImage( "data/particle.tga" );
	pic_hudship		= Ren_LoadImage( "data/hud_ship.tga" );
	pic_armorpowup	= Ren_LoadImage( "data/pow_armor.tga" );
	pic_block		= Ren_LoadImage( "data/block.tga" );
	pic_reticle		= Ren_LoadImage( "data/reticle.tga" );
	pic_ripple		= Ren_LoadImage( "data/ripple.tga" );

	/* Load Sounds */
	snd_laser1		= Snd_LoadWAV( "data/sound/laser1.wav" );
	snd_explode1	= Snd_LoadWAV( "data/sound/explosion1.wav" );
	//snd_music1		= Snd_LoadWAV( "data/sound/stroids.wav" );
	snd_sonic		= Snd_LoadWAV( "data/sound/sonic.wav" );

	//Snd_PlayMusicLoop( snd_music1->soundid, 5 );

	Ren_CreateSkyBox( "data/skybox.tga" );

	G_InitHighScores();

	/* Get timer info */
	lastframe = (float)OS_Milliseconds();

	/* Start at level */
	G_Restart();
	G_LoadLevel( 1 );

	gi.gamestate	 = GS_INTRO;
	gi.lastgamestate = GS_NOTRUNNING;
	gi.paused		 = False;
	gi.menuopen		 = False;
	gi.fadein		 = False;
	gi.fadeout		 = False;
	gi.timescale	 = 1.0f;

	playerinfo.cameratype = 0;

	G_InitKeys();
	G_InitMenu();
	//G_InitLimbo();
	G_InitIntro();


	/* Initialize entity linked list */
	ent_lsthead = &ship;
	ship.next = &ship2;
	ship2.next = stroids;
	for( i = 0; i < MAX_STROIDS   - 1; i++ ) stroids[i].next   = stroids   + i + 1;
	stroids[MAX_STROIDS - 1].next = stations;
	for( i = 0; i < MAX_STATIONS   - 1; i++ ) stations[i].next   = stations   + i + 1;
	stations[MAX_STATIONS - 1].next = bullets;
	for( i = 0; i < MAX_BULLETS   - 1; i++ ) bullets[i].next   = bullets   + i + 1;
	bullets[MAX_BULLETS - 1].next = bombs;
	for( i = 0; i < MAX_BOMBS  - 1; i++ ) bombs[i].next  = bombs  + i + 1;
	bombs[MAX_BOMBS - 1].next = powerups;
	for( i = 0; i < MAX_POWERUPS  - 1; i++ ) powerups[i].next  = powerups  + i + 1;
	powerups[MAX_POWERUPS - 1].next = particles;
	for( i = 0; i < MAX_PARTICLES - 1; i++ ) particles[i].next = particles + i + 1;
	particles[MAX_PARTICLES - 1].next = 0;

	//G_NetworkInit();
}


/*
 * Shutdown Game 
 */
void G_Shutdown( void )
{
	//G_NetworkShutdown();
}


sboolean G_FadeIn( void )
{
	static int lastframe;
	static float fade = 1.0f;
	int fadetime = 10;
	int thisframe;

	thisframe = OS_Milliseconds();

	if( (thisframe - lastframe) > fadetime )
	{
			lastframe = thisframe;
			fade -= 0.01f;

			if( fade < 0 ) {
				fade = 1.0f;
				return True;
			}
	}

	Ren_DrawFade( fade );

	return False;
}

void G_DrawFrame( void )
{
	float  speed;

	if( gi.gamestate == GS_NOTRUNNING )
	{
		//G_DrawLimbo();
		gi.gamestate = GS_INTRO;
	} else if( gi.gamestate == GS_INTRO ) {
		Ren_SetGL2D();
		//G_DrawMenu();
		G_DrawIntro();
	} else if( gi.gamestate == GS_MENU ) { //|| gi.gamestate == GS_INTRO) {
		//G_DrawWorld();
		Ren_DrawSky();
		G_DrawMenu();
	} else if ( gi.gamestate == GS_RUNNING ) {
		//VectorSubtract( ship.velocity, ship.origin, speedv );
		speed = VectorLength( ship.velocity );

		G_DrawWorld();


#if DRAW_COORD_AXES
		Ren_DrawAxis();
#endif
		/* Switch into 2d view */
		Ren_SetGL2D();
		Hud_Draw();
		InfoScreen_Draw();
		Ren_printf( 8.0f, 9.0f, 0.3f, "FPS: %i", fps );
		Ren_printf( 0.1f, 1.4f, 0.4f, "Velocity: %.1f", speed );
		Ren_printf( 0.1f, 2.0f, 0.4f, "Camera: %i", playerinfo.cameratype );
		Ren_printf( 8.0f, 8.7f, 0.3f, "TimeScale: %.1f", gi.timescale );
		Ren_printf( 8.0f, 8.4f, 0.3f, "Worlds: %.1f", num_worlds );
		Ren_printf( 8.0f, 8.1f, 0.3f, "World Size: %.1f", WORLD_WDTH );
		//G_DrawNetGraph();
		//Ren_printf( 2.0f, 6.0f, 1.0f, "JAXIS X: %i", keystate.axis_x );
		//Ren_printf( 2.0f, 5.0f, 1.0f, "JAXIS Y: %i", keystate.axis_y );
	}

	if( gi.fadein )
	{
		if( G_FadeIn() )
			gi.fadein = False;
	}

}

// Just some nice code to print debugging information
	/*
	//  float distance;
	
	entity_t* ent;
	vec3_t vect, 
	vec3_t zero;
	vec3_t forward, right, up;
	vec3_t stroidlocation;
	*/

	/*
	  VectorSubtract( ship.origin, stroids[1].origin, stroidlocation );
	  VectorSubtract( ship.origin, stroids[1].origin, vect );
	  distance = VectorLength( vect );

	AngleVectors( ship.angles, forward, right, up );
	ZeroVector( zero );
	*/
/*
		  Ren_DrawLine( ship.origin, forward, (10.0f) );
		  Ren_Color( 0.0f, 0.0f, 1.0f, 1.0f );
		  Ren_DrawLine( ship.origin, right, (10.0f) );
		  Ren_Color( 0.0f, 1.0f, 0.0f, 1.0f );
		  Ren_DrawLine( ship.origin, up, (10.0f) );
		  Ren_Color( 1.0f, 1.0f, 1.0f, 1.0f );
		  Ren_DrawLine( zero, stroids[1].origin, (1.0f) );
*/
/*
	  Ren_printf( 10, 280, "Player Z-F Vector: (%f,%f,%f)", forward[0], forward[1], forward[2] );
	  Ren_printf( 10, 260, "Player X-R Vector: (%f,%f,%f)", right[0], right[1], right[2] );
	  Ren_printf( 10, 240, "Player Y-U Vector: (%f,%f,%f)", up[0], up[1], up[2] );
	  Ren_printf( 10, 220, "Player angles PYR: (%f,%f,%f)", ship.angles[PITCH], ship.angles[YAW], ship.angles[ROLL] );
	  Ren_printf( 10, 200, "Vlength: %.2f", distance );
	  Ren_printf( 10, 180, "Player Origin: (%.2f,%.2f,%.2f)", ship.origin[0], ship.origin[1], ship.origin[2] );
	  Ren_printf( 10, 160, "Player2 Origin: (%.2f,%.2f,%.2f)", ship2.origin[0], ship2.origin[1], ship2.origin[2] );
*/




void G_RunFrame( void )
{
		int i;
		float infoscreen_time = 5000;
		static int lastframe;
		int thisframe;
		float input_time = 100;

		thisframe = OS_Milliseconds();

		playerinfo.stroids_left = 0;

		if( G_KeyDown( 'o' ) ) {
			WORLD_WDTH -= 1;
			WORLD_HGHT -= 1;
			WORLD_DPTH -= 1;

			WORLD_WIDTH_MAX		=	( WORLD_WDTH / 2);
			WORLD_WIDTH_MIN		=	(-WORLD_WDTH / 2);
			WORLD_HEIGHT_MAX	=	( WORLD_HGHT / 2);
			WORLD_HEIGHT_MIN	=	(-WORLD_HGHT / 2);
			WORLD_DEPTH_MAX		=	( WORLD_DPTH / 2);
			WORLD_DEPTH_MIN		=	(-WORLD_DPTH / 2);
		}

		if( G_KeyDown( 'p' ) ) {
			WORLD_WDTH += 1;
			WORLD_HGHT += 1;
			WORLD_DPTH += 1;

			WORLD_WIDTH_MAX		=	( WORLD_WDTH / 2);
			WORLD_WIDTH_MIN		=	(-WORLD_WDTH / 2);
			WORLD_HEIGHT_MAX	=	( WORLD_HGHT / 2);
			WORLD_HEIGHT_MIN	=	(-WORLD_HGHT / 2);
			WORLD_DEPTH_MAX		=	( WORLD_DPTH / 2);
			WORLD_DEPTH_MIN		=	(-WORLD_DPTH / 2);
		}

		if( G_KeyDown( 'k' ) ) {
			num_worlds -= 1;
		}

		if( G_KeyDown( 'l' ) ) {
			num_worlds += 1;
		}

		

		if( G_KeyDown( 'j' ) )
			bDrawCollision = !bDrawCollision;

		if( G_KeyDown( K_KP_END ) )
			playerinfo.health = 0;

		if( G_KeyDown( K_KP_DOWNARROW ) )
			Stroids_KillAll();

		if( G_KeyDown( K_ESCAPE ) )
		{
			gi.lastgamestate = gi.gamestate;
			gi.gamestate = GS_MENU;
			gi.menuopen = True;
			gi.paused = True;
			gi.fadein = True;
		}

		if( G_KeyDown( K_KP_PLUS ) )
			gi.timescale += 0.01f;

		if( G_KeyDown( K_KP_MINUS ) )
			gi.timescale -= 0.01f;
	
		if( playerinfo.state == PS_ALIVE )
		{
			sboolean alivestroid;

			if( (thisframe - lastframe) > input_time )
			{
				lastframe = thisframe;

				if( G_KeyDown('c') )
				{
					playerinfo.cameratype++;

					if( playerinfo.cameratype >= 3 )
						playerinfo.cameratype = 0;
				}
			}
			
			Ship_Move();

			if( playerinfo.health <= 0 )
			{
				/* Take away a player life */
				playerinfo.lives--;
				/* Set them as just being killed */
				playerinfo.state = PS_KILLED;
			}

			alivestroid = False;
			/* check is all stroids are dead */
			for( i = 0; i < MAX_STROIDS; i++ )
			{
				if( stroids[i].alive )
				{
					alivestroid = True;
					playerinfo.stroids_left++;
				}
			}

			if( !alivestroid )
			{
				printf( "YOU BEAT THE STROIDS\n" ); 
				playerinfo.level++;
				playerinfo.state = PS_NEXTLEVEL;
			}
		}
		else if( playerinfo.state == PS_KILLED )
		{
			Ship_Move();
		}
		else if( playerinfo.state == PS_DEAD )
		{
			if( (thisframe - lastframe) > infoscreen_time )
			{
				//printf( "(%i,%i)\n", thisframe, lastframe);
				lastframe = thisframe;
				G_PlayerDied();
				if( playerinfo.lives <= 0 )
					gi.gamestate = GS_RESTART;

				playerinfo.state = PS_ALIVE;
			}
		}
		else if( playerinfo.state == PS_NEXTLEVEL )
		{
			if( (thisframe - lastframe) > infoscreen_time )
			{
				lastframe = thisframe;
				G_LoadLevel( playerinfo.level );
			}
		}
		else if( playerinfo.state == PS_DEMOMODE )
		{
			gi.gamestate = GS_RESTART;
		}
		

		/* Move the Stroids */
		Stars_Move();
		Stroids_Move();
		Stations_Move();
		PowerUps_Move();
		Bullets_Move();
		W_BombMove();
		Particles_Move();
		Hud_Move();
		InfoScreen_Move();

	
		//G_UpdateNetwork();
	
}

void G_FirstPersonCam( void )
{
	Ren_CameraSetOrigin( ship.origin );
	Ren_CameraSetAngles( ship.angles );
}

void G_FreeLookCam( void )
{
	vec3_t corigin;
	vec3_t angles;

	VectorCopy( ship.origin, corigin );

	corigin[0] -= 40.0f;

	angles[0] = 0.0f;
	angles[1] = 0.0f;
	angles[2] = 0.0f;

	//AngleVectors( ship.angles, forward, NULL, NULL );
	//VectorNormalize( forward, forward );
	//VectorScale( forward, -10, forward );
	//VectorAdd( forward, corigin, corigin );

	//VectorSubtract( corigin, ship.origin, dist_vec );
	//VectorNormalize( dist_vec, dist_vec );

	//VectorToAngles( dist_vec, angles );

	//printf( "Angles: %f, %f, %f\n", angles[0], angles[1], angles[2] );

	Ren_CameraSetOrigin( corigin );
	Ren_CameraSetAngles( angles );
}

void G_ThirdPersonCam( void )
{
	vec3_t forward, corigin;
	//vec3_t dist_vec, angles;

	VectorCopy( ship.origin, corigin );

	//corigin[0] -= 40.0f;

	AngleVectors( ship.angles, forward, NULL, NULL );
	VectorNormalize( forward, forward );
	VectorScale( forward, -25, forward );
	//forward[2] += 10;
	VectorAdd( forward, corigin, corigin );

	corigin[2] += 10;

	//VectorSubtract( corigin, ship.origin, dist_vec );
	//VectorNormalize( dist_vec, dist_vec );

	//VectorToAngles( dist_vec, angles );

	//printf( "Angles: %f, %f, %f\n", angles[0], angles[1], angles[2] );

	//ZeroVector( angles );

	Ren_CameraSetOrigin( corigin );
	Ren_CameraSetAngles( ship.angles );
}



int	G_Main( void )
{ 
	int t;
	
	t = OS_Milliseconds();
	framelen = (t - T0) / 1000.0f;

	gi.framelen = framelen*gi.timescale;

	switch( gi.gamestate )
	{
		case GS_INIT:
			gi.gamestate = GS_RESTART;		// Just Restart the game
			break;
		case GS_RESTART:
			G_Restart();					// Reset the game
			gi.gamestate = GS_RUNNING;		// Run the game
			break;
		case GS_RUNNING:
			G_RunFrame();
			break;
		case GS_MENU:
			G_RunMenu();
			break;
		case GS_NOTRUNNING:
			G_RunFrame();
			break;
		case GS_INTRO:
			G_RunIntro();
			//G_RunMenu();
			break;
		default:
			printf( "Bad Game State!" );
	}

	G_KeyFrame();

	/* Setup our camera */
	if( gi.gamestate == GS_RUNNING ) {
		switch( playerinfo.cameratype )
		{
		case 0:
			G_FirstPersonCam();
			break;
		case 1:
			G_ThirdPersonCam();
			break;
		case 2:
			G_FreeLookCam();
			break;
		default:
			break;
		}
	} 
	else if( gi.gamestate == GS_MENU )
	{
	}

	/* Render the Scene */
	Ren_Begin();
		G_DrawFrame();
	Ren_End();

	T0 = t;

	// Calculate framerate
	thisframe = (float)OS_Milliseconds();
	ff++; 
	if((thisframe-lastframe) > 1000)
	{
		fps=ff;
		ff=0;
		lastframe = thisframe;
	}

	return(0);
}
