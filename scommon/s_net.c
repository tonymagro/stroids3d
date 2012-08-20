/* THIS CODE WRITTEN BY ALEX MOHR */

#include "s_common.h"
#if NETWORK_SUPPORT
#include "s_net.h"

/*
typedef int             Int32;
typedef unsigned int    UInt32;
typedef short           Int16;
typedef unsigned short  UInt16;
typedef char            Byte;
typedef unsigned char   UByte;
typedef float           Float32;
*/

sboolean Started;

SOCKET InSockFd;
struct sockaddr_in SockAddr;

uint BytesIn;
uint BytesOut;

uint PacketsIn;
uint PacketsOut;

ubyte TempDat[MAX_PACKET_LEN];

char * strchar( const char * str, const char * chars ) 
{
    const char * i;
    if( (str == NULL) || (chars == NULL) ) {
        return( NULL );
    }

    while( *str != '\0' ) {
        for( i = chars; *i != '\0'; i++ ) {
            if( *str == *i ) {
                return( (char *)str );
            }
        }
        str++;
    }

    return( NULL );
}

uint Net_get_dat( void * buf, uint max_len, uint * ipaddr, ushort * port ) {
    
    int len = 0;

    struct sockaddr_in dummy_addr;
    int dummy_addr_len = sizeof( dummy_addr );
    
    len = recvfrom( InSockFd, buf, max_len, 0, (struct sockaddr *) &dummy_addr,
        &dummy_addr_len );
    
    /** ** QUICK OUTS.. if we didn't get a packet, or other errors.. */
    if( (len < 0) ) {
        
#ifdef WIN32
        switch( WSAGetLastError() ) {
        case WSAEWOULDBLOCK:
            return( 0 ); /* didn't get a packet */
        case WSAECONNREFUSED:
            return( 0 ); /* ignore.. */
        case WSAECONNRESET:
            return( 0 ); /* this makes equally little sense */
        default:
            printf( "error code windows returned was %d\n", WSAGetLastError() );
            return( 0 );
        }
        
#else
		/*
        switch( errno ) {
        case EWOULDBLOCK:
            return( 0 ); /* didn't get a packet *
        case ECONNREFUSED:
        /* something's sort of fscked up in Linux, I think, in that
            it returns ECONNREFUSED for unconnected sockets when calling
            recvfrom()... this just doesn't really make sense at all, IMO *
        /* correction.. I've figured it out, and it's Linux specific.. sort
            of a feature, but I'm not going to put a chunk of linux
            specific code in here just for it.. *
            return( 0 );
            
        default:
            printf( "error code errno returned is %d\n", errno );
            printf( "recvfrom returned error, in get_packet()" );
            return( 0 );
        }
		*/
#endif
        
    }
    
    
    /* crapnet type stuff.. simulate bad net. */
#ifdef DROP_PACKETS /* this is for testing network.. randomly drops packets */
    if( b_math_frand( 0.0f, 1.0f ) > 0.65f ) {
        /* drop the packet -- 0.95 above would mean 5% random pl */
        //printf( "!\n" );
        return( 0 );
    }
#endif
    

    //printf( "packet length : %d bytes\n", len );


    BytesIn += len;
    PacketsIn++;


    if( ipaddr != NULL ) {
        *ipaddr = ntohl( dummy_addr.sin_addr.s_addr );
    }

    if( port != NULL ) {
        *port = ntohs( dummy_addr.sin_port );
    }

    return( len ); /* got a packet */
    
}











static void set_fd_nonblocking( SOCKET fd ) {
    
#ifdef WIN32

    u_long tmp = 0;
    u_long tmp2 = 0;

#else

    uint tmp = 0;

#endif
    
    printf( "setting network socket to non-blocking\n" );
    

#ifdef WIN32

    tmp2 = 1;
    tmp = ioctlsocket( fd, FIONBIO, &tmp2 );
    if( tmp ) {
        printf( " is %d\n", tmp );
        OS_Error( 1, "could not set socket to non-blocking" );
    }

#else

    if( (tmp = fcntl( fd, F_GETFL, 0 )) < 0 )
        OS_Error( 1, "could not read file descriptor flags" );
    if( (fcntl( fd, F_SETFL, tmp | O_NONBLOCK )) < 0 )
        OS_Error( 1, "could not set file descriptor to non-blocking" );

#endif


}






