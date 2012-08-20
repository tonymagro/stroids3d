//#include "game.h"
#include "../ren_gl/ren_gl.h"

image_t	*		pic_empty;						// An Unsigned Int To Store The Texture Number

/* Radial Blur from nehe lesson 36 - Totally hacked in to the menu */

void G_InitLimbo( void )
{
	pic_empty = Ren_LoadEmptyTexture();
}


vec3_t langles = { 0, 0, 0 };

void G_RunLimbo( void )
{

	if( G_KeyDown( K_ESCAPE ) )
	{
		gi.lastgamestate = gi.gamestate;
		gi.gamestate = GS_MENU;
		gi.menuopen = True;
		gi.paused = True;
	}

	langles[YAW] += 10.0f*gi.framelen;
	langles[PITCH] += 10.0f*gi.framelen;
	
	Ren_CameraSetAngles( langles );

}

void ViewOrtho()												// Set Up An Ortho View
{
	glMatrixMode(GL_PROJECTION);								// Select Projection
	glPushMatrix();												// Push The Matrix
	glLoadIdentity();											// Reset The Matrix
	glOrtho( 0, 640 , 480 , 0, -1, 1 );							// Select Ortho Mode (640x480)
	glMatrixMode(GL_MODELVIEW);									// Select Modelview Matrix
	glPushMatrix();												// Push The Matrix
	glLoadIdentity();											// Reset The Matrix
}

void ViewPerspective()											// Set Up A Perspective View
{
	glMatrixMode( GL_PROJECTION );								// Select Projection
	glPopMatrix();												// Pop The Matrix
	glMatrixMode( GL_MODELVIEW );								// Select Modelview
	glPopMatrix();												// Pop The Matrix
}

void G_DrawLimbo( void )
{
	float times = 50;
	float inc = 0.09f;
	float spost = 0.0f;											// Starting Texture Coordinate Offset
	float alphainc = 0.9f / times;								// Fade Speed For Alpha Blending
	float alpha = 0.2f;						// Starting Alpha Value

	int num;

	//ViewPerspective();

	glViewport(0,0,128,128);					// Set Our Viewport (Match Texture Size)


	Ren_BindImage( pic_empty->textureid );		// Bind To The Blur Texture

	// Copy Our ViewPort To The Blur Texture (From 0,0 To 128,128... No Border)
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, 0, 0, 128, 128, 0);

	glClearColor(0.0f, 0.0f, 0.5f, 0.5);				// Set The Clear Color To Medium Blue
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear The Screen And Depth Buffer

	glViewport(0 , 0, vidinfo.width , vidinfo.height);					// Set Viewport (0,0 to 640x480)


	/*Draw The blur */

	// Disable AutoTexture Coordinates
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);

	glEnable(GL_TEXTURE_2D);									// Enable 2D Texture Mapping
	glDisable(GL_DEPTH_TEST);									// Disable Depth Testing
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);							// Set Blending Mode
	//glEnable(GL_BLEND);											// Enable Blending
	Ren_BindImage( pic_empty->textureid ); 					// Bind To The Blur Texture
	ViewOrtho();												// Switch To An Ortho View

	alphainc = alpha / times;									// alphainc=0.2f / Times To Render Blur

	glBegin(GL_QUADS);											// Begin Drawing Quads
		for (num = 0;num < times;num++)						// Number Of Times To Render Blur
		{
			glColor4f(1.0f, 1.0f, 1.0f, alpha);					// Set The Alpha Value (Starts At 0.2)
			glTexCoord2f(0+spost,1-spost);						// Texture Coordinate	( 0, 1 )
			glVertex2f(0,0);									// First Vertex		(   0,   0 )

			glTexCoord2f(0+spost,0+spost);						// Texture Coordinate	( 0, 0 )
			glVertex2f(0,480);									// Second Vertex	(   0, 480 )

			glTexCoord2f(1-spost,0+spost);						// Texture Coordinate	( 1, 0 )
			glVertex2f(640,480);								// Third Vertex		( 640, 480 )

			glTexCoord2f(1-spost,1-spost);						// Texture Coordinate	( 1, 1 )
			glVertex2f(640,0);									// Fourth Vertex	( 640,   0 )

			spost += inc;										// Gradually Increase spost (Zooming Closer To Texture Center)
			alpha = alpha - alphainc;							// Gradually Decrease alpha (Gradually Fading Image Out)
		}
	glEnd();													// Done Drawing Quads

	ViewPerspective();											// Switch To A Perspective View

	glEnable(GL_DEPTH_TEST);									// Enable Depth Testing
	//glDisable(GL_TEXTURE_2D);									// Disable 2D Texture Mapping
	//glDisable(GL_BLEND);										// Disable Blending
	glBindTexture(GL_TEXTURE_2D,0);								// Unbind The Blur Texture

}
