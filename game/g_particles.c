#include "game.h"

void Particles_Init( void )
{
	int i;

	for( i = 0; i < MAX_PARTICLES; i++ )
	{
		particles[i].type             = ET_PARTICLE;
		particles[i].alive			  = False;
		particles[i].u.particle.type  = PT_FADE;
		particles[i].counter1	      = 0;
		particles[i].maxcount1	      = 0;
		particles[i].scale            = 0.4f;
		particles[i].angles[YAW]      = 0;

		ZeroVector( particles[i].velocity );
		ZeroVector( particles[i].origin );
	}
}

void Particles_Spawn( vec3_t origin, vec3_t velocity, vec4_t color, particletype type, float fade, float scale )
{
	int i;

	for( i = 0; i < MAX_PARTICLES; i++ )
	{
		if( !particles[i].alive )
		{
			particles[i].counter2           = fade; // how much to fade
			particles[i].maxcount2          = 1.0f; // starting life
			particles[i].u.particle.type    = type;
			particles[i].scale              = scale;
			particles[i].u.particle.color[0] = color[0];
			particles[i].u.particle.color[1] = color[1];
			particles[i].u.particle.color[2] = color[2];
			particles[i].u.particle.color[3] = color[3];

			VectorCopy( origin, particles[i].origin );
			VectorCopy( velocity, particles[i].velocity );

			particles[i].alive = True;

			return;
		}
	}
}

/*
void Particles_Draw( void )
{
	int i;

	Ren_BindImage( pic_particle->textureid );

	for( i = 0; i < MAX_PARTICLES; i++ )
	{
		if( particles[i].alive )
		{
			Ren_Color( particles[i].u.particle.color.r,
					   particles[i].u.particle.color.g,
					   particles[i].u.particle.color.b,
					   particles[i].maxcount2 );
			Ren_DrawParticle( particles[i].origin, particles[i].scale );
		}
	}

	Ren_Color( 1.0f, 1.0f, 1.0f, 1.0f );
}
*/

void Particles_Move( void )
{
	int i;

	for( i = 0; i < MAX_PARTICLES; i++ )
	{
		if( particles[i].alive )
		{
			G_MoveEntity( &particles[i], gi.framelen );
			particles[i].maxcount2 -= (particles[i].counter2*gi.framelen)*100;

			if( particles[i].maxcount2 < 0 )
			{
				particles[i].alive = False;
			}
		}
	}
}

void G_StartExplosion( vec3_t origin, vec3_t velocity, vec4_t color, particletype type, float fade, float scale, int numparticles )
{
	while( numparticles >= 0 )
	{
		vec3_t angles;
		float scalar;
		vec3_t forward, right, up;

		angles[YAW] = (float)(rand()%359);
		angles[PITCH] = (float)(rand()%359);
		angles[ROLL] = (float)(rand()%359);

		scalar = ((float)rand() / RAND_MAX) * 5.0f;
//		printf( "%f", scalar );

		AngleVectors( angles, forward, right, up );
		VectorNormalize( forward, forward );
		VectorScale( forward, scalar, forward );
		VectorAdd( forward, velocity, forward );

		Particles_Spawn( origin, forward, color, type, fade, scale );

		numparticles--;
	}
}