void Net_init() {
    
#ifdef WIN32
    uint err;
    WORD wVersionRequested;
    WSADATA wsaData;
#endif

    
    printf( "Initializing network driver\n" );
    Started = 0;
    BytesIn = 0;
    BytesOut = 0;
    PacketsIn = 0;
    PacketsOut = 0;

    //b_mem_swap_in( sizeof( TempDat ), TempDat );

    
#ifdef WIN32
    wVersionRequested = MAKEWORD( 2, 0 );
    err = WSAStartup( wVersionRequested, &wsaData );
    if( err ) {
        OS_Error( 1, "could not find windows sockets version 2.0" );
        return;
    }
    if( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 0 ) {
        WSACleanup();
        OS_Error( 1, "could not find windows sockets version 2.0" );
        return;
    }
#endif

    InSockFd = 0;


}






void Net_shutdown() {
    Net_stop();
    printf( "shutting down network driver\n" );
}






uint Net_start( ushort port ) {
    
    int i = 0;
    
    printf( "===========\n" );
    printf( " net_start\n" );
    printf( "===========\n" );
    
    if( Started )
        return( 1 ); /* 1 - network already up */
    
    if( (port < 1024) && (port != 0) )
        port = DEFAULT_SERVER_PORT;
    
    /* create socket */

    printf( "creating socket...\n" );

#ifdef WIN32
    if( (InSockFd = socket( AF_INET, SOCK_DGRAM, 0 )) == INVALID_SOCKET )
        OS_Error( 1, "could not create socket win32" );
#else
    if( (InSockFd = socket( AF_INET, SOCK_DGRAM, 0 )) < 0 )
        OS_Error( 1, "could not create socket" );
#endif
    
    /* clear it out */
    
    memset( &SockAddr, 0, sizeof( SockAddr ) );
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_addr.s_addr = htonl( INADDR_ANY );
    
    /* try to bind the socket */
    
    printf( "binding socket...\n" );

    if( port == 0 ) {
        SockAddr.sin_port = 0;
        if( bind( InSockFd, (struct sockaddr *) &SockAddr, sizeof( SockAddr ) ) ) {
            printf( "could not bind socket (wildcard port)" );
            return( 0 );
        }
    } else {
        for( i = 0; ((i < PORTS_TO_CHECK) && (i < 65536)); i++ ) {
            SockAddr.sin_port = htons( (ushort)(port+i) );
            if( bind( InSockFd, (struct sockaddr *) &SockAddr, sizeof( SockAddr ) ) )
                printf( "could not bind socket" );
            else
                break;
        }
    }
    
    if( (i == PORTS_TO_CHECK) || (i == 65536) ) {
        printf( "could not bind to a port in range %d to %d\n", port, port+i );
        printf( "could not bind to port\n" );
        return( 0 );
    }
    
    i = sizeof( SockAddr );
    getsockname( InSockFd, (struct sockaddr *) &SockAddr, &i );
    
    printf( "bound to port %d\n", ntohs( SockAddr.sin_port ) );
    
    set_fd_nonblocking( InSockFd );
    
    
#ifndef WIN32
    i = 1;
    setsockopt( InSockFd, SOL_SOCKET, SO_BSDCOMPAT, (const void*)(&i), sizeof( i ) );
#endif
    

    Started = 1;
    BytesIn = 0;
    BytesOut = 0;
    PacketsIn = 0;
    PacketsOut = 0;
    
    return( 1 );
    
} 





