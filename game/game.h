#include "../scommon/s_common.h"
#include "../scommon/keys.h"

/* -----------------------------
	Player State Info
-------------------------------- */
typedef enum { PS_ALIVE, PS_DEAD, PS_KILLED, PS_NEXTLEVEL, PS_DEMOMODE } player_state;
typedef struct player_info_s
{
	player_state	state;
	int				score;
	int				lives;
	int				level;
	int				r_counter;
	int				r_count;
	int				maxHealth;
	int				health;
	float			energy;
	float			maxEnergy;
	int				ammo;
	int				counter;
	int				shield_count;
	int				shield_strength;
	int				rate_of_fire;

	int				stroiddamage;
	int				stroidpoints;

	int				win_counter;
	int				win;/* Check for collisions with rocks */

	int				stroids_left;

	sboolean		paused;
	sboolean		shieldon;

	sboolean		beatLevel;

	// Weapons
	//int				weapon[2];
	int				weapon_selection;

	// Controls
	float			mouse_sense;
	int				mouse_dx;
	int				mouse_dy;
	float			joy_sense;
	int				joy_dx;
	int				joy_dy;


	// View
	int				cameratype;

} player_info_t;

/* Player State Information */
extern player_info_t	playerinfo;


/* Game Key Event Handling */
void		G_InitKeys( void );
void		G_KeyFrame( void );
void		G_KeyEvent( int key, sboolean down );
sboolean	G_KeyDown( int key );
void		G_MouseMove( int deltax, int deltay );
void		G_JoyStickAxis( int deltax, int deltay );

/* -----------------------------
	Player State Info
-------------------------------- */


/* -----------------------------
	World
-------------------------------- */
/* Note: In some places, I think it might be assumed that the following three are equal. This is bad. */
/* Yes. This is True. Changing the world to a non-cube may destroy the fabric of the universe */

/*
#define WORLD_WDTH    		1000

#define WORLD_HGHT          1000

#define WORLD_DPTH     		1000
*/

extern float num_worlds;

extern float WORLD_WDTH;
extern float WORLD_HGHT;
extern float WORLD_DPTH;

/*
#define WORLD_WIDTH_MAX		( WORLD_WDTH / 2)
#define WORLD_WIDTH_MIN		(-WORLD_WDTH / 2)
#define WORLD_HEIGHT_MAX	( WORLD_HGHT / 2)
#define WORLD_HEIGHT_MIN	(-WORLD_HGHT / 2)
#define WORLD_DEPTH_MAX		( WORLD_DPTH / 2)
#define WORLD_DEPTH_MIN		(-WORLD_DPTH / 2)
*/
extern float WORLD_WIDTH_MAX;
extern float WORLD_WIDTH_MIN;
extern float WORLD_HEIGHT_MAX;
extern float WORLD_HEIGHT_MIN;
extern float WORLD_DEPTH_MAX;
extern float WORLD_DEPTH_MIN;


/* Maximum amount of enitites in the world */
#define MAX_STROIDS		20
#define MAX_BULLETS		100
#define MAX_PARTICLES	1000
#define MAX_STARS		1
#define MAX_POWERUPS	20
#define MAX_BOMBS		35
#define MAX_STATIONS	4

#define MAX_BULLETLIFE	1.0f

/* Entities in the world */
extern entity_t* ent_lsthead;

extern entity_t	ship;
extern entity_t	ship2;

extern entity_t	stations[MAX_STATIONS];

extern entity_t	shield;
extern entity_t	stroids[MAX_STROIDS];
extern entity_t	bullets[MAX_BULLETS];
extern entity_t bombs[MAX_BOMBS];
extern entity_t powerups[MAX_POWERUPS];
/* Particle Engine */
extern entity_t particles[MAX_PARTICLES];
/* Stars */
extern entity_t	stars[MAX_STARS];

/* Models */
extern model_t		*mod_ship;
extern model_t		*mod_stroid;
extern model_t		*mod_bullet;
extern model_t		*mod_station;

/* images */
extern image_t		*pic_ship;
//extern image_t		*pic_stroid;
extern image_t		*pic_particle;
extern image_t		*pic_hudship;
extern image_t		*pic_block;
extern image_t		*pic_reticle;
extern image_t		*pic_ripple;

extern image_t		*pic_armorpowup;

/* sounds */
extern sound_t		*snd_laser1;
extern sound_t		*snd_explode1;
extern sound_t		*snd_music1;
extern sound_t		*snd_sonic;
/* -----------------------------
	World
-------------------------------- */

/* -----------------------------
	Network
-------------------------------- */

#define PACKET_BULLETCOORD	1
#define PACKET_PLAYERCOORD	2
#define PACKET_CONNECT		3
#define PACKET_DISCONNECT	4
#define PACKET_CHAT			5

#define PACKET_SERVER_PLAYERS	6

typedef struct netplayerinfo_s
{
	unsigned int ipaddress;
	unsigned short port;
	char name[256];
	int	 id;
} netplayerinfo_t;

void G_NetworkInit( void );
void G_NetworkShutdown( void );

void G_UpdateNetwork( void );
void G_HostGame( void );
void G_JoinGame( char *ip );

