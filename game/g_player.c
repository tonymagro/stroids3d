#include "game.h"
/* ------------------------------
	SHIP
---------------------------------*/

model_t		*mod_ship;

vec4_t	enginecolor;
vec4_t	fuelcolor;
vec4_t	bullettrail = { 0.0f, 0.3f, 1.0f, 1.0f };



void Ship_Move( void )
{
	int i;
	static int lastframe, timer1_l, timer2_last;
	int thisframe, timer1_t, timer2_this;
	vec3_t origin, forward, right, up, randvec;
	vec3_t tempvec;
	//static int counter = 0;

	thisframe = timer1_t = timer2_this = OS_Milliseconds();

	// FIXME: Disable input
	if( playerinfo.state == PS_KILLED )
	{
		//G_DisableInputFrame();
	}

	//FIXME: Correct input
	if( playerinfo.energy <= 0.004 )
	{
		//keystate.jetting = 0;
		//keystate.shield	 = 0;
	}

	if( G_KeyDown( 'e' ) )
	{
		playerinfo.weapon_selection++;

		/* FIXME: Don't hard code */
		if( playerinfo.weapon_selection > 1 )
			playerinfo.weapon_selection = 0;
	}

	if( G_KeyDown( K_MWHEELUP ) )
	{
		playerinfo.weapon_selection++;

		/* FIXME: Don't hard code */
		if( playerinfo.weapon_selection > 1 )
			playerinfo.weapon_selection = 0;
	}else if( G_KeyDown( K_MWHEELDOWN ) )
	{
		playerinfo.weapon_selection--;

		/* FIXME: Don't hard code */
		if( playerinfo.weapon_selection < 0 )
			playerinfo.weapon_selection = 1;
	}


	/* Do keyboard input */
	G_RotateEntity( &ship, PITCH, playerinfo.mouse_sense*playerinfo.mouse_dy*gi.framelen );
	G_RotateEntity( &ship, YAW, playerinfo.mouse_sense*playerinfo.mouse_dx*gi.framelen*-1 );

	if( gi.joystick )
	{
		if( (playerinfo.joy_dx > 10000) || (playerinfo.joy_dx < -10000) )
			G_RotateEntity( &ship, YAW, playerinfo.joy_sense*playerinfo.joy_dx*gi.framelen*-1 );
		
		if( (playerinfo.joy_dy > 10000) || (playerinfo.joy_dy < -10000) )
			G_RotateEntity( &ship, PITCH, playerinfo.joy_sense*playerinfo.joy_dy*gi.framelen );
	}


	/* Get the ship's angle vectors */
	AngleVectors( ship.angles, forward, right, up );

	if( G_KeyDown('a') )
	{
		VectorScale( right, -1.0f, tempvec );
		VectorAdd( ship.velocity, tempvec, ship.velocity );
		G_AccelEntity( &ship, gi.framelen*10.0f );
	}
			
	if( G_KeyDown('d') )
	{
		//VectorScale( right, ship.speed*gi.framelen, tempvec );
		VectorAdd( ship.velocity, right, ship.velocity );
		G_AccelEntity( &ship, gi.framelen*10.0f );
	}


	if ( G_KeyDown('w') || G_KeyDown( K_JOY2 ) || G_KeyDown('s') )
	{
		if( G_KeyDown('w') || G_KeyDown(K_JOY2) )
			G_AccelEntity( &ship, gi.framelen*30.0f );
		else
			G_AccelEntity( &ship, -gi.framelen*30.0f );

		ZeroVector( forward );
		ZeroVector( right );
		ZeroVector( up );

		AngleVectors( ship.angles, forward, right, up );

		/* Create Engine Particles */
		randvec[0] = ((float)rand() / RAND_MAX) * 5.0f - 2.0f;
		randvec[1] = ((float)rand() / RAND_MAX) * 5.0f - 2.0f;
		randvec[2] = ((float)rand() / RAND_MAX) * 5.0f - 2.0f;

		//VectorNormalize( forward, forward );
		VectorScale( forward, 20.0f, forward );
		VectorSubtract( forward, randvec, forward );
		VectorSubtract( ship.velocity, forward, forward );

		if( (timer2_this - timer2_last) > 10 ) {
			timer2_last = timer2_this;
			Particles_Spawn( ship.origin, forward, enginecolor, PT_FADE, 0.02f, 1.5f );
		}
		//Particles_Spawn( ship.origin, forward, &fuelcolor, PT_FADE, 0.09f, 1.5f );

		if( playerinfo.energy > 0.002 )
			playerinfo.energy -= 0.002f;
	}
	
	G_MoveEntity( &ship, gi.framelen );

	G_WorldWrap( &ship );
	
	/*
	if( playerinfo.energy <= 0.04 )
		keystate.fire = 0;
	*/

	if( G_KeyDown(K_SPACE) || G_KeyDown( K_MOUSE1 ) || G_KeyDown(K_JOY1) )
	{
		/* Bullet */
		if( playerinfo.weapon_selection == 0 ) {
			if( (thisframe - lastframe) > (playerinfo.rate_of_fire) )
			{
				lastframe = thisframe;

				Snd_PlaySound( snd_laser1->soundid );

				ZeroVector( forward );
				ZeroVector( right );
				ZeroVector( up );
				ZeroVector( origin );

				AngleVectors( ship.angles, forward, right, up );
				//VectorNormalize( forward, right );
				VectorScale( forward, 150.0f, forward );
				/* Get the new velocity */
				VectorAdd( ship.velocity, forward, forward );

				Bullets_Fire( ship.origin, forward, bullettrail, &ship, 6.0f );

				if( playerinfo.energy > 0.04 )
					playerinfo.energy -= 0.04f;
			}
		}
		else { /* Bomb */
			if( (thisframe - lastframe) > (playerinfo.rate_of_fire*10) )
			{
				lastframe = thisframe;

				//Snd_PlaySound( snd_laser1->soundid );

				ZeroVector( forward );
				ZeroVector( right );
				ZeroVector( up );
				ZeroVector( origin );

				AngleVectors( ship.angles, forward, right, up );
				VectorScale( forward, 100.0f, forward );
				VectorAdd( ship.velocity, forward, forward );

				W_BombFire( ship.origin, forward, 0 );

				if( playerinfo.energy > 0.04 )
					playerinfo.energy -= 0.04f;
			}
		}
	}

	/*
	if( G_KeyDown('b') || G_KeyDown(K_MOUSE2) || G_KeyDown(K_JOY3) )
	{
	}
	*/

	
	/*
	if( keystate.shield )
	{
		playerinfo.shieldon = True;
		VectorCopy( ship.origin, shield.origin );

		if( playerinfo.energy > 0.007 )
			playerinfo.energy -= 0.007f;

	}
	else
	{
		playerinfo.shieldon = False;
	}
	*/

	ZeroVector( forward );

	/* Check for collisions with rocks */
	for( i = 0; i < MAX_STROIDS; i++ )
	{
		if( stroids[i].alive )
		{
			if( G_Collision( &ship, &stroids[i] ) )
			{
				Snd_PlaySound( snd_explode1->soundid );

				if( !playerinfo.shieldon )
					playerinfo.health -= (int)(playerinfo.stroiddamage*stroids[i].scale);
				
				G_StartExplosion( stroids[i].origin, forward, enginecolor, PT_FADE, 0.01f, 2.0f, 15 );
				G_StartExplosion( ship.origin, forward, fuelcolor, PT_FADE, 0.01f, 2.0f, 15 );
				stroids[i].alive = False;
			}
		}
	}

	/* Check for collisions with powerups */
	for( i = 0; i < MAX_POWERUPS; i++ )
	{
		if( powerups[i].alive )
		{
			if( G_Collision( &ship, &powerups[i] ) )
			{
				playerinfo.health += 20;
				powerups[i].alive = False;
			}
		}
	}

	
	/* Simulate fuel leak */
	if( playerinfo.health < (playerinfo.maxHealth*0.5f) )
	{
		ZeroVector( forward );
		ZeroVector( right );
		ZeroVector( randvec );
		
		/* Create Engine Particles */
		randvec[0] = ((float)rand() / RAND_MAX) * 5.0f - 2.0f;
		randvec[1] = ((float)rand() / RAND_MAX) * 5.0f - 2.0f;

		VectorAdd( ship.velocity, randvec, forward );
		VectorScale( forward, 0.2f, forward );

		//Particles_Spawn( ship.origin, forward, &fuelcolor, PT_FADE, 0.006f, 1.0f );	
	}
	
	/* replenish energy */
	if( playerinfo.energy < playerinfo.maxEnergy )
	{
		if( playerinfo.health < playerinfo.maxHealth*0.5 )
			playerinfo.energy += 0.0005f;
		else
			playerinfo.energy += 0.001f;
	}

	/* The ship has just died */
	if( playerinfo.state == PS_KILLED )
	{
		ZeroVector( forward );
		ZeroVector( ship.velocity );

		//VectorScale( ship.velocity, 0.5f, forward );

		/* spin the player out of control */
		ship.angles[YAW] += gi.framelen*100.0f;
		ship.angles[PITCH] += gi.framelen*100.0f;

		G_StartExplosion( ship.origin, forward, enginecolor, PT_FADE, 0.00004f, 1.6f, 30 );

		if( (timer1_t - timer1_l) > 5000 )
		{
			timer1_l = timer1_t;
			playerinfo.state = PS_DEAD;
		}
	}

	/* Cheater! */
	playerinfo.energy = 1.0f;

	
}

