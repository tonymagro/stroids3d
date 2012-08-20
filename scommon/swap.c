/*
============================================================================

					BYTE ORDER FUNCTIONS

============================================================================
*/

#include "s_shared.h"


sboolean BigEndian = False;


sboolean Swap_SysBigEndian() {

    char foo[2] = { 0, 1 };
    return( *((unsigned short *)foo) == 1 );

}

short Swap_Int16( short val ) {
    short temp;
    
    temp = ((val & 0xFF)   << 8) + 
           ((val & 0xFF00) >> 8);
           
    return temp;
    
}


int Swap_Int32( int val ) {
    int temp;
    
    temp = ((val & 0xFF)       << 24) +
        ((val & 0xFF00)     <<  8) +
        ((val & 0xFF0000)   >>  8) +
        ((val & 0xFF000000) >> 24);
        
    
    return temp;

}


float Swap_Float32( float val ) {

    union {
        float fl;
        unsigned char b[4];
    } foo, bar;

    foo.fl = val;

    bar.b[0] = foo.b[3];
    bar.b[1] = foo.b[2];
    bar.b[2] = foo.b[1];
    bar.b[3] = foo.b[0];
    
    
    return( bar.fl );
}


void File_ReadInt16( FILE *file, unsigned short *scanline, int length )
{
    fread(scanline, length, sizeof(unsigned short), file);
    if ( BigEndian ) {
        int i;
        for ( i = length-1; i >= 0; --i )
            scanline[i] = Swap_Int16( scanline[i] );
    }
}

void File_ReadInt32( FILE *file, unsigned int *scanline, int length )
{
    fread(scanline, length, sizeof(unsigned int), file);
    if ( BigEndian ) {
        int i;
        for ( i = length-1; i >= 0; --i )
            scanline[i] = Swap_Int32( scanline[i] );
    }
}

void File_ReadFloat32( FILE *file, float *scanline, int length )
{
    fread(scanline, length, sizeof(float), file);
    if ( BigEndian ) {
        int i;
        for ( i = length-1; i >= 0; --i )
            scanline[i] = Swap_Float32( scanline[i] );
    }
}

/*
================
Swap_Init
================
*/
void Swap_Init (void)
{
	BigEndian = Swap_SysBigEndian();
}