void G_DrawNetGraph( void );

/* -----------------------------
	Network
-------------------------------- */
void G_InitHighScores( void );

typedef enum {  GS_INIT,
				GS_INTRO, 
				GS_RESTART, 
				GS_PAUSED, 
				GS_RUNNING, 
				GS_MENU,
				GS_NOTRUNNING,
				GS_INFOSCREEN } game_state_s;

typedef struct game_info_s
{
	game_state_s gamestate;
	sboolean	 paused;
	sboolean	 menuopen;
	sboolean	 fadein;
	sboolean	 fadeout;
	sboolean	 joystick;
	float		 framelen;
	float		 timescale;
	game_state_s lastgamestate;
} game_info_t;

/* Game State */
extern game_info_t	gi;

int		G_Main( void );
void	G_Init( void );
void	G_Shutdown( void );
void	G_Restart( void );
void	G_LoadLevel( int level );
void	G_Spawn( entity_t *ent, vec3_t origin, float radius, float scale );

void Menu_KeyDown( int key, sboolean down );

/* ------------------------------
	Entities
---------------------------------*/
int		G_Collision( entity_t *ent1, entity_t *ent2 );
void	G_RotateEntity( entity_t *ent, int axis, float angle );
void	G_MoveEntity( entity_t *ent, float scalar );
void	G_AccelEntity( entity_t *ent, float scalar );
void	G_WorldWrap( entity_t *ent );
void	G_DisableInputFrame( void );
char    G_PointVisible( vec3_t p );
char    G_CubeVisible( vec3_t c );
void    G_DrawWorld( void );
/* ------------------------------
	Entities
---------------------------------*/

/* ------------------------------
	Player
---------------------------------*/
typedef struct player_view_s
{
	/* Maybe this should be replaced with (forward, right, up) vectors? */
	float delta_roll;
	float delta_yaw;
	float delta_pitch;
} player_view_t;

typedef struct player_view_thirdperson_s
{
	float temp;
} player_view_thirdperson_t;

extern plane_t frustum[6];

extern 	vec4_t	enginecolor;
extern  vec4_t	fuelcolor;

void Ship_Init( void );
/* void Ship_Draw( void ); */
void Ship_Move();

/* ------------------------------
	Player
---------------------------------*/

/* ------------------------------
	HUD
---------------------------------*/
void Hud_Init( void );
void Hud_Move( void );
void Hud_Draw( void );
/* ------------------------------
	HUD
---------------------------------*/

/* ------------------------------
	PARTICLES
---------------------------------*/
void Particles_Init( void );
void Particles_Spawn( vec3_t origin, vec3_t velocity, vec4_t color, particletype type, float fade, float scale );
/* void Particles_Draw( void ); */
void Particles_Move( void );
void G_StartExplosion( vec3_t origin, vec3_t velocity, vec4_t color, particletype type, float fade, float scale, int numparticles );
/* ------------------------------
	PARTICLES
---------------------------------*/

/* ------------------------------
	STROIDS
---------------------------------*/
void Stroids_Init( void );
void G_StroidSpawn( vec3_t origin, vec3_t velocity, float radius, float scale );
void Stroids_Move( void );
/* void Stroids_Draw( void ); */
void Stroids_Shutdown( void );
void Stroids_KillAll( void );
/* ------------------------------
	STROIDS
---------------------------------*/

/* ------------------------------
	STATIONS
---------------------------------*/
void Stations_Init( void );
void Stations_Move( void );
void Stations_Explosion( vec3_t origin );

extern vec4_t trailcolor;
/* ------------------------------
	STATIONS
---------------------------------*/

/* ------------------------------
	BULLETS
---------------------------------*/
void Bullets_Init( void );
void Bullets_Move( void );
void Bullets_Fire( vec3_t origin, vec3_t velocity, vec4_t color, entity_t *owner, float life );
void Bullets_Draw( void );
/* ------------------------------
	BULLETS
---------------------------------*/
/* ------------------------------
	BOMB
---------------------------------*/
void W_BombInit( void );
void W_BombMove( void );
void W_BombFire( vec3_t origin, vec3_t velocity, int type );

/* ------------------------------
	BOMB
---------------------------------*/


/* ------------------------------
	STARS
---------------------------------*/
void Stars_Init( void );
void Stars_Move( void );
void Stars_Draw( void );
/* ------------------------------
	STARS
---------------------------------*/

/* ------------------------------
	Menu
---------------------------------*/
void G_InitMenu( void );
void G_RunMenu( void );
void G_DrawMenu( void );

void G_InitLimbo( void );
void G_RunLimbo( void );
void G_DrawLimbo( void );
/* ------------------------------
	Menu
---------------------------------*/

/* ------------------------------
	Intro
---------------------------------*/
void G_InitIntro( void );
void G_RunIntro( void );
void G_DrawIntro( void );
void Intro_KeyDown( int key, sboolean down );
/* ------------------------------
	Intro
---------------------------------*/
/* ------------------------------
	Powerups
---------------------------------*/
void PowerUps_Init();
void PowerUps_Draw();
void PowerUps_Move();
/* ------------------------------
	Powerups
---------------------------------*/