void Net_stop() {
    
    int result;
    
    printf( "stopping network driver\n" );
    
    if( !Started ) {
        return;
    }
    

#ifdef WIN32
    result = closesocket( InSockFd );
#else
    result = close( InSockFd );
#endif

    
    switch( result ) {
    case 0: /* success */
        break;
//    case EBADF: /* bad file descriptor.. happens if net already down */
//        printf( "bad file descriptor passed to close in Net_stop" );
//        break;
    default:
        printf( "unknown error from close in Net_stop" );
        break;
    }

    Started = 0;
}







void Net_in() {
    
    uint ipaddr;
    ushort port;
    uint len;

//    uint chunks;
//    uint leftover;

//    uint i;
    
    if( Started ) {
        len = Net_get_dat( TempDat, MAX_PACKET_LEN, &ipaddr, &port );
        if( len ) {

            //if( b_math_frand( 0, 1 ) < 0.1 ) {
            //    return;
            //}

			/*
            b_time_timestamp();

            b_msg_enqueue( M_NET_DATA );
            b_msg_enqueue( len );
            b_msg_enqueue( ipaddr );
            b_msg_enqueue( (uint) port );

            chunks = len >> 2;
            leftover = (len & 0x3) ? 1 : 0;

            for( i = 0; i < chunks + leftover; i++ ) {
                b_msg_enqueue( ((uint *)(TempDat))[i] );
            }
			*/

        }
    }
}




ushort Net_get_port() {
    return( ntohs( SockAddr.sin_port ) );
}



uint Net_get_bytes_in() {
    return( BytesIn );
}

uint Net_get_bytes_out() {
    return( BytesOut );
}



uint Net_get_packets_in() {
    return( PacketsIn );
}

uint Net_get_packets_out() {
    return( PacketsOut );
}



uint Net_send_dgram( uint ip, ushort port, const void * dat, uint len ) {
    
    int res = 0;
    struct sockaddr_in sockaddr;
    
    if( !Started ) {
        printf( "error: tried to send_dgram while network was stopped.\n" );
        return( 0 );
    }
    
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = htonl( ip );
    sockaddr.sin_port = htons( port );
    
    res = sendto( InSockFd, dat, len, 0, (struct sockaddr *) &sockaddr,
        sizeof( sockaddr ) );
    
#ifdef WIN32
    if( res == SOCKET_ERROR ) {
        printf( "error: %d\n", WSAGetLastError() );
        OS_Error( 1, "failed sending packet in send_dgram" );
    }
#else
    if( res < 0 ) {
        //printf( "errno: %d\n", errno );
        printf( "Sending to: %x:%d, %d bytes\n", ip, port, len );
        OS_Error( 1, "failed sending packet in send_dgram" );
    }
#endif
    
    BytesOut += res;
    PacketsOut++;

    if( res < (int)len ) {
        printf( "actual bytes sent less than expected in send_dgram" );
        return( 1 );
    }
    
    return( 0 );
    
}






// this turns an ip addr into dotted-decimal form.  str should be at least
// 16 chars.. (3*4+3+1).
void Net_addr_to_str( uint addr, char * str, uint len ) {
    
    //addr = b_sys_lend_int( addr );
    sprintf( str, "%d.%d.%d.%d",
        (addr >> 24), ((addr >> 16) & 0xFF), ((addr >> 8) & 0xFF), (addr & 0xFF) );

}





void Net_get_local_addr( uint * ipaddr ) {

    uint ip;
    ushort dummy;
    char hostname[256];

    if( gethostname( hostname, 256 ) == SOCKET_ERROR ) {
        printf( "gethostname() fails\n" );
        Net_addr_from_str( "localhost", &ip, &dummy );
    } else {
        hostname[255] = '\0';
        Net_addr_from_str( hostname, &ip, &dummy );
    }

    *ipaddr = ip;

}




/* this takes some string and gets a network address from it.  The string can
   be of the format [hostname|dotted-decimal][:port].  That is, it handles
   192.168.9.1:2760 specifications, as well as dukat.upl.cs.wisc.edu:2760
   specifications.  Port is optional. */
