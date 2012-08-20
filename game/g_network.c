#include "game.h"
#include "../scommon/s_net.h"

#if NETWORK_SUPPORT

#define MAX_PLAYERS 32

char * configfile = "config.cfg";

sboolean Hosting;
sboolean Joining;

netplayerinfo_t players[MAX_PLAYERS];

int currentplayers;

unsigned char TempDat[MAX_PACKET_LEN];

/* Client Mode information */
char	name[256];
char	IP[256];

static unsigned int serverip;
static unsigned short serverport;

void G_NetworkInit( void )
{
	unsigned int ipaddr;
	char ipstring[128];

	currentplayers = 0;

	Net_start( 5050 );
	Net_get_local_addr( &ipaddr );
	Net_addr_to_str( ipaddr, ipstring, 128 );

	printf( "Local IP: %s\n", ipstring );
}

void G_NetworkShutdown( void )
{
	Net_stop();
}

void G_SendPlayersInfo( unsigned int ipaddr, unsigned short port )
{
	printf( "Send other player info to this player\n" );
}

void G_CreatePlayer( void )
{
	ship2.radius = 2.0f;
	ship2.type = ET_SHIP;
	ship2.scale = 1.0f;
	ship2.alive = True;
	ship2.angles[YAW]	= 0.0f;
	ship2.angles[PITCH] = 0.0f;
	ship2.angles[ROLL]	= 0.0f;
	ship2.counter1	= 0;
	ship2.counter2	= 0.0f;
	ship2.maxcount1 = 0;
	ship2.maxcount2 = 0.0f;
	ship2.maxVelocity = 0;
	ZeroVector( ship2.origin );
	ZeroVector( ship2.velocity );

	ship2.origin[0] = 10.0f;
}

void G_ParseNetMessage( char *data, unsigned int ipaddr, unsigned short port )
{
	char ipstring[128];
	unsigned short header;
	uint index;
	float x, y, z;

	index = 0;

	Net_addr_to_str( ipaddr, ipstring, 128 );
	printf( "INDEX: %i\n", index );

	readShort( data, index, header );

	printf( "PACKET_HEADER: %i\n", header );

	switch( header )
	{
	case PACKET_CONNECT:
		printf( "Player from %s trying to connect\n", ipstring );
		G_SendPlayersInfo( ipaddr, port );
		players[currentplayers].ipaddress = ipaddr;
		players[currentplayers].port = port;
		G_CreatePlayer();
		break;
	case PACKET_PLAYERCOORD:
		readFloat( data, index, x );
		readFloat( data, index, y );
		readFloat( data, index, z );
		ship2.origin[0] = x;
		ship2.origin[1] = y;
		ship2.origin[2] = z;
		//printf( "NewPC(%f,%f,%f)\n", ship2.origin[0], ship2.origin[1], ship2.origin[2] );
		break;
	default:
		printf( "BAD PACKET!\n" );
	}
	
}

/* Frame Length */
static int	T0;
float framelen;

void G_UpdateNetwork( void )
{
	int len;
	unsigned int ipaddr;
	unsigned short port;
	char ipstring[128];
	int index;
	static int timecount = 0;
	float x, y, z;

	char senddata[MAX_PACKET_LEN];

	//if( framelen < 10 )
	//	return;

	if( Hosting )
	{

		/* Get network data */
		len = Net_get_dat( TempDat, MAX_PACKET_LEN, &ipaddr, &port );
	
		if( len > 0 )
		{
			Net_addr_to_str( ipaddr, ipstring, 128 );
			//printf( "Got %i bytes from %s\n", len, ipstring );
			//printf( "Data: %s\n", TempDat );
			G_ParseNetMessage( TempDat, ipaddr, port );
		}
	}

	if( Joining )
	{
		if( timecount++ < 3 )
			return;

		timecount = 0;

		// send position data
		//printf( "Sending info\n" );
		index = 0;
		x = ship.origin[0];
		y = ship.origin[1];
		z = ship.origin[2];

		writeShort( senddata, index, PACKET_PLAYERCOORD );
		writeFloat( senddata, index, x );
		writeFloat( senddata, index, y );
		writeFloat( senddata, index, z );

		/*
		index = x = y = z = 0;

		readShort( senddata, index, temp );
		readFloat( senddata, index, x );
		readFloat( senddata, index, y );
		readFloat( senddata, index, z );

		printf( "RPC(%f,%f,%f)\n", x, y, z );
		*/

		Net_addr_to_str( serverip, ipstring, 128 );
		//printf("Trying to send message to %s\n", ipstring );
		//printf( "1PC(%f,%f,%f)\n", ship.origin[0], ship.origin[1], ship.origin[2] );
		Net_send_dgram( serverip, 5050, senddata, sizeof(short)+sizeof(float)*3 );
	}
}

void G_HostGame( void )
{
	Hosting = True;
}

void G_DrawNetGraph( void )
{
	int in, out;

	Ren_Color( 1.0f, 0.0f, 0.0f, 1.0f );

	Ren_printf( 5.0f, 5.0f, 0.5f, "Packets Out: %i", Net_get_packets_out() );
	Ren_printf( 5.0f, 4.5f, 0.5f, "Packets In:  %i", Net_get_packets_in() );

	in  = Net_get_bytes_in() / 10000;
	out = Net_get_bytes_out() / 10000;

	Ren_printf( 5.0f, 3.5f, 0.5f, "Bytes Out: %i mb", out  );
	Ren_printf( 5.0f, 3.0f, 0.5f, "Bytes In:  %i mb", in );
}

void G_JoinGame( char *ip )
{
	FILE *fp;
	unsigned char senddata[256];
	int index;

	index = 0;

	if( Joining )
		return;

	Joining = True;

	/* read in ip from config file */
	fp = fopen("config.cfg","r");
	fgets(IP,16,fp);
	IP[strlen(IP)-1] = '\0';
	fgets(name,32,fp);
	name[strlen(name)-1] = '\0';
	fclose(fp);

	strcpy( players[0].name, name );
	Net_addr_from_str( ip, &serverip, &serverport );

	printf( "Multiplayer Name: %s\n", players[0].name );
	printf( "Trying to connect to %s\n", ip );

	writeShort( senddata, index, PACKET_CONNECT );

	printf( "Sending: %s\n", senddata );

	Net_send_dgram( serverip, serverport, senddata, sizeof( short int ) );
}

#else

void G_NetworkInit( void )
{
}

void G_UpdateNetwork(void )
{
}

void G_NetworkShutdown( void )
{
}

void G_DrawNetGraph( void )
{
}

void G_JoinGame( char *ip )
{
}

void G_HostGame( void )
{
}

#endif
