#include "s_shared.h"
#include "milkshape.h"

ms3d_model_t *MS3D_LoadModel( const char *filename )
{
	int i;
	FILE* file;
	ms3d_model_t *ms3dmodel;
	ms3d_header_t header;

	printf( "Model Info: %s\n", filename );

	if( (file = fopen( filename, "rb" )) == NULL )
	{
		//printf( "ERROR: Couldn't load Model: %s\n", filename );
		return NULL;
	}

	/* Read in Header information */
	fread( &header.id, sizeof(char), 10, file );
        File_ReadInt32( file, &header.version, 1 );

	printf( " header: %s\n", header.id );
	printf( " version: %i\n", header.version );

	/* Make sure header information is correct */
	if( strncmp( header.id, "MS3D000000", 10) != 0 )
	{
		printf( "ERROR: Model is not .ms3d format: %s\n", filename );
                fclose( file );
		return NULL;
	}

	if( header.version != 3 && header.version != 4 )
	{
		printf( "ERROR: Model incorrect version number: %s\n", filename );
                fclose( file );
		return NULL;
	}

	/* Convert ms3dmodel to stroids model format */
	ms3dmodel = mem_malloc( sizeof(ms3d_model_t) );

	
	/* Read in vertices */
    File_ReadInt16( file,  &ms3dmodel->numVertices, 1 );
	printf( " Verticies: %i\n", ms3dmodel->numVertices );
	ms3dmodel->vertices = mem_malloc(sizeof(ms3d_vertex_t) * ms3dmodel->numVertices);
	for( i = 0; i < ms3dmodel->numVertices; i++ )
	{
		fread( &ms3dmodel->vertices[i].flags,	sizeof(byte),	1, file);
        File_ReadFloat32( file, ms3dmodel->vertices[i].vertex, 3 );
                
		fread( &ms3dmodel->vertices[i].boneId,	sizeof(char),	1, file);
		fread( &ms3dmodel->vertices[i].refcount,sizeof(byte),	1, file);

		//printf( "  Vertex[%i] = (%f,%f,%f)\n", i, ms3dmodel->vertices[i].vertex[0], ms3dmodel->vertices[i].vertex[1], ms3dmodel->vertices[i].vertex[2] );
	}

	/* Read in triangles */
	File_ReadInt16( file, &ms3dmodel->numTriangles, 1 );
	printf( " Triangles: %i\n", ms3dmodel->numTriangles );
	ms3dmodel->triangles = mem_malloc(sizeof(ms3d_triangle_t) * ms3dmodel->numTriangles);
	for( i = 0; i < ms3dmodel->numTriangles; i++ )
	{
		File_ReadInt16( file, &ms3dmodel->triangles[i].flags, 1 );
        File_ReadInt16( file, ms3dmodel->triangles[i].vertexIndices, 3 );
        File_ReadFloat32( file, ms3dmodel->triangles[i].vertexNormals[0], 3 );
        File_ReadFloat32( file, ms3dmodel->triangles[i].vertexNormals[1], 3 );
        File_ReadFloat32( file, ms3dmodel->triangles[i].vertexNormals[2], 3 );
        File_ReadFloat32( file, ms3dmodel->triangles[i].u, 3 );
		File_ReadFloat32( file, ms3dmodel->triangles[i].v, 3 );
		fread(&ms3dmodel->triangles[i].smoothingGroup,	sizeof(byte),		1, file);
		fread(&ms3dmodel->triangles[i].groupIndex,		sizeof(byte),		1, file);
	}

	/* Read in groups */
    File_ReadInt16( file, &ms3dmodel->numGroups, 1 );
	printf( " numGroups: %i\n", ms3dmodel->numGroups );
	ms3dmodel->groups = mem_malloc(sizeof(ms3d_group_t) * ms3dmodel->numGroups);
	for( i = 0; i < ms3dmodel->numGroups; i++ )
	{
		fread(&ms3dmodel->groups[i].flags,			sizeof(byte),	1,  file);
		fread( ms3dmodel->groups[i].name,			sizeof(char),	32, file);
        File_ReadInt16( file, &ms3dmodel->groups[i].numTriangles, 1 );
		
		ms3dmodel->groups[i].triangleIndices = mem_malloc( sizeof(word)*ms3dmodel->groups[i].numTriangles);
		
        File_ReadInt16( file, ms3dmodel->groups[i].triangleIndices, ms3dmodel->groups[i].numTriangles);
		fread(&ms3dmodel->groups[i].materialIndex,   sizeof(char),	1, file);
	}

	/* Read in Material */
    File_ReadInt16( file, &ms3dmodel->numMaterials, 1 );
	printf( " numMaterials: %i\n", ms3dmodel->numMaterials );
	ms3dmodel->materials = mem_malloc( sizeof(ms3d_material_t) * ms3dmodel->numMaterials );
	for( i = 0; i < ms3dmodel->numMaterials; i++ )
	{
		fread( ms3dmodel->materials[i].name,		sizeof(char), 32, file );
        File_ReadFloat32(file, ms3dmodel->materials[i].ambient, 4 );
        File_ReadFloat32(file, ms3dmodel->materials[i].diffuse, 4 );
        File_ReadFloat32(file, ms3dmodel->materials[i].specular, 4 );
        File_ReadFloat32(file, ms3dmodel->materials[i].emissive, 4 );
        File_ReadFloat32(file, &ms3dmodel->materials[i].shininess, 1 );
        File_ReadFloat32(file, &ms3dmodel->materials[i].transparency, 1 );
		fread( &ms3dmodel->materials[i].mode,		sizeof(char),	1, file );
		fread( ms3dmodel->materials[i].texture,		sizeof(char),	128, file );
		fread( ms3dmodel->materials[i].alphamap,	sizeof(char),	128, file );
                
		printf( "  Material%i Name: %s\n", i, ms3dmodel->materials[i].name );
		printf( "  Material%i File: \t%s\n", i, ms3dmodel->materials[i].texture );
		printf( "  Material%i AlphaMap: \t%s\n", i, ms3dmodel->materials[i].alphamap );

		printf( "  Material%i Ambient:\t(%f,%f,%f,%f)\n", i, ms3dmodel->materials[i].ambient[0], 
														   ms3dmodel->materials[i].ambient[1],
														   ms3dmodel->materials[i].ambient[2],
														   ms3dmodel->materials[i].ambient[3]);

		printf( "  Material%i Diffuse:\t(%f,%f,%f,%f)\n", i, ms3dmodel->materials[i].diffuse[0], 
														   ms3dmodel->materials[i].diffuse[1],
														   ms3dmodel->materials[i].diffuse[2],
														   ms3dmodel->materials[i].diffuse[3]);

		printf( "  Material%i Specular:\t(%f,%f,%f,%f)\n", i, ms3dmodel->materials[i].specular[0], 
														   ms3dmodel->materials[i].specular[1],
														   ms3dmodel->materials[i].specular[2],
														   ms3dmodel->materials[i].specular[3]);

		printf( "  Material%i Emissive:\t(%f,%f,%f,%f)\n", i, ms3dmodel->materials[i].emissive[0], 
														   ms3dmodel->materials[i].emissive[1],
														   ms3dmodel->materials[i].emissive[2],
														   ms3dmodel->materials[i].emissive[3]);

		printf( "  Material%i Shininess:    %f\n", i, ms3dmodel->materials[i].shininess );
		printf( "  Material%i Transparency: %f\n\n", i, ms3dmodel->materials[i].transparency );
		
	}

	printf( "Loaded MS3D Model: %s\n", filename );

	/* close the file */
	fclose( file );

	return ms3dmodel;
}

void MS3D_DestroyModel( ms3d_model_t *model )
{
	int i;

	if( !model )
		return;

	if( model->numMaterials )
	{
		mem_free( model->materials );
	}
	for( i = 0; i < model->numGroups; i++ )
	{
		if( model->groups[i].triangleIndices )
		{
			mem_free( model->groups[i].triangleIndices );
			model->groups[i].triangleIndices = NULL;
		}
	}
	if( model->groups )
	{
		mem_free( model->groups );
		model->groups = NULL;
	}
	if( model->triangles )
	{
		mem_free( model->triangles );
		model->triangles = NULL;
	}
	if( model->vertices )
	{
		mem_free( model->vertices );
		model->vertices = NULL;
	}

	mem_free( model );
}
