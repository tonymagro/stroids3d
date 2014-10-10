#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define GL_BGRA	0x80E1
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include "../game/game.h"

extern plane_t frustum[6];

//
// ren_texture.c
//
void	Ren_TextureInit( void );
void	Ren_TextureShutdown( void );

//
// ren_draw.c
//
void	Ren_DrawInit( void );
void	Ren_DrawShutdown( void );
void 	Ren_DrawChar( float x, float y, float size, int character );
void	Ren_DrawChar3f( float x, float y, float z, int character );
void	Ren_DrawString( float x, float y, float size, char *str );
void	Ren_DrawString3f( float x, float y, float z, char *str );
void	Ren_DrawImage( float x, float y, float width, float height, image_t *pic );
image_t *Ren_FindImage( char *filename );

//
// ren_model.c
//
void	Ren_ModelInit( void );
void	Ren_ModelShutdown( void );

void	Ren_ComputeFrustPlanes( void );
