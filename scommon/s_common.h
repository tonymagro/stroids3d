#ifndef S_COMMON_H
#define S_COMMON_H

#include "s_shared.h"

/* This should be done differently */
#define DEFAULT_VIEWPORT_WIDTH	1280
#define DEFAULT_VIEWPORT_HEIGHT	1024
#define DEFAULT_VIEWPORT_BPP	32
#define DEFAULT_VIEWPORT_FULLSCREEN 0

#define DEFAULT_VIDMODE		1

#define JOYSTICK_SUPPORT	0	// only disables use of joystick
#define NETWORK_SUPPORT		0 	// Compiles with or without  network support

#define VIEWPORT_FOV		60.0f
#define VIEWPORT_NEAR		1.0f
#define VIEWPORT_FAR		WORLD_WDTH * 5

#define CULLING_SLOP		5.0f
#define QUICK_WORLD_CULLING 1

#define DRAW_WORLD_BOXEN	1
#define DRAW_COORD_AXES		1

/* ---------------------------------
	Main Functions
------------------------------------*/
void	Main_Init( void );
void	Main_Shutdown( void );
void	Main_Frame( int msec );
/* ---------------------------------
	Main Functions
------------------------------------*/

/* ---------------------------------
	OS Specific
------------------------------------*/
void	OS_Quit( int exitcode );
void	OS_Error( int type, char * message );
int		OS_Milliseconds( void );
void	OS_SwapBuffers( void );
void	OS_ShowCursor( sboolean show );
void	OS_GetMousePosition( int *x, int *y );
void	OS_SetVidMode( int mode, sboolean fullscreen );

#define VID_MODES 5

typedef struct vid_mode_s
{
	char name[128];
	int  width;
	int  height;
	int  bpp;
} vid_mode_t;
extern vid_mode_t	vidmode[VID_MODES];

typedef struct vid_info_t
{
	int width;
	int height;
	int bpp;
	sboolean fullscreen;
	int mode;
} vid_info_t;

extern vid_info_t vidinfo;
/* ---------------------------------
	OS Specific
------------------------------------*/

/* ---------------------------------
	Entities
------------------------------------*/
typedef enum { ET_STROID,
			   ET_BULLET,
			   ET_STATION,
			   ET_BOMB,
			   ET_SHIP,
			   ET_POWERUP,
			   ET_STAR,
			   ET_PARTICLE,
               ET_NULL } entity_type;

typedef enum { PT_FADE, PT_BURST } particletype;
typedef struct particle_s
{
	particletype type;
	vec4_t	color;
} particle_t;

typedef enum { PU_AMMO, PU_ARMOR, PU_ENERGY } poweruptype;
typedef struct powerup_s
{
	poweruptype type;
} powerup_t;

typedef struct entity_s
{
	entity_type     type;
	union
	{
		particle_t  particle;
		powerup_t   powerup;
	}               u;
	vec3_t			origin;
	vec3_t			velocity;
	
	float			radius;			// for collision detection
	float			scale;
	float			speed;
	vec3_t			angles;			// rotation angles YAW PITCH ROLL
	float			maxVelocity;
	sboolean		alive;
	float			counter1;		// general purpose counters
	float			counter2;
	float			maxcount1;
	float			maxcount2;
	int				flag1;			// general purpose flags
	int				flag2;

	//float			alpha;			// transparency of the entitiy
	vec4_t			color;			// color of this entity

	float			hitdamage;		// how much damage a hit from this item does
	struct entity_s* owner;			// Owner of this entity

	/* Control the entity with these function pointers - FIXME: Not yet implemented */
	void		(*think) ( struct entity_s *self);
	void		(*draw) ( struct entity_s *self, vec3_t origin );
	void		(*damage) ( struct entity_s *self, struct entity_s *attacker, float shotpower ,int damage);
	void		(*kill) ( struct entity_s *self, struct entity_s *attacker, struct entity_s *object, int damage );

	struct entity_s* next;
} entity_t;
/* ---------------------------------
	Entities
------------------------------------*/

/* ---------------------------------
	Renderer
------------------------------------*/

