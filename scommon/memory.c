/* Alex Mohr's memory allocation code from inferno */

#include "s_common.h"


/* this is all stuff for the malloc instrumenting stuff */
/* i may want to do a full malloc implementation later, either based on
   malloc or using the underlying syscall to get more mem */
#define HASH_KEEP_BITS   (16)
#define HASH_SIZE        (1 << HASH_KEEP_BITS)
#define HASH_IGNORE_BITS (2)
#define HASH_MASK        (~(0xFFFFFFFF << (HASH_IGNORE_BITS + HASH_KEEP_BITS)))


#define MAX_MEM_FILE    (128)

typedef struct _hashnode_t {
    unsigned long long pointer;
    unsigned int size;
    unsigned int line;
    char file[MAX_MEM_FILE];
    struct _hashnode_t * next;
} hashnode_t;



static hashnode_t * MemHash[HASH_SIZE];
static unsigned int TotalBytes;
static unsigned int TotalAllocs;
static unsigned int Collisions;



#define HASH_PTR(x)     ((((unsigned int)(x)) & HASH_MASK) >> HASH_IGNORE_BITS)



/* computes the hashed value of pointer */
static void hash_insert( void * ptr, unsigned int size, unsigned int line, const char * fname ) {

    hashnode_t * h;
    unsigned int hashpos;

    if( ptr == NULL ) {
        OS_Error( 1, "tried to hash null pointer\n" );
    }

    hashpos = HASH_PTR(ptr);

    // first time?
    if( MemHash[hashpos] == NULL ) {
        MemHash[hashpos] = malloc( sizeof( hashnode_t ) );
        if( MemHash[hashpos] == NULL  ) {
            OS_Error( 1, "out of memory!\n" );
        }
        MemHash[hashpos]->next = NULL;
        h = MemHash[hashpos];
    } else {

        h = MemHash[hashpos];

        /* collision */
        ++Collisions;

        // march down the linked list until the end.
        while( h->next != NULL ) {
            h = h->next;
        }

        // allocate a new node on the end.
        h->next = malloc( sizeof( hashnode_t ) );
        if( h->next == NULL ) {
            OS_Error( 1, "out of memory!\n" );
        }

        // move to node we just created
        h = h->next;

        // make sure this terminates the list.
        h->next = NULL;
    }

    h->pointer = (unsigned long long)ptr;
    h->line = line;
    h->size = size;
    strncpy( h->file, fname, MAX_MEM_FILE );

    TotalBytes += size;
    ++TotalAllocs;

}



/* computes the hashed value of pointer */
static void hash_remove( void * ptr ) {

    int hashpos;
    hashnode_t *h, *prev = NULL;

    if( ptr == NULL ) {
        OS_Error( 1, "tried to free a null pointer\n" );
    }

    hashpos = HASH_PTR(ptr);

    h = MemHash[hashpos];

    if( h == NULL ) {
        // this is wrong -- trying to free a pointer that was
        // never hashed.
        OS_Error( 1, "tried to a free an unallocated pointer\n" );
        return;
    }

    while( h->pointer != (unsigned long long)ptr ) {
        if( h->next == NULL ) {
            // wrong again -- freeing a pointer never hashed
            OS_Error( 1, "tried to a free an unallocated pointer\n" );
            return;
        }
        prev = h;
        h = h->next;
    }

    // guaranteed to have the right one now.

    TotalBytes -= h->size;
    --TotalAllocs;

    free( (void *)(h->pointer) );

    // if was first in list..
    if( prev == NULL ) {
        prev = h;
        MemHash[hashpos] = h->next;
        free( h );
    } else {
        // works even in end case
        prev->next = h->next;
        free( h );
    }

}




static hashnode_t * hash_lookup( void * ptr ) {

    int hashpos;
    hashnode_t * h;

    if( ptr == NULL ) {
        printf( "hash_lookup() - null pointer\n" );
        return( NULL );
    }

    hashpos = HASH_PTR(ptr);

    if( MemHash[hashpos] == NULL ) {
        // pointer never allocated
        return( NULL );
    }

    h = MemHash[hashpos];

    while( h->pointer != (unsigned long long)ptr ) {
        if( h->next == NULL ) {
            // again, never allocated
            return( NULL );
        }
        h = h->next;
    }

    // now guaranteed to have right node

    return( h );

}





void * mem_allocate( unsigned int size, unsigned int line, const char * file ) {

    void * ret;

    ret = malloc( size );
    if( ret == NULL ) {
        OS_Error( 1, "out of memory in mem_malloc()\n" );
    }

    hash_insert( ret, size, line, file );

    return( ret );

}



void * mem_reallocate( void * ptr, unsigned int size, unsigned int line, const char * file ) {

    hashnode_t * h;
    void * ret;

    if( (h = hash_lookup( ptr )) == NULL ) {
        OS_Error( 1, "mem_realloc() with unallocated pointer" );
    }

    ret = mem_allocate( size, line, file );

    memcpy( ret, ptr, size <= h->size ? size : h->size );
    mem_free( ptr );

    return( ret );

}



void mem_free( void * ptr ) {

    if( ptr == NULL ) {
        printf( "tried to free a null pointer!\n" );
        return;
    }

    hash_remove( ptr );

}





void mem_swap_in( unsigned int size, unsigned char * buf ) {

    // run around in the memory and touch a bunch of things --
    // read and write, to try and force this chunk into physical
    // mem.
	/*

    unsigned int i;
    unsigned int index;
    unsigned char save_byte;

    printf( "trying to swap in %d bytes\n", size );

    for( i = 0; i < (size >> 3); i++ ) {

        index = b_math_irand() % size;

        // read (save), write, replace...
        save_byte = buf[index];
        buf[index] = index & 0xFF;
        buf[index] = save_byte;

        save_byte = buf[index>>1];
        buf[index>>1] = index & 0xFF;
        buf[index>>1] = save_byte;

        save_byte = buf[index>>3];
        buf[index>>3] = index & 0xFF;
        buf[index>>3] = save_byte;

    }
	*/

}







unsigned int mem_malloc_bytes() {
    return( TotalBytes );
}


unsigned int mem_malloc_number() {
    return( TotalAllocs );
}




void mem_init() {
    //b_mem_swap_in( sizeof( MemHash ), (UByte *)MemHash );

#ifdef WIN32
    // turn down malloc granularity
    //_amblksiz = 1024;
#endif

}



void mem_shutdown() {

    int i, count = 0;
    hashnode_t * h;

    char msgbuf[256];

	/* Print Memory usage */
	printf( "MEMORY: Shutting Down - %i bytes allocated from %i allocs\n", mem_malloc_bytes(), mem_malloc_number() );

    if( TotalBytes || TotalAllocs ) {
        sprintf( msgbuf,
            "Leaked %d bytes of memory with %d unfreed allocations!",
            TotalBytes, TotalAllocs );
        printf( "%s\n", msgbuf );

        printf( "Memory Leaked!\n" );

        // print out some more meaningful things to the console.

        printf( "Unfreed Memory\n---------------\n" );
        for( i = 0; i < HASH_SIZE; i++ ) {
            if( (MemHash[i] != NULL) ) {
                h = MemHash[i];
                while( h != NULL ) {
                    printf( "Pointer 0x%X with %d bytes @ line %d in %s\n",
                        (unsigned int)h->pointer, h->size, h->line, h->file );
                    h = h->next;
                    if( ++count >= 32 ) {
                        return;
                    }
                }
            }
        }
    }
}

