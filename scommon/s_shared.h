#ifndef S_SHARED_H
#define S_SHARED_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
//#include <time.h>

#define S_VERSION		"Stroids beta 1.0"
#define	I_VERSION		1.0

/* Endian */
// This does nothing. Endian is determined on the fly. Why keep it? Why not?
//#ifdef MAC 
#define WORDS_BIGENDIAN
//#endif

#define MAX_FILENAME	128
#define MAX_PRINTSTR	512



typedef enum { False, True } sboolean;
typedef unsigned char byte;

#ifdef WIN32
typedef unsigned int  uint;
typedef unsigned short ushort;
#endif
typedef unsigned char ubyte;

#ifndef word
typedef unsigned short word;
#endif // word


#define DEFAULT_SERVER_PORT  2094

#define ERROR_FATAL			1
#define ERROR_DROP			2


// Tokenizing FIXME: Quake 2
#define	MAX_STRING_CHARS	1024	// max length of a string passed to Cmd_TokenizeString
#define	MAX_STRING_TOKENS	80		// max tokens resulting from Cmd_TokenizeString
#define	MAX_TOKEN_CHARS		128		// max length of an individual token

#define	MAX_INFO_KEY		64
#define	MAX_INFO_VALUE		64
#define	MAX_INFO_STRING		512

#define	MAX_GPATH			64


extern int curtime;				// last time returned by OS_Milliseconds

/*----------------------------------
        Endian
------------------------------------*/

sboolean Swap_SysBigEndian( void );
void Swap_Init( void );
int 	Swap_Int32( int value );
short 	Swap_Int16( short value );
float 	Swap_Float32( float value );


void File_ReadInt32( FILE *file,unsigned int *scanline, int length );
void File_ReadInt16( FILE *file,unsigned short *scanline, int length );
void File_ReadFloat32( FILE *file, float *scanline, int length );

#ifdef WORDS_BIGENDIAN
#define SWAP_INT32(A)	Swap_Int32(A)
#define SWAP_INT16(A)	Swap_Int16(A)
#define SWAP_FLOAT32(A)	Swap_Float32(A)
#else
#define SWAP_INT32(A)	(A)
#define SWAP_INT16(A)	(A)
#define SWAP_FLOAT32(A)	(A)
#endif

short	BigShort(short l);
short	LittleShort(short l);
int		BigLong (int l);
int		LittleLong (int l);
float	BigFloat (float l);
float	LittleFloat (float l);
/* ---------------------------------
	Endian
------------------------------------*/

/* ---------------------------------
	Memory
------------------------------------*/
void mem_init();
void mem_shutdown();


#define mem_malloc( size ) \
    (mem_allocate( (size), __LINE__, __FILE__ ))

#define mem_realloc( ptr, size ) \
    (mem_reallocate( (ptr), (size), __LINE__, __FILE__ ))

void*	mem_allocate( unsigned int size, unsigned int line, const char * file );
void*	mem_reallocate( void * ptr, unsigned int size, unsigned int line, const char * file );
void	mem_free( void * ptr );

// attempts to touch memory enough so that the OS will swap it in.
void	mem_swap_in( unsigned int size, unsigned char * buf );

unsigned int mem_malloc_bytes();
unsigned int mem_malloc_number();
/* ---------------------------------
	Memory
------------------------------------*/

/*----------------------------------
	Math
------------------------------------*/
#ifndef M_PI
#define M_PI 3.14159265358979f
#endif

#define PIOVER180 M_PI / 180

#define RAND_NUM(min, max) ((int)(min) + (int)((float)((max) - (min) + 1) * \
		rand() / (float)(RAND_MAX)))
//#define RAND_NUM(a,b) ((a)+(rand()%((b)-(a)+1)))
//#define RAND_NUM(l,h) (((int)(rand() * ((float)(h)-(l)+1))) + (l))

// angle indexes
#define	PITCH				0		// up / down	// Y AXIS
#define	YAW					1		// left / right	// Z AXIS
#define	ROLL				2		// fall over	// X AXIS

// Colors
#define RED		0
#define GREEN	1
#define BLUE	2
#define ALPHA	3

typedef float vec_t;
//typedef vec_t vec2_t[2];
typedef vec_t vec3_t[3];
typedef vec_t vec4_t[4];

