// Quake 2
#include "s_common.h"


void Buf_Init( buffer_t* buf, byte* data, int length )
{
	memset( buf, 0, sizeof( *buf ) );
	buf->data = data;
	buf->maxsize = length;
}

void Buf_Clear( buffer_t* buf )
{
	buf->currentsize = 0;
	buf->overflow = False;
}

void* Buf_GetSpace( buffer_t* buf, int length )
{
	void*	data;

	// See if buffers current size plus added data is too large for the buffer
	if( buf->currentsize + length > buf->maxsize )
	{
		if( !buf->allowoverflow )
			OS_Error( 1, "Buf_GetSpace - Fatal Buffer Overflow\n" );
		if( length > buf->maxsize )
			OS_Error( 1, "Buf_GetSpace - Length is Greater then Buffer Size\n" );

		printf( "Buf_GetSpace - Non-Fatal Buffer Overflow" );

		Buf_Clear( buf );

		buf->overflow = True;
	}

	data = buf->data + buf->currentsize;
	buf->currentsize += length;

	return data;
}

void Buf_Write( buffer_t* buf, void* data, int length )
{
	memcpy( Buf_GetSpace( buf, length ), data, length);
}