#define REN_OVERLAY	1

/* Image */
typedef struct image_s
{
	char filename[MAX_FILENAME];
	int	 textureid;
} image_t;

/* Model */
typedef struct model_s
{
	char filename[MAX_FILENAME];
	int	 modelid;
	image_t* texture;
} model_t;



void	Ren_Init( void );
void	Ren_Shutdown( void );
int		Ren_WindowSize( int width, int height );

void	Ren_Begin( void );
void	Ren_End( void );

void	Ren_Color( float r, float g, float b, float fade );

image_t	*Ren_LoadImage( char *filename );
void	Ren_ReloadTextures( void );
void	Ren_BindImage( int textureid );
void	Ren_SetGL2D (void);

void	Ren_SetPolyMode( sboolean wire );

void	Ren_DrawEntity( entity_t *ent, int flags );
void	Ren_DrawRectangle( float x, float y, float width, float height );
void	Ren_DrawLine( vec3_t start, vec3_t finish, float scale );
void	Ren_DrawImage( float x, float y, float width, float height, image_t *pic );
void	Ren_DrawParticle( vec3_t origin, float scale );

void	Ren_DrawSphere( vec3_t origin, float radius );

void	Ren_DrawImage3d( vec3_t origin, float scale );
void	Ren_DrawImage3dPlane( vec3_t origin, int axis1, int axis2, float scale );

model_t *Ren_LoadModel( const char *filename );
void	Ren_DrawModel( int modelid, entity_t *ent );
void	Ren_DrawModelLocation( int modelid, entity_t *ent, vec3_t location );
void	Ren_Draw2DModel( float x, float y, vec3_t rotation, int modelid );

void	Ren_DrawChar( float x, float y, float scale, int character );
void	Ren_print3f( vec3_t coord, char* str, ... );
void 	Ren_printf( float x, float y, float size, char* str, ... );

void	Ren_DrawWorldBox( vec3_t off, vec3_t color );
void	Ren_DrawAxis( void );

void	Ren_DrawFade( float alpha );

image_t *Ren_LoadEmptyTexture( void );

void	Ren_CreateSkyBox( char * filename );
void	Ren_DrawSky( void );

/* NEW CAMERA FUNCTIONS - USE THESE */
void	Ren_CameraSetOrigin( vec3_t neworigin );
void	Ren_CameraSetAngles( vec3_t newangles );

void	Ren_CameraRoll( float * roll );
void	Ren_CameraPitch( float * pitch );
void	Ren_CameraYaw( float *yaw );
/* ---------------------------------
	Renderer
------------------------------------*/
/* ---------------------------------
	Input
------------------------------------*/
void Input_Init( void );
void Input_Shutdown( void );
void Input_Frame( void );
/* ---------------------------------
	Input
------------------------------------*/

/* ---------------------------------
	Sound
------------------------------------*/
typedef struct sound_s
{
	char filename[MAX_FILENAME];
	int soundid;
} sound_t;

void Snd_Init( void );
void Snd_Shutdown( void );
sound_t *Snd_LoadWAV( char *filename );

/* Play a sound once */
void Snd_PlaySound( int soundid );
void Snd_PlaySoundLoop( int soundid, int loop );

/* ---------------------------------
	Sound
------------------------------------*/

/* ----------------------------------------------------------------------------
	File System - Quake 2
------------------------------------------------------------------------------- */
/* Init File System */
void	Fsys_Init( void );
/* Shutdown File System */
void	Fys_Shutdown( void );

/* Set the Game Path */
void	Fsys_SetGameDir( char* dir );
/* Get Game Path */
char*	Fsys_GetGameDir( void );

int		Fsys_LoadFile( char* path, void** buffer_t );
void	Fsys_FreeFile( void* buffer_t );
int		Fsys_OpenFile( char* filename, FILE **file );
void	Fsys_Read( void* buffer_t, int len, FILE* f );
int		Fsys_FileLength( FILE* f );
/* ----------------------------------------------------------------------------
	File System - Quake 2
------------------------------------------------------------------------------- */

#endif
