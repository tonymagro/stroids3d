#include "ren_gl.h"

#define EPSILON 0.5f

int viewtype = 0;

//
// view origin
//
vec3_t	c_up;
vec3_t	c_view;
vec3_t	c_right;
vec3_t	c_origin;
vec3_t	c_angles;

/* This is for dynamic world creation */
float num_worlds = 1.0f;

float WORLD_WDTH = 1000.0f;
float WORLD_HGHT = 1000.0f;
float WORLD_DPTH = 1000.0f;

float WORLD_WIDTH_MAX;
float WORLD_WIDTH_MIN;
float WORLD_HEIGHT_MAX;
float WORLD_HEIGHT_MIN;
float WORLD_DEPTH_MAX;
float WORLD_DEPTH_MIN;


// Fog
vec3_t fog = { 0.5f, 0.5f, 0.5f };
float fog_color[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
unsigned int fogMode[] = { GL_EXP, GL_EXP2, GL_LINEAR };
unsigned int fogfilter = 0;

/* Hardware Lighting
float light_ambient[]= { 1.0f, 1.0f, 1.0f, 1.0f };
float light_diffuse[]= { 1.0f, 1.0f, 1.0f, 1.0f };
float light_specular[]= { 1.0f, 1.0f, 1.0f, 1.0f };
float light_position[]= { 0.0f, 0.0f, 2.0f, 0.0f };

float lmodel_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
*/

/////// Lighting variables
float ambientLight[] = { 0.4f, 0.4f, 0.4f, 1.0f };	// ambient light
float diffuseLight[] = { 0.25f, 0.25f, 0.25f, 1.0f };	// diffuse light
float lightPosition[] = { 0.0f, 0.0f, 1.0f, 0.0f };	// the light position

////// Material variables
float matAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
float matDiff[] = { 1.0f, 1.0f, 1.0f, 1.0f};

/*
void Ren_DrawCircle( int sides, int type )
{
	int i;
	float angle;
	glBegin(type);
	for (i=0; i < sides; i++) {
		angle = 2.0*M_PI*i/sides;
		glVertex2f(cosf(angle),sinf(angle));
	}
	glEnd();
	
}
*/

void Ren_printf( float x, float y, float size, char* str, ... )
{
	va_list		args;
	char		msg[MAX_PRINTSTR];

	va_start( args,str );
	vsprintf( msg, str, args );
	va_end( args );

	Ren_DrawString( x, y, size, msg );
	
}

void Ren_print3f( vec3_t coord, char* str, ... )
{
	va_list		args;
	char		msg[MAX_PRINTSTR];

	va_start( args,str );
	vsprintf( msg, str, args );
	va_end( args );

	Ren_DrawString3f( coord[0], coord[1], coord[2], msg );
	
}


void Ren_SetGL2D(void)
{
	glViewport( 0, 0, vidinfo.width, vidinfo.height );
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity ();
	glOrtho( 0.0f, 10.0f, 0.0f, 10.0f, -500, 500 );
	//glOrtho( -1, 1, -1, 1, -1, 1 );
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	glDisable(GL_DEPTH_TEST);
	glDisable( GL_LIGHTING );
//	glDisable(GL_CULL_FACE);
//	glDisable(GL_BLEND);
//	glEnable(GL_ALPHA_TEST);
	glColor4f(1,1,1,1);
}

void Ren_SetGL3D( void )
{

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix
	gluPerspective(VIEWPORT_FOV,  vidinfo.width / vidinfo.height,
				   VIEWPORT_NEAR, WORLD_WDTH);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

static int skyboxlist;
static image_t *skyboxtexture;

	// Create the sky cube list:
float skypoints[8][3] = 
{
	{-1.0f, -1.0f, -1.0f},	//0
	{1.0f, -1.0f, -1.0f},	//1
	{1.0f, 1.0f, -1.0f},	//2
	{-1.0f, 1.0f, -1.0f},	//3
	{-1.0f, -1.0f, 1.0f},	//4
	{1.0f, -1.0f, 1.0f},	//5
	{1.0f, 1.0f, 1.0f},		//6
	{-1.0f, 1.0f, 1.0f},	//7
};

/* Create a Skybox for the level. Right now I have a display list and a normal function
 * that does this. I am testing if there are performance gains with using display lists
 */
void Ren_CreateSkyBox( char * filename )
{

	// Load in skybox image
	skyboxtexture = Ren_LoadImage( filename );

	skyboxlist = glGenLists(1);
	glNewList(skyboxlist, GL_COMPILE);
		glEnable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glDisable(GL_FOG);
		glDepthMask( GL_FALSE );
		glColor3f(1.0f, 1.0f, 1.0f);
		glScalef(100.0f, 100.0f, 100.0f);

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, skypoints);

		glBindTexture(GL_TEXTURE_2D, skyboxtexture->textureid );
		glBegin(GL_QUADS);
		
			//Floor face
			glTexCoord2f(0.0f, 0.0f); glArrayElement(4);
			glTexCoord2f(1.0f, 0.0f); glArrayElement(5);
			glTexCoord2f(1.0f, -1.0f); glArrayElement(6);
			glTexCoord2f(0.0f, -1.0f); glArrayElement(7);

			//Sky face
			glTexCoord2f(0.0f, -1.0f); glArrayElement(0);
			glTexCoord2f(0.0f, 0.0f); glArrayElement(3);
			glTexCoord2f(1.0f, 0.0f); glArrayElement(2);
			glTexCoord2f(1.0f, -1.0f); glArrayElement(1);

			//Left face
			glTexCoord2f(1.0f, 0.0f); glArrayElement(0);
			glTexCoord2f(1.0f, -1.0f); glArrayElement(4);
			glTexCoord2f(0.0f, -1.0f); glArrayElement(7);
			glTexCoord2f(0.0f, 0.0f); glArrayElement(3);

			//Front face
			glTexCoord2f(0.0f, 0.0f); glArrayElement(0);
			glTexCoord2f(1.0f, 0.0f); glArrayElement(1);
			glTexCoord2f(1.0f, -1.0f); glArrayElement(5);
			glTexCoord2f(0.0f, -1.0f); glArrayElement(4);

			//Right face
			glTexCoord2f(0.0f, 0.0f); glArrayElement(1);
			glTexCoord2f(1.0f, 0.0f); glArrayElement(2);
			glTexCoord2f(1.0f, -1.0f); glArrayElement(6);
			glTexCoord2f(0.0f, -1.0f); glArrayElement(5);
	
			//Back face
			glTexCoord2f(1.0f, 0.0f); glArrayElement(3);
			glTexCoord2f(1.0f, -1.0f); glArrayElement(7);
			glTexCoord2f(0.0f, -1.0f); glArrayElement(6);
			glTexCoord2f(0.0f, 0.0f); glArrayElement(2);

		glEnd();
		
		glDisableClientState(GL_VERTEX_ARRAY);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glEnable( GL_LIGHTING );
		glEnable( GL_BLEND );
		glDepthMask( GL_TRUE );
	glEndList();
}

void Ren_DrawSkyBox( void )
{
		glEnable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glDisable(GL_FOG);
		glColor3f(1.0f, 1.0f, 1.0f);
		glScalef(100.0f, 100.0f, 100.0f);

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, skypoints);

		// Remember: this is a direct call to glBindTexture, not Ren_BindImage
		glBindTexture(GL_TEXTURE_2D, skyboxtexture->textureid );
		glBegin(GL_QUADS);
		
			//Floor face
			glTexCoord2f(0.0f, 0.0f); glArrayElement(4);
			glTexCoord2f(1.0f, 0.0f); glArrayElement(5);
			glTexCoord2f(1.0f, -1.0f); glArrayElement(6);
			glTexCoord2f(0.0f, -1.0f); glArrayElement(7);

			//Sky face
			glTexCoord2f(0.0f, -1.0f); glArrayElement(0);
			glTexCoord2f(0.0f, 0.0f); glArrayElement(3);
			glTexCoord2f(1.0f, 0.0f); glArrayElement(2);
			glTexCoord2f(1.0f, -1.0f); glArrayElement(1);

			//Left face
			glTexCoord2f(1.0f, 0.0f); glArrayElement(0);
			glTexCoord2f(1.0f, -1.0f); glArrayElement(4);
			glTexCoord2f(0.0f, -1.0f); glArrayElement(7);
			glTexCoord2f(0.0f, 0.0f); glArrayElement(3);

			//Front face
			glTexCoord2f(0.0f, 0.0f); glArrayElement(0);
			glTexCoord2f(1.0f, 0.0f); glArrayElement(1);
			glTexCoord2f(1.0f, -1.0f); glArrayElement(5);
			glTexCoord2f(0.0f, -1.0f); glArrayElement(4);

			//Right face
			glTexCoord2f(0.0f, 0.0f); glArrayElement(1);
			glTexCoord2f(1.0f, 0.0f); glArrayElement(2);
			glTexCoord2f(1.0f, -1.0f); glArrayElement(6);
			glTexCoord2f(0.0f, -1.0f); glArrayElement(5);
	
			//Back face
			glTexCoord2f(1.0f, 0.0f); glArrayElement(3);
			glTexCoord2f(1.0f, -1.0f); glArrayElement(7);
			glTexCoord2f(0.0f, -1.0f); glArrayElement(6);
			glTexCoord2f(0.0f, 0.0f); glArrayElement(2);

		glEnd();
		
		glDisableClientState(GL_VERTEX_ARRAY);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glEnable( GL_LIGHTING );
		glEnable( GL_BLEND );

}