typedef struct plane_s
{
	vec3_t normal;
	vec_t  dist; /* distance to origin */
} plane_t;
	
 void	AngleVectors( vec3_t angles, vec3_t forward, vec3_t right, vec3_t up );
 void	VectorScale( const vec3_t vec, float scale, vec3_t out );
 void	VectorMultiply( const vec3_t vec1, const vec3_t vec2, vec3_t vec3 );
 void	VectorSubtract( const vec3_t vec1, const vec3_t vec2, vec3_t vec3 );
 void	VectorAdd( const vec3_t vec1, const vec3_t vec2, vec3_t vec3 );
 void	ZeroVector( vec3_t vec );
 float	VectorLength( const vec3_t source );
 void	VectorMA( const vec3_t f, float s, const vec3_t m, vec3_t result );
 void	VectorNormalize( const vec3_t in, vec3_t out );
 void	VectorCopy( const vec3_t in, vec3_t out );
 void	VectorSwap( vec3_t a, vec3_t b );
 float	DotProduct( const vec3_t a, const vec3_t b );
 void	VectorToAngles( vec3_t vect, vec3_t angles );
 void	VectorInverse( const vec3_t in, vec3_t out );
 void	Matrix_Mult( float in1[16], float in2[16], float out[16] );

 void	VectorRotate( const float in[2], float angle, float out[2] );

 float VectorDistance( const vec3_t a, const vec3_t b );

/*----------------------------------
	Math
------------------------------------*/

 /* ----------------------------------------------------------------------------
	Shared - Quake2
------------------------------------------------------------------------------- */
char* SHR_Parse( char** data );

char* va( const char *format, ... );
char* SHR_StrCpy( char* str );

void Info_RemoveKey (char *s, char *key);
void Info_SetValueForKey (char *s, char *key, char *value);
/* ----------------------------------------------------------------------------
	Shared - Quake2
------------------------------------------------------------------------------- */

 /* ----------------------------------------------------------------------------
	Console Variables - Quake2
------------------------------------------------------------------------------- */

/* Flags */
#define CVAR_SAVE		1
#define CVAR_READONLY	2
#define CVAR_SERVER		4
#define CVAR_USER       8

typedef struct cvar_struct
{
	char*	name;	
	char*	string;
	float	value;
	int		flags;
	sboolean	changed;
	struct cvar_struct*	next;	// Points to next cvar in linked list
} cvar;

void Cvar_Init( void );
/* Add a cvar. This will overwrite Cvar if it exists */
cvar*	Cvar_Add( char* name, char* value, int flags );
/* Returns the Cvar if found and OR's together flags.
Otherwise it creates the cvar.*/
cvar*	Cvar_Get( char* name, char* value, int flags );
cvar*	Cvar_Set(char* name, char* value, sboolean force);
/* Set the value of a Cvar or create it if it doesn't exist */
void	Cvar_SetValue( char *var_name, float value );
sboolean Cvar_IsCommand( void );
char*	Cvar_VariableString( char* name );
void	Cvar_OutPutVariables( char* path );
float	Cvar_GetValue( char* name );

char*	Cvar_BitInfo( int bit );
char*	Cvar_UserInfo( void );
char*	Cvar_ServerInfo( void );
/* ----------------------------------------------------------------------------
	Console Variables - Quake2
------------------------------------------------------------------------------- */

/* ----------------------------------------------------------------------------
	Commands - Quake2
------------------------------------------------------------------------------- */
typedef void (*fCommand) (void);

extern int alias_count;

void	Cmd_Init( void );
void	Cmd_Add( char* name, fCommand function );
void	Cmd_ExecuteString( char *text );

int		Cmd_argc( void );
char*	Cmd_argv( int arg );
char*	Cmd_args( void );
/* ----------------------------------------------------------------------------
	Commands - Quake2
------------------------------------------------------------------------------- */

/* ----------------------------------------------------------------------------
	Command Buffer - Quake2
------------------------------------------------------------------------------- */

void	Cbuf_Init( void );
void	Cbuf_Add( char* text );
void	Cbuf_Insert( char* text );
void	Cbuf_Exec( void );

/* ----------------------------------------------------------------------------
	Command Buffer - Quake2
------------------------------------------------------------------------------- */

/* ----------------------------------------------------------------------------
	Buffers - Quake2
------------------------------------------------------------------------------- */
typedef struct buffer_struct_s
{
	sboolean		allowoverflow;
	sboolean		overflow;
	byte*		data;
	int			maxsize;
	int			currentsize;
	int			readcount;
}buffer_t;

/* This takes in a buffer and initializes it */
void	Buf_Init( buffer_t* buf, byte* data, int length );
void	Buf_Write( buffer_t* buf, void* data, int length );
void	Buf_Clear( buffer_t* buf );
void*	Buf_GetSpace( buffer_t* buf, int length );
/* ----------------------------------------------------------------------------
	Buffers - Quake2
------------------------------------------------------------------------------- */

#endif
