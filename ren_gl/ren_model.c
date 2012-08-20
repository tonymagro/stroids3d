#include "ren_gl.h"
#include "../scommon/milkshape.h"

#define MAX_MODELS	32

ms3d_model_t *ms3d_data[MAX_MODELS];

model_t *models[MAX_MODELS];
int		modelcount;

model_t nullmodel;

sboolean bPolyMode;

void Ren_ModelInit( void )
{
	printf( "Initializing Models\n" );
	modelcount = 0;
	bPolyMode = False;
	nullmodel.modelid = -1;
}

int Ren_LoadModelSkin( char *filename )
{
	image_t *model;

	model = Ren_FindImage( filename );

	if( !model )
		return 0;

	return model->textureid;
}

void Ren_ModelShutdown( void )
{
	int i;

	printf( "Shutting Down Models\n" );

	for( i = 0; i < modelcount; i++ )
	{
		if( ms3d_data[i] )
		{
			MS3D_DestroyModel( ms3d_data[i] );
		}
	}

	for( i = 0; i < modelcount; i++ )
	{
		if( models[i] )
		{
			mem_free( models[i] );
			models[i] = NULL;
		}
	}

	modelcount = 0;
}

model_t *Ren_LoadModel( const char *filename )
{
	int i;
	int texid;
	char texturefile[MAX_FILENAME];
	ms3d_model_t *ms3ddata;
	model_t *new_model;

	new_model = NULL;

	if( modelcount > MAX_MODELS )
	{
		printf( "Maximun Number of Models Reached: %i", MAX_MODELS );
		return &nullmodel;
	}

	printf( "Loading Model: %s\n", filename );
	
	// see if the model is already loaded
	for( i = 0; i < modelcount; i++ )
	{
		if( strcmp( models[i]->filename, filename ) == 0 )
		{
			printf( "Model already loaded with index: %i\n", i );
			return models[i];
		}
	}

    ms3ddata = MS3D_LoadModel( filename );

	if( !ms3ddata )
	{
		printf( "ERROR: Couldn't load model: %s\n", filename );
		return &nullmodel;
	}
 
	printf( "Loading Textures for: %s\n", filename );
	/* Find the model's skin id */
	for( i = 0; i < ms3ddata->numMaterials; i++ )
	{
		/* check if there is no material name */
		if( strcmp( ms3ddata->materials[i].texture, "\0" ) == 0 )
		{
			printf( " No Texture associated with material\n" );
			ms3ddata->materials[i].textureid = 0;
			continue;
		}

		/* There is a material texture file so load it */
		ms3ddata->materials[i].texture[0] = 's';
		ms3ddata->materials[i].texture[1] = '/';
		sprintf( texturefile, "data/model%s", ms3ddata->materials[i].texture ); 
		texid = Ren_LoadModelSkin( texturefile );
		ms3ddata->materials[i].textureid = texid;
	}

	/* Store the model data */
	ms3d_data[modelcount] = ms3ddata;


	/* Create model_t object */
	new_model = mem_malloc( sizeof(model_t) );

	strcpy( new_model->filename, filename  );
	new_model->modelid = modelcount;

	models[modelcount] = new_model;

	modelcount++;

	printf( "Finished\n" );

	return new_model;
}

void Ren_SetPolyMode( sboolean wire )
{
	bPolyMode = wire;
}