/* function to reset our viewport after a window resize */
int Ren_WindowSize( int width, int height )
{
	GLfloat ratio;

	if (height==0)										// Prevent A Divide By Zero By
	{
		height=1;										// Making Height Equal One
	}

	ratio = ( GLfloat )width / ( GLfloat )height;

	glViewport(0,0,width,height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	gluPerspective(VIEWPORT_FOV, ratio, 1.0f, WORLD_WDTH/2);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix

    return( True );
}

void Ren_MoveToEnt( entity_t *ent )
{
	glTranslatef( ent->origin[0], ent->origin[1], ent->origin[2] );
}

void Ren_Color( float r, float g, float b, float fade )
{
	glColor4f( r, g, b, fade );
}

void Ren_DrawImage3dPlane( vec3_t origin, int axis1, int axis2, float scale )
{
	glPushMatrix();

	//glEnable( GL_TEXTURE_2D );
	glDisable( GL_CULL_FACE );
	//glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_DST_ALPHA );
	glDisable( GL_LIGHTING );
	//glDisable( GL_DEPTH_TEST );
	//glBlendFunc( GL_SRC_ALPHA, GL_DST_ALPHA );

	glDepthMask( GL_FALSE );		// no z buffering

	glTranslatef( origin[0], origin[1], origin[2] );

	glBegin( GL_POLYGON );
			glTexCoord2f( 0.0f, -1.0f ); glVertex3f( -1.0f*scale, -1.0f*scale, 0.0f );
			glTexCoord2f( 1.0f, -1.0f ); glVertex3f( 1.0f*scale, -1.0f*scale, 0.0f );
			glTexCoord2f( 1.0f, -0.0f ); glVertex3f( 1.0f*scale, 1.0f*scale, 0.0f );
			glTexCoord2f( 0.0f, -0.0f ); glVertex3f( -1.0f*scale, 1.0f*scale, 0.0f );
	glEnd();

	//glEnable( GL_TEXTURE_2D );
	glEnable( GL_CULL_FACE );
	glEnable( GL_LIGHTING );
	//glDisable( GL_TEXTURE_2D );
	//glEnable( GL_DEPTH_TEST );

	glPopMatrix();
}

void Ren_DrawImage3d( vec3_t origin, float scale )
{
	vec3_t up, right;
	vec3_t point;

	float frame_height, frame_width, frame_origin_x, frame_origin_y;

	frame_height = scale;
	frame_origin_y = scale/2;
	frame_origin_x = scale/2;
	frame_width = scale;


	//glEnable( GL_TEXTURE_2D );
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glPushMatrix();

	glDepthMask( GL_FALSE );		// no z buffering
	//glEnable( GL_BLEND );

	VectorCopy( c_up, up);
	VectorCopy( c_right, right);

	glBegin (GL_QUADS);
		glTexCoord2f (0, -1);
		VectorMA (origin, -frame_origin_y, up, point);
		VectorMA (point, -frame_origin_x, right, point);
		glVertex3fv (point);

		glTexCoord2f (0, 0);
		VectorMA (origin, frame_height - frame_origin_y, up, point);
		VectorMA (point, -frame_origin_x, right, point);
		glVertex3fv (point);

		glTexCoord2f (1, 0);
		VectorMA (origin, frame_height - frame_origin_y, up, point);
		VectorMA (point, frame_width - frame_origin_x, right, point);
		glVertex3fv (point);

		glTexCoord2f (1, -1);
		VectorMA (origin, -frame_origin_y, up, point);
		VectorMA (point, frame_width - frame_origin_x, right, point);
		glVertex3fv (point);
	glEnd ();

	//glDisable( GL_BLEND );
	glDepthMask( GL_TRUE );
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	//glDisable( GL_TEXTURE_2D );

	glPopMatrix();
}

void Ren_DrawEntity( entity_t *ent, int flags )
{
	vec3_t up, right;
	vec3_t point;

	float frame_height, frame_width, frame_origin_x, frame_origin_y;

	frame_height = ent->scale;
	frame_origin_y = ent->scale/2;
	frame_origin_x = ent->scale/2;
	frame_width = ent->scale;

	if( flags & REN_OVERLAY )
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		//glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	}

	glPushMatrix();

	glDisable(GL_CULL_FACE);

	glDepthMask( GL_FALSE );		// no z buffering
	//glEnable( GL_BLEND );

	//glDisable( GL_TEXTURE_2D );
	//glDisable( GL_BLEND );

	//VectorScale( c_up, ent->scale, up);
	//VectorScale( c_right, ent->scale, right);
	VectorCopy( c_up, up);
	VectorCopy( c_right, right);

	glBegin (GL_QUADS);
		glTexCoord2f (0, -1);
		VectorMA (ent->origin, -frame_origin_y, up, point);
		VectorMA (point, -frame_origin_x, right, point);
		glVertex3fv (point);

		glTexCoord2f (0, 0);
		VectorMA (ent->origin, frame_height - frame_origin_y, up, point);
		VectorMA (point, -frame_origin_x, right, point);
		glVertex3fv (point);

		glTexCoord2f (1, 0);
		VectorMA (ent->origin, frame_height - frame_origin_y, up, point);
		VectorMA (point, frame_width - frame_origin_x, right, point);
		glVertex3fv (point);

		glTexCoord2f (1, -1);
		VectorMA (ent->origin, -frame_origin_y, up, point);
		VectorMA (point, frame_width - frame_origin_x, right, point);
		glVertex3fv (point);
	glEnd ();
	
	/* THis works well for particles so particles should be drawn differently *
	glBegin( GL_TRIANGLES );
		glTexCoord2f( 0.0625, 0.0625 );
		glVertex3f( ent->origin[0], ent->origin[1], ent->origin );

		glTexCoord2f( 1.0625, 0.0625 );
		glVertex3f( ent->origin[0] + up[0]*ent->scale, 
						 ent->origin[1] + up[1]*ent->scale, 
						 ent->origin[2] + up[2]*ent->scale);

		glTexCoord2f( 0.0625, 1.0625 );
		glVertex3f( ent->origin[0] + right[0]*ent->scale, 
						 ent->origin[1] + right[1]*ent->scale, 
						 ent->origin[2] + right[2]*ent->scale);
	glEnd ();
	
	

	
		glBegin( GL_POLYGON );
			glTexCoord2f( 0.0f, -1.0f ); glVertex3f( -1.0f* ent->scale, -1.0f* ent->scale, 0.0f );
			glTexCoord2f( 1.0f, -1.0f ); glVertex3f( 1.0f* ent->scale, -1.0f* ent->scale, 0.0f );
			glTexCoord2f( 1.0f, -0.0f ); glVertex3f( 1.0f* ent->scale, 1.0f* ent->scale, 0.0f );
			glTexCoord2f( 0.0f, -0.0f ); glVertex3f( -1.0f* ent->scale, 1.0f* ent->scale, 0.0f );
		glEnd();
	*/

	//glDisable( GL_BLEND );
		//glColor4f( 1,1,1,1 );
	glDepthMask( GL_TRUE );		// back to normal Z buffering

	glEnable(GL_CULL_FACE);

	glPopMatrix();

		//glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

void Ren_DrawParticle( vec3_t porigin, float scale )
{
	// DrawImage3d is for drawing images that must face the camera
	// Particles rotation doesn't matter becuase they look the same
	// no matter how they are rotated. DrawParticles doesn't have to do
	// the extra calculation and there are alot of particles on the screen
	// at one time so this function should be optimized
	Ren_DrawImage3d( porigin, scale );
	
	/*
	vec3_t up, right, origin;
	float offset;

	offset = scale/2;

	VectorCopy( porigin, origin );
	
	origin[0] += offset;
	origin[1] += offset;
	origin[2] += offset;

	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	VectorScale( c_up, scale, up);
	VectorScale( c_right, scale, right);

	//VectorSubtract( porigin, up, origin );
	//VectorSubtract( origin, right, origin );

	glPushMatrix();

	glDepthMask( GL_FALSE );		// no z buffering
	glEnable( GL_BLEND );

	glTranslatef( origin[0], origin[1], origin[2] );

	//glDisable( GL_TEXTURE_2D );
	//glDisable( GL_BLEND );

	//glPolygonMode(GL_FRONT, GL_LINE);
	//glPolygonMode(GL_BACK, GL_LINE);

	*/

	/*
	glBegin( GL_TRIANGLE_STRIP );
	
		glTexCoord2f(1.0f,-1.0f); 
		glVertex3f( origin[0] - right[0]*scale/2 - up[0]*scale/2,
					origin[1] - right[1]*scale/2 - up[1]*scale/2, 
					origin[2] - right[2]*scale/2 - up[2]*scale/2);
		glTexCoord2f(0.0f,-1.0f); 
		glVertex3f( origin[0] + up[0]*scale, 
					origin[1] + up[1]*scale, 
					origin[2] + up[2]*scale);
		glTexCoord2f(0.0f,0.0f);  
		glVertex3f( origin[0] + right[0]*scale, 
					origin[1] + right[1]*scale, 
					origin[2] + right[2]*scale);
		glTexCoord2f(1.0f,0.0f);  
		glVertex3f( origin[0] + right[0]*scale + up[0]*scale,
					origin[1] + right[1]*scale + up[1]*scale, 
					origin[2] + right[2]*scale + up[2]*scale );
		glTexCoord2d(1,1); glVertex3f(up[0]*scale+right[0]*scale,
							up[1]*scale+right[1]*scale, 
							up[2]*scale+right[2]*scale); // Top Right
		glTexCoord2d(0,1); 	glVertex3f( up[0]*scale, 
						 up[1]*scale, 
						 up[2]*scale);
		glTexCoord2d(1,0); 	glVertex3f( right[0]*scale, 
						 right[1]*scale, 
						 right[2]*scale);
		glTexCoord2d(0,0); glVertex3f( 0.0f, 0.0f, 0.0f);

	glEnd();
	*/





	/*
	glBegin( GL_TRIANGLES );
		glTexCoord2f( 0.0, 0.0 );
		glVertex3f( origin[0], origin[1], origin[2] );

		glTexCoord2f( 1.0, 0.0 );
		glVertex3f( origin[0] + up[0]*scale, 
						 origin[1] + up[1]*scale, 
						 origin[2] + up[2]*scale);

		glTexCoord2f( 0.0, 1.0 );
		glVertex3f( origin[0] + right[0]*scale, 
						 origin[1] + right[1]*scale, 
						 origin[2] + right[2]*scale);
	glEnd ();
	

	glPopMatrix();

	glDisable( GL_BLEND );
	glDepthMask( GL_TRUE );
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	*/
	
}

void Ren_DrawLine( vec3_t start, vec3_t finish, float scale )
{
	glPushMatrix();
	glDisable( GL_TEXTURE_2D );

	glTranslatef( start[0], start[1], start[2] );

	glBegin( GL_LINES );
		//glColor4f( 1.0f, 0.0f, 0.0f, 1.0f);
		glVertex3f( 0.0f, 0.0, 0.0f );
		glVertex3f( finish[0]*scale, finish[1]*scale, finish[2]*scale );
    glEnd();

	glPopMatrix();

	glEnable( GL_TEXTURE_2D );

	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
}


void Ren_DrawFade( float alpha )
{
	Ren_Color( 0.0f, 0.0f, 0.0f, alpha );
	Ren_DrawImage( 0, 0, 10, 10, pic_block );
}



void Ren_ComputeFrustPlanes( void )
{
	/* Ripped off from Mark Morley's code due to laziness */

	float   proj[16];
	float   modl[16];
	float   clip[16];
	float   t;

	/* Get the current PROJECTION matrix from OpenGL */
	glGetFloatv( GL_PROJECTION_MATRIX, proj );

	/* Get the current MODELVIEW matrix from OpenGL */
	glGetFloatv( GL_MODELVIEW_MATRIX, modl );

	/* Combine the two matrices (multiply projection by modelview) */
	clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
	clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
	clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
	clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

	clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
	clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
	clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
	clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

	clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
	clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
	clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
	clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

	clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
	clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
	clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
	clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];

	/* Extract the numbers for the RIGHT plane */
	frustum[0].normal[0] = clip[ 3] - clip[ 0];
	frustum[0].normal[1] = clip[ 7] - clip[ 4];
	frustum[0].normal[2] = clip[11] - clip[ 8];
	frustum[0].dist = clip[15] - clip[12];

	/* Normalize the result */
	t = (float)sqrt( frustum[0].normal[0] * frustum[0].normal[0] +
			  frustum[0].normal[1] * frustum[0].normal[1] +
			  frustum[0].normal[2] * frustum[0].normal[2] );
	frustum[0].normal[0] /= t;
	frustum[0].normal[1] /= t;
	frustum[0].normal[2] /= t;
	frustum[0].dist /= t;

	/* Extract the numbers for the LEFT plane */
	frustum[1].normal[0] = clip[ 3] + clip[ 0];
	frustum[1].normal[1] = clip[ 7] + clip[ 4];
	frustum[1].normal[2] = clip[11] + clip[ 8];
	frustum[1].dist = clip[15] + clip[12];

	/* Normalize the result */
	t = (float)sqrt( frustum[1].normal[0] * frustum[1].normal[0] +
			  frustum[1].normal[1] * frustum[1].normal[1] +
			  frustum[1].normal[2] * frustum[1].normal[2] );
	frustum[1].normal[0] /= t;
	frustum[1].normal[1] /= t;
	frustum[1].normal[2] /= t;
	frustum[1].dist /= t;

	/* Extract the BOTTOM plane */
	frustum[2].normal[0] = clip[ 3] + clip[ 1];
	frustum[2].normal[1] = clip[ 7] + clip[ 5];
	frustum[2].normal[2] = clip[11] + clip[ 9];
	frustum[2].dist = clip[15] + clip[13];

	/* Normalize the result */
	t = (float)sqrt( frustum[2].normal[0] * frustum[2].normal[0] +
			  frustum[2].normal[1] * frustum[2].normal[1] +
			  frustum[2].normal[2] * frustum[2].normal[2] );
	frustum[2].normal[0] /= t;
	frustum[2].normal[1] /= t;
	frustum[2].normal[2] /= t;
	frustum[2].dist /= t;

	/* Extract the TOP plane */
	frustum[3].normal[0] = clip[ 3] - clip[ 1];
	frustum[3].normal[1] = clip[ 7] - clip[ 5];
	frustum[3].normal[2] = clip[11] - clip[ 9];
	frustum[3].dist = clip[15] - clip[13];

	/* Normalize the result */
	t = (float)sqrt( frustum[3].normal[0] * frustum[3].normal[0] +
			  frustum[3].normal[1] * frustum[3].normal[1] +
			  frustum[3].normal[2] * frustum[3].normal[2] );
	frustum[3].normal[0] /= t;
	frustum[3].normal[1] /= t;
	frustum[3].normal[2] /= t;
	frustum[3].dist /= t;

	/* Extract the FAR plane */
	frustum[4].normal[0] = clip[ 3] - clip[ 2];
	frustum[4].normal[1] = clip[ 7] - clip[ 6];
	frustum[4].normal[2] = clip[11] - clip[10];
	frustum[4].dist = clip[15] - clip[14];

	/* Normalize the result */
	t = (float)sqrt( frustum[4].normal[0] * frustum[4].normal[0] +
			  frustum[4].normal[1] * frustum[4].normal[1] +
			  frustum[4].normal[2] * frustum[4].normal[2] );
	frustum[4].normal[0] /= t;
	frustum[4].normal[1] /= t;
	frustum[4].normal[2] /= t;
	frustum[4].dist /= t;

	/* Extract the NEAR plane */
	frustum[5].normal[0] = clip[ 3] + clip[ 2];
	frustum[5].normal[1] = clip[ 7] + clip[ 6];
	frustum[5].normal[2] = clip[11] + clip[10];
	frustum[5].dist = clip[15] + clip[14];

	/* Normalize the result */
	t = (float)sqrt( frustum[5].normal[0] * frustum[5].normal[0] +
			  frustum[5].normal[1] * frustum[5].normal[1] +
			  frustum[5].normal[2] * frustum[5].normal[2] );
	frustum[5].normal[0] /= t;
	frustum[5].normal[1] /= t;
	frustum[5].normal[2] /= t;
	frustum[5].dist /= t;
}

void Ren_DrawWorldBox( vec3_t off, vec3_t color )
{
	glPushMatrix();

	glDisable( GL_TEXTURE_2D );
	glDisable( GL_LIGHTING );

	glColor3f( color[0], color[1], color[2] );

    glBegin( GL_LINES );
        glVertex3f( WORLD_WIDTH_MIN  + off[0] + EPSILON,
					WORLD_HEIGHT_MIN + off[1] + EPSILON,
					WORLD_DEPTH_MIN  + off[2] + EPSILON );
        glVertex3f( WORLD_WIDTH_MIN  + off[0] + EPSILON,
					WORLD_HEIGHT_MIN + off[1] + EPSILON,
					WORLD_DEPTH_MAX  + off[2] - EPSILON );

        glVertex3f( WORLD_WIDTH_MIN  + off[0] + EPSILON,
					WORLD_HEIGHT_MIN + off[1] + EPSILON,
					WORLD_DEPTH_MIN  + off[2] + EPSILON );
        glVertex3f( WORLD_WIDTH_MIN  + off[0] + EPSILON,
					WORLD_HEIGHT_MAX + off[1] - EPSILON,
					WORLD_DEPTH_MIN  + off[2] + EPSILON );

        glVertex3f( WORLD_WIDTH_MIN  + off[0] + EPSILON,
					WORLD_HEIGHT_MIN + off[1] + EPSILON,
					WORLD_DEPTH_MIN  + off[2] + EPSILON );
        glVertex3f( WORLD_WIDTH_MAX  + off[0] - EPSILON,
					WORLD_HEIGHT_MIN + off[1] + EPSILON,
					WORLD_DEPTH_MIN  + off[2] + EPSILON );

        glVertex3f( WORLD_WIDTH_MAX  + off[0] - EPSILON,
					WORLD_HEIGHT_MAX + off[1] - EPSILON,
					WORLD_DEPTH_MAX  + off[2] - EPSILON );
        glVertex3f( WORLD_WIDTH_MAX  + off[0] - EPSILON,
					WORLD_HEIGHT_MAX + off[1] - EPSILON,
					WORLD_DEPTH_MIN  + off[2] + EPSILON );

        glVertex3f( WORLD_WIDTH_MAX  + off[0] - EPSILON,
					WORLD_HEIGHT_MAX + off[1] - EPSILON,
					WORLD_DEPTH_MAX  + off[2] - EPSILON );
        glVertex3f( WORLD_WIDTH_MAX  + off[0] - EPSILON,
					WORLD_HEIGHT_MIN + off[1] + EPSILON,
					WORLD_DEPTH_MAX  + off[2] - EPSILON );

        glVertex3f( WORLD_WIDTH_MAX  + off[0] - EPSILON,
					WORLD_HEIGHT_MAX + off[1] - EPSILON,
					WORLD_DEPTH_MAX  + off[2] - EPSILON );
        glVertex3f( WORLD_WIDTH_MIN  + off[0] + EPSILON,
					WORLD_HEIGHT_MAX + off[1] - EPSILON,
					WORLD_DEPTH_MAX  + off[2] - EPSILON );

        
        glVertex3f( WORLD_WIDTH_MIN  + off[0] + EPSILON,
					WORLD_HEIGHT_MAX + off[1] - EPSILON,
					WORLD_DEPTH_MIN  + off[2] + EPSILON );
        glVertex3f( WORLD_WIDTH_MIN  + off[0] + EPSILON,
					WORLD_HEIGHT_MAX + off[1] - EPSILON,
					WORLD_DEPTH_MAX  + off[2] - EPSILON );

        glVertex3f( WORLD_WIDTH_MIN  + off[0] + EPSILON,
					WORLD_HEIGHT_MAX + off[1] - EPSILON,
					WORLD_DEPTH_MAX  + off[2] - EPSILON );
        glVertex3f( WORLD_WIDTH_MIN  + off[0] + EPSILON,
					WORLD_HEIGHT_MIN + off[1] + EPSILON,
					WORLD_DEPTH_MAX  + off[2] - EPSILON );

        glVertex3f( WORLD_WIDTH_MIN  + off[0] + EPSILON,
					WORLD_HEIGHT_MIN + off[1] + EPSILON,
					WORLD_DEPTH_MAX  + off[2] - EPSILON );
        glVertex3f( WORLD_WIDTH_MAX  + off[0] - EPSILON,
					WORLD_HEIGHT_MIN + off[1] + EPSILON,
					WORLD_DEPTH_MAX  + off[2] - EPSILON );

        glVertex3f( WORLD_WIDTH_MAX  + off[0] - EPSILON,
					WORLD_HEIGHT_MIN + off[1] + EPSILON,
					WORLD_DEPTH_MAX  + off[2] - EPSILON );
        glVertex3f( WORLD_WIDTH_MAX  + off[0] - EPSILON,
					WORLD_HEIGHT_MIN + off[1] + EPSILON,
					WORLD_DEPTH_MIN  + off[2] + EPSILON );

        glVertex3f( WORLD_WIDTH_MAX  + off[0] - EPSILON,
					WORLD_HEIGHT_MIN + off[1] + EPSILON,
					WORLD_DEPTH_MIN  + off[2] + EPSILON );
        glVertex3f( WORLD_WIDTH_MAX  + off[0] - EPSILON,
					WORLD_HEIGHT_MAX + off[1] - EPSILON,
					WORLD_DEPTH_MIN  + off[2] + EPSILON );

        glVertex3f( WORLD_WIDTH_MAX  + off[0] - EPSILON,
					WORLD_HEIGHT_MAX + off[1] - EPSILON,
					WORLD_DEPTH_MIN  + off[2] + EPSILON );
        glVertex3f( WORLD_WIDTH_MIN  + off[0] + EPSILON,
					WORLD_HEIGHT_MAX + off[1] - EPSILON,
					WORLD_DEPTH_MIN  + off[2] + EPSILON );
    glEnd();

	glColor3f( 1.0f, 1.0f, 1.0f );

    glEnable( GL_TEXTURE_2D );
	glEnable( GL_LIGHTING );

	glPopMatrix();
}

void Ren_DrawAxis( void )
{
	vec3_t printaxis;

	glPushMatrix();
	glTranslatef( 0.0f, 0.0f, 0.0f );

	glDisable( GL_TEXTURE_2D );
	glDisable( GL_LIGHTING );

	glBegin( GL_LINES );
    glColor4f( 1.0f, 0.0f, 0.0f, 1.0f);
    glVertex3f( 0.0f, 0.0, 0.0f );
    glVertex3f( 8.0, 0.0, 0.0f );
    glColor4f( 0.0f, 1.0f, 0.0f, 1.0f );
    glVertex3f( 0.0f, 0.0, 0.0f );
    glVertex3f( 0.0f, 2*8.0, 0.0f );
    glColor4f( 0.0f, 0.0f, 1.0f, 1.0f );
    glVertex3f( 0.0f, 0.0, 0.0f );
    glVertex3f( 0.0f, 0.0, 9*8.0 );
    glEnd();

	glEnable( GL_TEXTURE_2D );
	
	printaxis[0] = 8.0f;
	printaxis[1] = 0.0f;
	printaxis[2] = 0.0f;
	Ren_print3f( printaxis, "X axis" );

	printaxis[0] = 0.0f;
	printaxis[1] = 8.0f;
	printaxis[2] = 0.0f;
	Ren_print3f( printaxis, "Y axis" );

	printaxis[0] = 0.0f;
	printaxis[1] = 0.0f;
	printaxis[2] = 8.0f;
	Ren_print3f( printaxis, "Z axis" );

	glPopMatrix();

	glEnable( GL_TEXTURE_2D );
	glDisable( GL_LIGHTING );

	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
}

void Ren_DrawSky( void )
{
	glPushMatrix();
	glTranslatef( c_origin[0], c_origin[1], c_origin[2] );
	glCallList(skyboxlist);
	glPopMatrix();

}

void Ren_Begin( void )
{
	float   translation[16];
	float	rotation[16];

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix
	gluPerspective(VIEWPORT_FOV,  vidinfo.width / vidinfo.height,
				   VIEWPORT_NEAR, WORLD_WDTH*num_worlds);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//glEnable(GL_LIGHTING);
	glEnable( GL_DEPTH_TEST );

	/* Calculate the new viewangles */
	AngleVectors( c_angles, c_view, c_right, c_up );


	if( viewtype == 0 ) {
	
		glRotatef (-90,  1, 0, 0);	    // put Z going up
		glRotatef (90,  0, 0, 1);	    // put Z going up
		
		glRotatef (-c_angles[ROLL],   1, 0, 0);
		glRotatef (-c_angles[PITCH],  0, 1, 0);
		glRotatef (-c_angles[YAW],    0, 0, 1);

		// Draw the SkyBox
		//glPushMatrix();
		//glCallList(skyboxlist);
		//Ren_DrawSkyBox();
		//glPopMatrix();


		glTranslatef( -c_origin[0], -c_origin[1], -c_origin[2] );

	}
	else
	{
		/* Get the current MODELVIEW matrix from OpenGL */
		//glGetFloatv( GL_MODELVIEW_MATRIX, modl );

		glRotatef (-90,  1, 0, 0);	    // put Z going up
		glRotatef (90,  0, 0, 1);	    // put Z going up

		/* Calculate rotation matrix */
		rotation[1] = -c_right[0];
		rotation[5] = -c_right[1];
		rotation[9] = -c_right[2];

		rotation[2] = c_up[0];
		rotation[6] = c_up[1];
		rotation[10] = c_up[2];

		rotation[0] = c_view[0];
		rotation[4] = c_view[1];
		rotation[8] = c_view[2];

		rotation[12]	= 0;
		rotation[13]	= 0;
		rotation[14]	= 0;
		rotation[3]		= 0;
		rotation[7]		= 0;
		rotation[11]	= 0;

		rotation[15]	= 1;

		/* Calculate translation matrix */
		translation[0]	= 1;
		translation[1]	= 0;
		translation[2]	= 0;
		translation[3]	= 0;
		translation[4]	= 0;
		translation[5]	= 1;
		translation[6]	= 0;
		translation[7]	= 0;
		translation[8]	= 0;
		translation[9]	= 0;
		translation[10]	= 1;
		translation[11]	= 0;
		translation[12]	= -c_origin[0];
		translation[13]	= -c_origin[1];
		translation[14]	= -c_origin[2];
		translation[15]	= 1;

		glLoadMatrixf( rotation );
		glMultMatrixf( translation );

		// Draw the SkyBox
		glPushMatrix();
		glCallList(skyboxlist);
		//Ren_DrawSkyBox();
		glPopMatrix();


		
		//glRotatef (-90,  1, 0, 0);	    // put Z going up
		//glRotatef (90,  0, 0, 1);	    // put Z going up
		
		//glRotatef (-c_angles[2],  1, 0, 0);
		//glRotatef (-c_angles[0],  0, 1, 0);
		//glRotatef (-c_angles[1],  0, 0, 1);
		//glRotatef (-c_angles[2],  1, 0, 0);
		
		//glRotatef( -c_angles[YAW], c_up[2], c_up[0], c_up[1] );
		//glRotatef( -c_angles[PITCH], 0, 1, 0 );
	
	


		// Draw the SkyBox
		/*
		glPushMatrix();
		glCallList(skyboxlist);
		//Ren_DrawSkyBox();
		glPopMatrix();
		*/


		//glTranslatef( -c_origin[0], -c_origin[1], -c_origin[2] );
		
		/*
		gluLookAt( c_origin[0], c_origin[1], c_origin[2],
			c_origin[0] + c_view[0], c_origin[1] + c_view[1], c_origin[2] + c_view[2],
			c_up[0], c_up[1], c_up[2] );

		// Draw the SkyBox
		glPushMatrix();
		glCallList(skyboxlist);
		//Ren_DrawSkyBox();
		glPopMatrix();
		*/
			
	}

	Ren_ComputeFrustPlanes();
}

void Ren_End( void )
{
	/* Draw it to the screen */
    OS_SwapBuffers( );
}

void Ren_Init( void )
{
	
	printf( "Initializing Renderer\n" );

	WORLD_WIDTH_MAX		=	( WORLD_WDTH / 2);
	WORLD_WIDTH_MIN		=	(-WORLD_WDTH / 2);
	WORLD_HEIGHT_MAX	=	( WORLD_HGHT / 2);
	WORLD_HEIGHT_MIN	=	(-WORLD_HGHT / 2);
	WORLD_DEPTH_MAX		=	( WORLD_DPTH / 2);
	WORLD_DEPTH_MIN		=	(-WORLD_DPTH / 2);
											
	glShadeModel(GL_SMOOTH);				// Smooth shading model										
	//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);	// set the clear color	
	glClearColor( fog_color[0], fog_color[0], fog_color[2], fog_color[3] );	// set the clear color to fog color
	glClearDepth(1.0f);						// Specifies the depth value used when the depth buffer is cleared																						
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glFrontFace(GL_CCW);					// counter clock-wise polygons are out
	glEnable(GL_TEXTURE_2D);				// enable 2d textures

	// Setup Alpha Blending
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glEnable( GL_BLEND );

	// Enable Depth Testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);					// Specifies the depth-comparison function

	// Enable Cull Facing so the backs of polygons are not drawn
	glEnable(GL_CULL_FACE);

	// Let Colors effect how a material looks
	glEnable( GL_COLOR_MATERIAL );

	// Make lines a little thicker
	glLineWidth( 1.5 );
	

	/* Lighting */
	glEnable(GL_LIGHTING);						// enable lighting

	// Setup the materials for LIGHT0
	glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiff);

	// Now setup LIGHT0
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);		// setup the ambient element
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);		// the diffuse element
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);	// place the light in the world

	// Enable the light
	glEnable(GL_LIGHT0);

	/* Fog */
	glFogi(GL_FOG_MODE, fogMode[fogfilter]);	// Fog Mode
	glFogfv(GL_FOG_COLOR, fog_color);			// Set Fog Color
	glFogf(GL_FOG_DENSITY, 0.009f);				// How Dense Will The Fog Be
	glHint(GL_FOG_HINT, GL_DONT_CARE);			// Fog Hint Value
	glFogf(GL_FOG_START, 1.0f);					// Fog Start Depth
	glFogf(GL_FOG_END, WORLD_WDTH );			// Fog End Depth
	glDisable(GL_FOG);						// Enables GL_FOG

	/* Set up camera */
	ZeroVector( c_origin );
	ZeroVector( c_angles );
	ZeroVector( c_view );
	ZeroVector( c_right );
	ZeroVector( c_up );
	
	Ren_TextureInit();
	Ren_DrawInit();
	Ren_ModelInit();

	//Ren_CreateSkyBox();
}

void Ren_Shutdown( void )
{
	printf( "Shutting Down Renderer\n" );

	Ren_ModelShutdown();
	Ren_DrawShutdown();
	Ren_TextureShutdown();
}

/* NEW CAMERA ROTATION CODE NOW THAT I KIND OF UNDERSTAND TRANSLATION IN OPENGL */
/* TOO ANSWER A QUESTION FOR GRISWOLD - YES THE VIEW ALWAYS LOOKS DOWN ITS OWN Z AXIS */
/* I DIDN'T KNOW WHAT YOU WERE TALKING ABOUT BEFORE */

void Ren_CameraSetOrigin( vec3_t neworigin )
{
	VectorCopy( neworigin, c_origin );
}

void Ren_CameraSetAngles( vec3_t newangles )
{
	VectorCopy( newangles, c_angles );
}
