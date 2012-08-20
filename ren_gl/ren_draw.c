#include "ren_gl.h"

image_t *charfont;

float charsize = 0.9f;
float printsize = 0.7f;
float printsizex = 0.4f;

/* Sphere Drawing */
GLint slices, stacks;
GLUquadricObj *quadObj;
int sphereh = 0;

void Ren_DrawInit( void )
{
	printf( "Initializing Draw\n" );
	charfont = Ren_LoadImage("data/font1.tga");
}

void Ren_DrawShutdown( void )
{
	printf( "Shutting Down Draw\n" );
	if( sphereh == 1 )
		gluDeleteQuadric( quadObj );
}

void Ren_DrawString( float x, float y, float scale, char *str )
{
	while( *str )
	{
		Ren_DrawChar( x, y, scale, *str );
		x += printsizex * scale;
		str++;
	}
}

void Ren_DrawString3f( float x, float y, float z, char *str )
{
	while( *str )
	{
		Ren_DrawChar3f( x, y, z, *str );
		x += charsize;
		str++;
	}
}

void Ren_DrawChar( float x, float y, float scale, int character )
{
	int		row, col;
	float	frow, fcol, colsize, rowsize;
	float psize, psizex;

	psize  = printsize * scale;
	psizex = printsizex * scale;

	if( character == 32 )
		return;
	
	//glEnable( GL_BLEND );
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	row = character/32;
	col = character%32;

	//printf( "Row: %i, Col: %i\n", row, col );

	rowsize = 1.0f/4.0f;
	colsize = 1.0f/32.0f;

	frow = row*rowsize;
	fcol = col*colsize;

	Ren_BindImage( charfont->textureid );

	glBegin(GL_QUADS);
		glTexCoord2f( fcol, -(frow + rowsize) );
		glVertex2f( x, y );
		glTexCoord2f( fcol + colsize, -(frow + rowsize) );
		glVertex2f( x+psizex, y );
		glTexCoord2f( fcol + colsize, -frow );
		glVertex2f( x+psizex, y+psize );
		glTexCoord2f( fcol, -frow );
		glVertex2f( x, y+psize );
	glEnd();
	
}

void Ren_DrawChar3f( float x, float y, float z, int character )
{
	int		row, col;
	float	frow, fcol, colsize, rowsize;

	if( character == 32 )
		return;
	
	//glEnable( GL_BLEND );
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	row = character/32;
	col = character%32;

	//printf( "Row: %i, Col: %i\n", row, col );

	rowsize = 1.0f/4.0f;
	colsize = 1.0f/32.0f;

	frow = row*rowsize;
	fcol = col*colsize;

	Ren_BindImage( charfont->textureid );

	glPushMatrix();

	glDepthMask( GL_FALSE );		// no z buffering
	//glEnable( GL_BLEND );

	glTranslatef( x, y, z );
//	glRotatef( viewangles[YAW], 0.0f, 0.0f, 1.0f );
//	glRotatef( viewangles[PITCH], 0.0f, 1.0f, 0.0f );
//	glRotatef( viewangles[ROLL], 1.0f, 0.0f, 0.0f );

	glBegin(GL_QUADS);
		glTexCoord2f( fcol, -(frow + rowsize) );
		glVertex3f( 0.0f, 0.0f, 0.0f );
		glTexCoord2f( fcol + colsize, -(frow + rowsize) );
		glVertex3f( charsize, 0.0f, 0.0f );
		glTexCoord2f( fcol + colsize, -frow );
		glVertex3f( charsize, charsize, 0.0f );
		glTexCoord2f( fcol, -frow );
		glVertex3f( 0.0f, charsize, 0.0f );
	glEnd();

	glDepthMask( GL_TRUE );		// no z buffering
	//glDisable( GL_BLEND );

	glPopMatrix();
}

void Ren_DrawImage( float x, float y, float width, float height, image_t *pic )
{
	Ren_BindImage( pic->textureid );

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBegin (GL_QUADS);
		glTexCoord2f(0, -1);
		glVertex2f(x, y);
		glTexCoord2f(1, -1);
		glVertex2f(x+width, y);
		glTexCoord2f(1, -0);
		glVertex2f(x+width, y+height);
		glTexCoord2f(0, -0);
		glVertex2f(x, y+height);
	glEnd ();
}

void Ren_DrawRectangle( float x, float y, float width, float height )
{
	glBegin(GL_QUADS);
		glTexCoord2i(0, -1);
		glVertex2f( x, y );
		glTexCoord2i(1, -1);
		glVertex2f(x+width, y);
		glTexCoord2i(1, -0);
		glVertex2f( x+width, y+height );
		glTexCoord2i(0, -0);
		glVertex2f( x, y+height );
	glEnd ();

}

void Ren_DrawSphereList( void )
{
		//glColor3f(1.0, 0.0, 1.0);
		gluSphere(quadObj, 1.0, slices, stacks);
}

int Ren_CreateSphere( int sides, int type, int mode )
{
	int hand = glGenLists(1);
	quadObj = gluNewQuadric();
	slices = 7;
	stacks = 7;
	glNewList( hand, mode );
	Ren_DrawSphereList();
	glEndList();
	return hand;
}

void Ren_DrawSphere( vec3_t origin, float radius )
{
	//glDisable( GL_TEXTURE_2D);
	//glDisable( GL_CULL_FACE );
	//glEnable( GL_BLEND );
	//glDisable( GL_DEPTH_TEST );
	//glBlendFunc( GL_SRC_ALPHA, GL_DST_ALPHA );
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glPointSize(radius);
	//glColor4f( 0.0f, 0.2f, 1.0f, 0.6f );
	//glBegin(GL_POINTS);
	//glVertex3f(origin[0],origin[1],origin[2]);
	//glEnd();
	glPushMatrix();
	glTranslatef( origin[0], origin[1], origin[2] );
	glScalef( radius, radius, radius );
	if (!sphereh) 
		sphereh = Ren_CreateSphere(20,GL_POLYGON,GL_COMPILE_AND_EXECUTE);
	else 
		glCallList(sphereh);
	glPopMatrix();

	//glEnable( GL_TEXTURE_2D );
	//glEnable( GL_CULL_FACE );
	//glEnable( GL_DEPTH_TEST );
}