void Ren_DrawModelLocation( int modelid, entity_t *ent, vec3_t location )
{
	int i;
	int j;
	int k;


	if( modelid < 0 )
		return;
	
	if( bPolyMode )
	{
		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_LINE);
	}

	// draw the model
	glBlendFunc(GL_ONE, GL_ZERO);
	glDepthMask( GL_TRUE );		// back to normal Z buffering

	//printf( "NO(%f,%f,%f)\n", location[0], location[1], location[2] );

	glPushMatrix();
	glTranslatef( location[0], location[1], location[2] );
	glRotatef( ent->angles[YAW], 0.0f, 0.0f, 1.0f );
	glRotatef( ent->angles[PITCH], 0.0f, 1.0f, 0.0f );
	glRotatef( ent->angles[ROLL], 1.0f, 0.0f, 0.0f );
	glScalef( ent->scale, ent->scale, ent->scale );

	//Draw by group
	for( i = 0; i < ms3d_data[modelid]->numGroups; i++ )
	{
		
		int materialIndex = ms3d_data[modelid]->groups[i].materialIndex;
		if ( materialIndex >= 0 )
		{
			//glMaterialfv( GL_FRONT, GL_AMBIENT, ms3d_data[modelid]->materials[materialIndex].ambient );
			//glMaterialfv( GL_FRONT, GL_DIFFUSE, ms3d_data[modelid]->materials[materialIndex].diffuse );
			//glMaterialfv( GL_FRONT, GL_SPECULAR, ms3d_data[modelid]->materials[materialIndex].specular );
			//glMaterialfv( GL_FRONT, GL_EMISSION, ms3d_data[modelid]->materials[materialIndex].emissive );
			//glMaterialf( GL_FRONT, GL_SHININESS, ms3d_data[modelid]->materials[materialIndex].shininess );

			/* Bind the texture data */
			if ( ms3d_data[modelid]->materials[materialIndex].textureid > 0 )
			{
				Ren_BindImage( ms3d_data[modelid]->materials[materialIndex].textureid );
				glEnable( GL_TEXTURE_2D );
			}
			else
			{
				glDisable( GL_TEXTURE_2D );
			}
		}
		else
		{
			// Material properties?
			//glDisable( GL_TEXTURE_2D );
		}
		
		//Draw as regular triangles, since .ms3d's aren't optimized like .md2's
		glBegin(GL_TRIANGLES);
			for( j = 0; j < ms3d_data[modelid]->groups[i].numTriangles; j++ )
			{
				
				int triangleIndex		=  ms3d_data[modelid]->groups[i].triangleIndices[j];
				const ms3d_triangle_t* tri = &ms3d_data[modelid]->triangles[triangleIndex];

				//Loop through the triangle's vertices, and output them!
				for( k = 0; k < 3; k++)
				{
					unsigned short index = tri->vertexIndices[k];

					glNormal3fv( tri->vertexNormals[k]);
					glTexCoord2f(tri->u[k], -tri->v[k]);
					glVertex3fv(ms3d_data[modelid]->vertices[index].vertex);
				}
			}
		glEnd();
	}

//	glDisable( GL_LIGHTING );
	glEnable(GL_TEXTURE_2D);

	if( bPolyMode ) {
		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_FILL);
	}


	glPopMatrix();
}

void Ren_DrawModel( int modelid, entity_t *ent )
{
	int i;
	int j;
	int k;

	if( modelid < 0 )
		return;
	
	/*
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glDepthMask( GL_FALSE );		// no z buffering
	glEnable( GL_BLEND );


	glBindTexture( GL_TEXTURE_2D, 5 );

	glBegin( GL_POLYGON );
			glTexCoord2f( 0.0f, 1.0f ); glVertex3f( -3.0f, -3.0f, 5.0f );
			glTexCoord2f( 1.0f, 1.0f ); glVertex3f( 3.0f, -3.0f, 5.0f );
			glTexCoord2f( 1.0f, 0.0f ); glVertex3f( 3.0f, 3.0f, 5.0f );
			glTexCoord2f( 0.0f, 0.0f ); glVertex3f( -3.0f, 3.0f, 5.0f );
	glEnd();

	glDisable( GL_BLEND );
	glColor4f( 1,1,1,1 );
	glDepthMask( 1 );		// back to normal Z buffering

	glLightfv( GL_LIGHT1, GL_POSITION, lightposition );
	glLightfv( GL_LIGHT1, GL_DIFFUSE, whitelight );
	glLightfv( GL_LIGHT1, GL_SPECULAR, whitelight );
	glLightfv( GL_LIGHT1, GL_AMBIENT, light_ambient );
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT1 );

  */
	

	if( bPolyMode )
	{
		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_LINE);
	}

	// draw the model
	glBlendFunc(GL_ONE, GL_ZERO);
	glDepthMask( GL_TRUE );		// back to normal Z buffering

	glPushMatrix();
	glTranslatef( ent->origin[0], ent->origin[1], ent->origin[2] );
	glRotatef( ent->angles[YAW], 0.0f, 0.0f, 1.0f );
	glRotatef( ent->angles[PITCH], 0.0f, 1.0f, 0.0f );
	glRotatef( ent->angles[ROLL], 1.0f, 0.0f, 0.0f );
	glScalef( ent->scale, ent->scale, ent->scale );

	//Draw by group
	for( i = 0; i < ms3d_data[modelid]->numGroups; i++ )
	{
		
		int materialIndex = ms3d_data[modelid]->groups[i].materialIndex;
		if ( materialIndex >= 0 )
		{
			//glMaterialfv( GL_FRONT, GL_AMBIENT, ms3d_data[modelid]->materials[materialIndex].ambient );
			//glMaterialfv( GL_FRONT, GL_DIFFUSE, ms3d_data[modelid]->materials[materialIndex].diffuse );
			//glMaterialfv( GL_FRONT, GL_SPECULAR, ms3d_data[modelid]->materials[materialIndex].specular );
			//glMaterialfv( GL_FRONT, GL_EMISSION, ms3d_data[modelid]->materials[materialIndex].emissive );
			//glMaterialf( GL_FRONT, GL_SHININESS, ms3d_data[modelid]->materials[materialIndex].shininess );

			/* Bind the texture data */
			if ( ms3d_data[modelid]->materials[materialIndex].textureid > 0 )
			{
				Ren_BindImage( ms3d_data[modelid]->materials[materialIndex].textureid );
				glEnable( GL_TEXTURE_2D );
			}
			else
			{
				glDisable( GL_TEXTURE_2D );
			}
		}
		else
		{
			// Material properties?
			//glDisable( GL_TEXTURE_2D );
		}

		//Draw as regular triangles, since .ms3d's aren't optimized like .md2's
		glBegin(GL_TRIANGLES);
			for( j = 0; j < ms3d_data[modelid]->groups[i].numTriangles; j++ )
			{
				
				int triangleIndex		=  ms3d_data[modelid]->groups[i].triangleIndices[j];
				const ms3d_triangle_t* tri = &ms3d_data[modelid]->triangles[triangleIndex];

				//Loop through the triangle's vertices, and output them!
				for( k = 0; k < 3; k++)
				{
					unsigned short index = tri->vertexIndices[k];

					glNormal3fv( tri->vertexNormals[k]);
					glTexCoord2f(tri->u[k], -tri->v[k]);
					glVertex3fv(ms3d_data[modelid]->vertices[index].vertex);
				}
			}
		glEnd();
	}

	//glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );

	if( bPolyMode ) {
		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_FILL);
	}

