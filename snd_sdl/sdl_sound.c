#include <SDL_mixer.h>
#include "../os_sdl/s_sdl.h"
#include "../scommon/s_common.h"

#define MAX_SOUNDS 32

sboolean audioOpen;

/* If False no sounds will be played but all will still be loaded into memory */
sboolean bPlaySounds;

/* Holds the raw data */
Mix_Chunk *wavs[MAX_SOUNDS];

/* Holds the sounds struct */
sound_t *sounds[MAX_SOUNDS];
int soundcount;

int audio_rate;
Uint16 audio_format;
int audio_channels;

sound_t nullsound;

void Snd_Init()
{
	printf( "Initializing Sound\n" );

	//return;

	/* Initialize variables */
	audio_rate = MIX_DEFAULT_FREQUENCY;
	audio_format = MIX_DEFAULT_FORMAT;
	audio_channels = 2;

	/* Open Device */
	if( Mix_OpenAudio( audio_rate, audio_format, audio_channels, 4096 ) < 0 )
	{
		printf( "ERROR: Couldn't open audio device\n" );
	}
	else
	{
		printf( "Trying to query audio device\n" );
		Mix_QuerySpec( &audio_rate, &audio_format, &audio_channels );
		printf( "Successfully opened audio device\n" );
		printf("Opened audio at %d Hz %d bit %s\n", audio_rate, (audio_format&0xFF), (audio_channels > 1) ? "stereo" : "mono");
	}

	audioOpen = True;
	soundcount = 0;

	bPlaySounds = True;

	nullsound.soundid = -1;
}

void Snd_Shutdown( void )
{
	int i;

	//return;

	printf( "Shutting Down Sound\n" );

	/* Kill all the audio */
	for( i = 0; i < soundcount; i++ )
	{
		if( sounds[i] )
		{
			mem_free( sounds[i] );
			sounds[i] = NULL;
		}
		if( wavs[i] )
		{
			Mix_FreeChunk( wavs[i] );
			wavs[i] = NULL;
		}
	}

	/* Close audio device */
	if ( audioOpen ) {
		Mix_CloseAudio();
		audioOpen = 0;
	}
}

sound_t *Snd_LoadWAV( char *filename )
{
	int i;
	Mix_Chunk *mchunk;
	sound_t *wav;

	printf( "Loading Sound: %s\n", filename );
	
	// see if the audio is already loaded
	for( i = 0; i < soundcount; i++ )
	{
		if( strcmp( filename, sounds[i]->filename ) == 0 )
		{
			return sounds[i];
		}
	}

    mchunk = Mix_LoadWAV( filename );

	if( !mchunk )
	{
		printf( "ERROR: Couldn't load sound: %s\n", filename );
		return &nullsound;
	}
	
	/* Store the sound data */
	wavs[soundcount] = mchunk;

	wav = mem_malloc( sizeof( sound_t ) );

	strcpy( wav->filename, filename);
	wav->soundid = soundcount;

	sounds[soundcount] = wav;

	soundcount++;

	return wav;
}

/* Play a sound once */
void Snd_PlaySound( int soundid )
{
	if( soundid < 0 )
		return;
	
	Snd_PlaySoundLoop( soundid, 0 );
}

void Snd_PlaySoundLoop( int soundid, int loop )
{
	static int channel = 1;

	if( soundid < 0 )
		return;

	if( !bPlaySounds )
		return;

	Mix_PlayChannel( channel++, wavs[soundid], loop );

	if( channel >= 8 )
	{
		channel = 1;
	}

	//printf( "Channel: %i\n", channel );

	
}

void Snd_PlayMusicLoop( int soundid, int loop )
{
	if( !bPlaySounds )
		return;

	if( soundid < 0 )
		return;

	/* Deticate channel 0 to music */
	Mix_PlayChannel( 0, wavs[soundid], loop );
}