uint Net_addr_from_str( const char * str, uint * ipaddr, ushort * port ) {

    char * wrkstr;
    char * ptr1;
    char * ptr2;
    char * port_start;
    uint result;
    char dotted_decimal[16];
    struct hostent * host_ent = NULL;


    if( (str == NULL) || (*str == '\0') ) {
        return( 0 );
    }

    wrkstr = mem_malloc( strlen( str ) + 1 );

    memcpy( wrkstr, str, strlen( str ) + 1 );

    ptr1 = wrkstr;

    // trim leading whitespace.
    while( *ptr1 && ((*ptr1 == ' ') || (*ptr1 == '\t')) ) {
        ptr1++;
    }
    
    ptr2 = ptr1;

    if( (ptr2 == NULL) || (*ptr2 == '\0') ) {
        mem_free( wrkstr );
        return( 0 );
    }

    // did they specify a port? FIXME
    port_start = strchar( ptr1, ":" );

    // if so, replace the colon with a null, so the hostname is separate,
    // then march the pointer forward one, to point to the start of the
    // port.
    if( port_start != NULL ) {
        *port_start = '\0';
        port_start++;
    }

    //printf( "working with [%s] - [%s]\n", ptr2, port_start );

    // must determine whether the host is a dotted-decimal addr or a
    // hostname we must resolve.

    // assume a dotted decimal address, until we find a non-numeric char.
    // (oh, and non-dot too..)

    while( *ptr1 && ((*ptr1 == '.') || ( (*ptr1 >= '0') && (*ptr1 <= '9') )) ) {
        ptr1++;
    }
    
    if( *ptr1 == '\0' ) {
        // assume this is a dotted decimal, resolve that way.
        // (it may not be valid, but let the system find that out..)
        
        //printf( "dotted decimal format, calling inet_addr()\n" );

        result = inet_addr( ptr2 );
        if( result == INADDR_NONE ) {
            // not a valid address. just return.
            mem_free( wrkstr );
            return( 0 );
        }
        // otherwise, set the address in host order.
        *ipaddr = ntohl( result );
    } else {
        // assume this is a hostname to resolve.
       
        //printf( "hostname, calling resolver\n" );

        host_ent = gethostbyname( ptr2 );
        
        if( host_ent == NULL ) {
            /* could not resolve hostname */
            mem_free( wrkstr );
            return( 0 );
        }
        
        // take the first addr.
        *ipaddr = ntohl( *((uint *)host_ent->h_addr_list[0]) );
    }

    
    // now give the port some lovin, and we're done.

    if( port_start != NULL ) {
        *port = (ushort) atoi( port_start );
    }

    Net_addr_to_str( *ipaddr, dotted_decimal, 16 );

    //printf( "addr is: %s, port is %d\n", dotted_decimal, *port );

    mem_free( wrkstr );

    return( 1 );

}





uint Net_extract_int( const ubyte * dat, uint * index ) {

    *index += 4;
    return( *(uint *)(&(dat[(*index)-4])) );

}



ushort Net_extract_short( const ubyte * dat, uint * index ) {

    *index += 2;
    return( *(ushort *)(&(dat[(*index)-2])) );

}



float Net_extract_float( const ubyte * dat, uint * index ) {

    *index += 4;
    return( *(float *)(&(dat[(*index)-4])) );

}


ubyte Net_extract_byte( const ubyte * dat, uint * index ) {

    return( dat[(*index)++] );

}        



void Net_append_int( ubyte * dat, uint * index, int val ) {

    *(int *)(&(dat[(*index)])) = val;
    *index += 4;

}



void Net_append_short( ubyte * dat, uint * index, short val ) {

    *(short *)(&(dat[(*index)])) = val;
    *index += 2;

}



void Net_append_float( ubyte * dat, uint * index, float val ) {

    *(float *)(&(dat[(*index)])) = val;
    *index += 4;

}


void Net_append_byte( ubyte * dat, uint * index, byte val ) {

    dat[*index] = val;
    (*index)++;

}


void Net_append_String( ubyte * dat, uint * index, const char * str ) {

    do {
        Net_append_byte( dat, index, *str++ );
    } while( *str != '\0' );

}

#endif