//	glDisable( GL_LIGHTING );
	glEnable(GL_TEXTURE_2D);

	glPopMatrix();
}

void Ren_Draw2DModel( float x, float y, vec3_t angles, int modelid )
{
	int i;
	int j;
	int k;


	if( modelid < 0 )
		return;
	
	if( bPolyMode )
	{
		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_LINE);
	}

	// draw the model
	glBlendFunc(GL_ONE, GL_ZERO);
	glDepthMask( GL_TRUE );		// back to normal Z buffering
	//glEnable(GL_DEPTH_TEST);

	//printf( "NO(%f,%f,%f)\n", location[0], location[1], location[2] );

	glPushMatrix();
	glTranslatef( x, y, 0 );
	glRotatef( angles[YAW], 0.0f, 0.0f, 1.0f );
	glRotatef( angles[PITCH], 0.0f, 1.0f, 0.0f );
	glRotatef( angles[ROLL], 1.0f, 0.0f, 0.0f );
	//glScalef( ent->scale, ent->scale, ent->scale );

	//Draw by group
	for( i = 0; i < ms3d_data[modelid]->numGroups; i++ )
	{
		
		int materialIndex = ms3d_data[modelid]->groups[i].materialIndex;
		if ( materialIndex >= 0 )
		{
			//glMaterialfv( GL_FRONT, GL_AMBIENT, ms3d_data[modelid]->materials[materialIndex].ambient );
			//glMaterialfv( GL_FRONT, GL_DIFFUSE, ms3d_data[modelid]->materials[materialIndex].diffuse );
			//glMaterialfv( GL_FRONT, GL_SPECULAR, ms3d_data[modelid]->materials[materialIndex].specular );
			//glMaterialfv( GL_FRONT, GL_EMISSION, ms3d_data[modelid]->materials[materialIndex].emissive );
			//glMaterialf( GL_FRONT, GL_SHININESS, ms3d_data[modelid]->materials[materialIndex].shininess );

			/* Bind the texture data */
			if ( ms3d_data[modelid]->materials[materialIndex].textureid > 0 )
			{
				Ren_BindImage( ms3d_data[modelid]->materials[materialIndex].textureid );
				glEnable( GL_TEXTURE_2D );
			}
			else
			{
				glDisable( GL_TEXTURE_2D );
			}
		}
		else
		{
			// Material properties?
			//glDisable( GL_TEXTURE_2D );
		}
		
		//Draw as regular triangles, since .ms3d's aren't optimized like .md2's
		glBegin(GL_TRIANGLES);
			for( j = 0; j < ms3d_data[modelid]->groups[i].numTriangles; j++ )
			{
				
				int triangleIndex		=  ms3d_data[modelid]->groups[i].triangleIndices[j];
				const ms3d_triangle_t* tri = &ms3d_data[modelid]->triangles[triangleIndex];

				//Loop through the triangle's vertices, and output them!
				for( k = 0; k < 3; k++)
				{
					unsigned short index = tri->vertexIndices[k];

					glNormal3fv( tri->vertexNormals[k]);
					glTexCoord2f(tri->u[k], -tri->v[k]);
					glVertex3fv(ms3d_data[modelid]->vertices[index].vertex);
				}
			}
		glEnd();
	}

	glEnable(GL_TEXTURE_2D);
	//glDisable(GL_DEPTH_TEST);
	glDepthMask( GL_FALSE );

	if( bPolyMode ) {
		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_FILL);
	}


	glPopMatrix();
}