void Ship_Draw( entity_t *ent, vec3_t origin )
{
	//Ren_DrawModelLocation( mod_ship->modelid, ent, origin );
	Ren_DrawModel( mod_ship->modelid, ent );
}

void Ship_Init( void )
{
	//vec3_t origin;

	ship.type = ET_SHIP;
	ship.radius = 3.5f;
	ship.scale = 1.0f;
	ship.maxVelocity = 75.0f;
	ship.counter1 = 0.0f;
	ship.counter2 = 0.0f;
	ship.maxcount1 = 0.0f;
	ship.maxcount2 = 0.0f;

	ship.angles[YAW] = 0.0f;
	ship.angles[ROLL] = 0.0f;
	ship.angles[PITCH] = 0.0f;

	ship.alive = True;
	ship.speed = 20.0f;

	ZeroVector( ship.origin );
	ZeroVector( ship.velocity );

	shield.radius = 1.3f;
	shield.scale = 1.7f;
	shield.angles[YAW] = 0;
	shield.angles[PITCH] = 0;
	shield.angles[ROLL] = 0;
	shield.alive = False;
	
	ZeroVector( shield.origin );
	ZeroVector( shield.velocity );

	/* create the engine color */
	enginecolor[RED] = 1.0f;
	enginecolor[GREEN] = 0.5f;
	enginecolor[BLUE] = 0.2f;
	enginecolor[ALPHA] = 1.0f;

	/* create shield and plasma color */
	fuelcolor[RED]		= 0.0f;
	fuelcolor[GREEN]	= 1.0f;
	fuelcolor[BLUE]		= 0.0f;
	fuelcolor[ALPHA]	= 1.0f;

	G_Spawn( &ship, ship.origin, ship.radius, ship.scale );

	playerinfo.mouse_sense = 30;
	playerinfo.joy_sense   = 0.003f;

	playerinfo.cameratype = 0;
	playerinfo.weapon_selection = 0;

	ship.draw = Ship_Draw;

	// Load the ship's model
	mod_ship		= Ren_LoadModel( "data/models/ship.ms3d" );
	
}


/* ------------------------------
	SHIP
---------------------------------*/

