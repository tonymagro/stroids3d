#include "s_shared.h"


void VectorScale( const vec3_t vec, float scale, vec3_t out ) {
	out[0] = vec[0] * scale;
	out[1] = vec[1] * scale;
	out[2] = vec[2] * scale;
}
void VectorMultiply( const vec3_t vec1, const vec3_t vec2, vec3_t vec3 ) {
	vec3[0] = vec1[0] * vec2[0];
	vec3[1] = vec1[1] * vec2[1];
	vec3[2] = vec1[2] * vec2[2];
}
void VectorSubtract( const vec3_t vec1, const vec3_t vec2, vec3_t vec3 ) {
	vec3[0] = vec1[0] - vec2[0];
	vec3[1] = vec1[1] - vec2[1];
	vec3[2] = vec1[2] - vec2[2];
}
void VectorAdd( const vec3_t vec1, const vec3_t vec2, vec3_t vec3 ) {
	vec3[0] = vec1[0] + vec2[0];
	vec3[1] = vec1[1] + vec2[1];
	vec3[2] = vec1[2] + vec2[2];
}
void AngleVectors( vec3_t angles, vec3_t forward, vec3_t right, vec3_t up ) {

	float		angle;
	static float		sroll, spitch, syaw, croll, cpitch, cyaw;

	angle = angles[YAW] * (M_PI*2 / 360);
	syaw = (float)sin(angle);
	cyaw = (float)cos(angle);
	
	angle = angles[PITCH] * (M_PI*2 / 360);
	spitch = (float)sin(angle);
	cpitch = (float)cos(angle);
	
	angle = angles[ROLL] * (M_PI*2 / 360);
	sroll = (float)sin(angle);
	croll = (float)cos(angle);

	if (forward)
	{
		forward[0] = cpitch*cyaw;
		forward[1] = cpitch*syaw;
		forward[2] = -spitch;
	}
	if (right)
	{
		right[0] = (-1*sroll*spitch*cyaw+-1*croll*-syaw);
		right[1] = (-1*sroll*spitch*syaw+-1*croll*cyaw);
		right[2] = -1*sroll*cpitch;
	}
	if (up)
	{
		up[0] = (croll*spitch*cyaw+-sroll*-syaw);
		up[1] = (croll*spitch*syaw+-sroll*cyaw);
		up[2] = croll*cpitch;
	}
}

void ZeroVector( vec3_t vec ) {
	vec[0] = vec[1] = vec[2] = 0;
}
float VectorLength( const vec3_t v ) {
	return (float)sqrt( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] );
}

float VectorDistance( const vec3_t a, const vec3_t b )
{
	return ((((a)[0] - (b)[0]) * ((a)[0] - (b)[0])) + 
            (((a)[1] - (b)[1]) * ((a)[1] - (b)[1])) + 
            (((a)[2] - (b)[2]) * ((a)[2] - (b)[2])));
}

void VectorMA( const vec3_t f, float s, const vec3_t m, vec3_t result ) {
	result[0] = f[0] + s * m[0];
	result[1] = f[1] + s * m[1];
	result[2] = f[2] + s * m[2];
}

void VectorNormalize( const vec3_t in, vec3_t out ) {
	float len;

	len = VectorLength( in );

	/* check for divide by zero */
	if( len == 0 )
		return;
	
	out[0] = in[0] / len; 
	out[1] = in[1] / len;
	out[2] = in[2] / len;
}
void VectorCopy( const vec3_t in, vec3_t out ) {
	out[0] = in[0];
	out[1] = in[1];
	out[2] = in[2];
}

void VectorInverse( const vec3_t in, vec3_t out ) {
	out[0] = -in[0];
	out[1] = -in[1];
	out[2] = -in[2];
}

void VectorSwap( vec3_t a, vec3_t b ) {
	vec3_t t;
	t[0] = b[0];
	t[1] = b[1];
	t[2] = b[2];
	b[0] = a[0];
	b[1] = a[1];
	b[2] = a[2];
	a[0] = t[0];
	a[1] = t[1];
	a[2] = t[2];
}

/* Oh my god, this is still 2d - April 17, 2003 */ 
void VectorRotate( const float in[2], float angle, float out[2] ) {
	float temp[2];
	printf( "in is %f, %f\n", in[0], in[1] );
	VectorCopy( in, temp );
	/* so we dont fuck up the vec if in = out */
	printf ( "Temp is %f, %f\n", temp[0], temp[1] );
	out[0] = (float) (cos(angle) * temp[0] + sin( angle * PIOVER180 ) * temp[1]);
	out[1] = (float) (cos(angle) * temp[1] - sin( angle * PIOVER180 ) * temp[0]);
	printf ( "Out is %f, %f\n", out[0], out[1] );
}

float DotProduct( const vec3_t a, const vec3_t b ) {
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

void VectorToAngles( vec3_t vect, vec3_t angles )
{
	float v;

	v = (float)sqrt( (vect[0]*vect[0] + vect[1]*vect[1] + vect[2]*vect[2] ) );
	
	angles[2] = (float)acos( vect[0] / v );
	angles[0] = (float)acos( vect[1] / v );
	angles[1] = (float)acos( vect[2] / v );

	angles[2] = -angles[2] * 180 / M_PI;
	angles[0] = -angles[0] * 180 / M_PI;
	angles[1] = -angles[1] * 180 / M_PI;
}

void Matrix_Mult( float in1[16], float in2[16], float out[16] )
{
	int i, j, k;

	for( j = 0; j < 16; j+=4 )
	{
		for( i = 0; i < 4; i++ )
		{
			printf( "[ c%i = ", i+j );
			for( k = 0; k < 4; k++ )
			{
				out[i+j] += in1[k*4+i] * in2[k+j];
				printf ( "(a%i*b%i) + ", k*4+i, k+j );
					
			}

			printf( "]\n" );
		}
	}

	//printf( "\n" );

}
