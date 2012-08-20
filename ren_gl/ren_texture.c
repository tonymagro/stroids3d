#include "ren_gl.h"
#include "../scommon/libtarga.h"

#define MAX_TEXTURES	100

//GLuint texture[MAX_TEXTURES];
int	texturecount;

image_t *images[MAX_TEXTURES];

int currentimage = -1;

void Ren_BindImage( int textureid )
{
	if( currentimage == textureid )
		return;

	currentimage = textureid;
	
	if( textureid >= 0 )
		glBindTexture( GL_TEXTURE_2D, textureid );
}

/* Creates an Empty texture */
image_t *Ren_LoadEmptyTexture( void )									
{
	unsigned int* data;											// Stored Data
	image_t *pic;
	int id;

	printf( "Loading Empty Texture\n" );

	// Create Storage Space For Texture Data (128x128x4)
	data = (unsigned int*) mem_malloc( ((128 * 128)* 4 * sizeof(unsigned int)) );
	//ZeroMemory(data,((128 * 128)* 4 * sizeof(unsigned int)));	// Clear Storage Memory
	memset( data, 0, ((128*128) * 4 * sizeof(unsigned int)));

	glGenTextures(1, &id);								// Create 1 Texture
	glBindTexture(GL_TEXTURE_2D, id);					// Bind The Texture
	glTexImage2D(GL_TEXTURE_2D, 0, 4, 128, 128, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, data);						// Build Texture Using Information In data
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	pic = mem_malloc( sizeof( image_t ) );

	strcpy( pic->filename, "EmptyTexture");
	pic->textureid = id;

	images[texturecount] = pic;

	texturecount++;

	mem_free( data );							// Release data

	data = NULL;

	return pic;									// Return The Texture ID
	
}

image_t nullimage = { "nullimage", -1 };
image_t *Ren_FindImage( char *filename )
{
	unsigned char *TextureImage;
	int width, height;
	image_t *pic;
	int i, id;

	pic = NULL;
	TextureImage = NULL;


	printf( "Loading Image: %s\n", filename );
	
	// see if the image is already loaded
	for( i = 0; i < texturecount; i++ )
	{
		if( strcmp( filename, images[i]->filename ) == 0 )
		{
			return images[i];
		}
	}

    TextureImage = (unsigned char*)tga_load( filename, &width, &height, TGA_TrueCOLOR_32 );

	if( !TextureImage )
	{
		printf( "ERROR: Couldn't load texture: %s\n", filename );
		return &nullimage;
	}
	
	glGenTextures( 1, &id );

	glBindTexture( GL_TEXTURE_2D, id );

	gluBuild2DMipmaps( GL_TEXTURE_2D, 4, width, height, GL_RGBA, GL_UNSIGNED_BYTE, TextureImage );

	/*
	glTexImage2D( GL_TEXTURE_2D, 0, 4, width,
					  height, 0, GL_RGBA,
					  GL_UNSIGNED_BYTE, TextureImage );
					  */

	// Linear Filtering 
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		
	pic = mem_malloc( sizeof( image_t ) );

	strcpy( pic->filename, filename);
	pic->textureid = id;

	printf( " - ID => %i\n", pic->textureid );
	printf( " - TC => %i\n", texturecount );

	images[texturecount] = pic;

	mem_free( TextureImage );

	TextureImage = NULL;

	texturecount++;

	return pic;
}

image_t * Ren_LoadImage( char *filename )
{
	return Ren_FindImage( filename );
}

void Ren_ReloadTextures( void )
{
	int i;
	int old_texturecount;
	char filename[128];
	image_t *old_images[MAX_TEXTURES];

	old_texturecount = texturecount;
	texturecount = 0;

	memcpy( &old_images, &images, sizeof( image_t *) * MAX_TEXTURES );

	// see if the image is already loaded
	glDeleteTextures( old_texturecount, 0 );

	for( i = 0; i < old_texturecount; i++ )
	{
		if( images[i] )
		{
			strcpy( filename, images[i]->filename );
			mem_free( images[i] );
			images[i] = NULL;
			printf( "Re" );
			if( strcmp( filename, "EmptyTexture" ) == 0 )
				Ren_LoadEmptyTexture();
			else
				Ren_LoadImage( filename );
		}
	}
}

void Ren_TextureInit( void )
{
	printf( "Initializing Textures\n" );
	texturecount = 0;
}

void Ren_TextureShutdown( void )
{
	int i;

	printf( "Shutting Down Textures\n" );

	for( i = 0; i < texturecount; i++ )
	{
		if( images[i] )
		{
			mem_free( images[i] );
			images[i] = NULL;
		}
	}
}

