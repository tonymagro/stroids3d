
//
// types
//
#ifndef byte
//typedef unsigned char byte;
#endif // byte



/* File Header Information */
typedef struct ms3d_header_s
{
	char id[10];
	int version;
} ms3d_header_t;

/* MilkShape Vertex Info */
typedef struct ms3d_vertex_s
{
	unsigned char flags;
	unsigned char refcount;
	char boneId;
	vec3_t vertex;
} ms3d_vertex_t;

/* MilkShape Triangle Info */
typedef struct ms3d_triangle_s
{
	unsigned short flags;
	unsigned short vertexIndices[3];
	float vertexNormals[3][3];
	vec3_t u, v;
	unsigned char smoothingGroup;
	unsigned char groupIndex;
} ms3d_triangle_t;

/* MilkShape Groups Info */
typedef struct ms3d_group_s
{
	unsigned char flags;
	char name[32];
	unsigned short numTriangles;
	unsigned short* triangleIndices;
	char materialIndex;
} ms3d_group_t;

typedef struct ms3d_material_s
{
    char            name[32];                           //
    float           ambient[4];                         //
    float           diffuse[4];                         //
    float           specular[4];                        //
    float           emissive[4];                        //
    float           shininess;                          // 0.0f - 128.0f
    float           transparency;                       // 0.0f - 1.0f
    char            mode;                               // 0, 1, 2 is unused now
    char            texture[128];                        // texture.bmp
    char            alphamap[128];                       // alpha.bmp
	int				textureid;
} ms3d_material_t;


/* MilkShape Struct */
typedef struct ms3d_model_s
{
	unsigned short numVertices;
	ms3d_vertex_t* vertices;

	unsigned short numTriangles;
	ms3d_triangle_t* triangles;

	unsigned short numGroups;
	ms3d_group_t*	groups;

	unsigned short numMaterials;
	ms3d_material_t *materials;
} ms3d_model_t;


ms3d_model_t	*MS3D_LoadModel( const char *filename );
void			MS3D_DestroyModel( ms3d_model_t *model );

