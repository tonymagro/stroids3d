#ifndef _Net_h_
#define _Net_h_


#if NETWORK_SUPPORT

#ifdef WIN32

#include <winsock2.h> // use windows sockets v. 2
#include <windows.h>
#include <io.h>

#else

#define	 SOCKET	int     // windows calls fds sockets, so this makes life easier.
#define  SOCKET_ERROR (-1)

// unix - berkely sockets includes.
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#endif

#endif



#define PORTS_TO_CHECK  (100)


void Net_init();
void Net_shutdown();


void Net_in();


/* BINDLIB_API_BEGIN */


// this gives enough room for headers for UDP while remaining under ethernet's 1500
// MTU.  The reason for the funny value is that it's 87 * 16, so we get 16 byte alignment.
#define MAX_PACKET_LEN  (1392)


// Packet buffers
#define writeShort(x, y, z)     {*((ushort *)((byte *)&x[y])) = z; y += 2;}
#define writeLong(x, y, z)      {*((ulong  *)((byte *)&x[y])) = z; y += 4;}
#define writeFloat(x, y, z)     {*((float  *)((byte *)&x[y])) = z; y += 4;}
#define writeDouble(x, y, z)    {*((double *)((byte *)&x[y])) = z; y += 8;}
#define readShort(x, y, z)      {z = *(ushort *)((byte *)&x[y]); y += 2;}
#define readLong(x, y, z)       {z = *(ulong  *)((byte *)&x[y]); y += 4;}
#define readFloat(x, y, z)      {z = *(float  *)((byte *)&x[y]); y += 4;}
#define readDouble(x, y, z)     {z = *(double *)((byte *)&x[y]); y += 8;}

#define writeByte(x, y, z)      (*(byte *)&x[y++] = (byte)z)
#define writeBlock(x, y, z, a)  {memcpy((char *)&x[y], (char *)z, a);y += a;}
#define readByte(x, y, z)       (z = *(byte *)&x[y++])
#define readBlock(x, y, z, a)   {memcpy((char *)z, (char *)&x[y], a);y += a;}

char * strchar( const char * str, const char * chars );


uint Net_start( ushort port );
void Net_stop();


ushort Net_get_port();
uint Net_get_bytes_in();
uint Net_get_bytes_out();
uint Net_get_packets_in();
uint Net_get_packets_out();


uint Net_send_dgram( uint ip, ushort port, const void * dat, uint len );

uint Net_aton( const char * ipstring, const char * portstring, 
		     uint * ipaddr, ushort * port );
uint Net_addr_from_str( const char * str, uint * ipaddr, ushort * port );
void Net_addr_to_str( uint addr, char * str, uint len );
void Net_get_local_addr( uint * ipaddr );

uint Net_get_dat( void * buf, uint max_len, uint * ipaddr, ushort * port );


ubyte   Net_extract_byte( const ubyte * dat, uint * index );
uint	Net_extract_int( const ubyte * dat, uint * index );
ushort  Net_extract_short( const ubyte * dat, uint * index );
float	Net_extract_float( const ubyte * dat, uint * index );


void Net_append_byte( ubyte * dat, uint * index, byte val );
void Net_append_int( ubyte * dat, uint * index, int val );
void Net_append_short( ubyte * dat, uint * index, short val );
void Net_append_float( ubyte * dat, uint * index, float val );
void Net_append_String( ubyte * dat, uint * index, const char * str );


/* BINDLIB_API_END */



#endif /* _Net_h_ */
