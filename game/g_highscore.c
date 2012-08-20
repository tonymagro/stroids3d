#include "game.h"

typedef struct highscore_header_s
{
	char id[10];
	int version;
}highscore_header_t;

typedef struct highscore_s
{
	char	name0[15];
	char	name1[15];
	char	name2[15];
	char	name3[15];
	char	name4[15];
	char	name5[15];
	char	name6[15];
	char	name7[15];
	char	name8[15];
	char	name9[15];

	int		scores[10];
} highscore_t;

highscore_t highscores;

char * filename = "gamedata.dat";

void G_WriteDefaultScores( void );

/* This should read the high scores off of disk */
void G_InitHighScores( void )
{
	FILE* file;
	highscore_header_t hsh;

	printf( "Loading high scores\n" );

	if( (file = fopen( filename, "rb" ) ) == NULL )
	{
		printf( "ERROR: Couldn't load gamedata file: %s\nCreating Default\n", filename );
		G_WriteDefaultScores();
	} 

	file = fopen( filename, "rb" );

	if( !file )
	{
		printf( "Unable to create new gamedata file" );
		return;
	}

	fread( &hsh.id, sizeof(char), 10, file );
	fread( &hsh.version, sizeof(int), 1, file );

	printf( "Game Data file header:\n" );
	printf( " id = %s\n", hsh.id );
	printf( " version = %i\n", hsh.version );
	
	/*FIXME: Make Sure this is a highscore file */
	
	fread( highscores.name0, sizeof(char), 15, file );
	fread( highscores.name1, sizeof(char), 15, file );
	fread( highscores.name2, sizeof(char), 15, file );
	fread( highscores.name3, sizeof(char), 15, file );
	fread( highscores.name4, sizeof(char), 15, file );
	fread( highscores.name5, sizeof(char), 15, file );
	fread( highscores.name6, sizeof(char), 15, file );
	fread( highscores.name7, sizeof(char), 15, file );
	fread( highscores.name8, sizeof(char), 15, file );
	fread( highscores.name9, sizeof(char), 15, file );

	fread( &highscores.scores, sizeof(int), 10, file );

	printf( "HIGH SCORES\n" );
	printf( "1. %s", highscores.name0 );
	printf( "\t%i\n", highscores.scores[0] );
	printf( "2. %s", highscores.name1 );
	printf( "\t%i\n", highscores.scores[1] );
	printf( "3. %s", highscores.name2 );
	printf( "\t%i\n", highscores.scores[2] );
	printf( "4. %s", highscores.name3 );
	printf( "\t%i\n", highscores.scores[3] );
	printf( "5. %s", highscores.name4 );
	printf( "\t%i\n", highscores.scores[4] );
	printf( "6. %s", highscores.name5 );
	printf( "\t%i\n", highscores.scores[5] );
	printf( "7. %s", highscores.name6 );
	printf( "\t%i\n", highscores.scores[6] );
	printf( "8. %s", highscores.name7 );
	printf( "\t%i\n", highscores.scores[7] );
	printf( "9. %s", highscores.name8 );
	printf( "\t%i\n", highscores.scores[8] );
	printf( "10.%s", highscores.name9 );
	printf( "\t%i\n", highscores.scores[9] );

}

void G_WriteDefaultScores( void )
{
	FILE* file;
	highscore_header_t 	hsh;
	highscore_t 		highscore;

	strcpy( hsh.id, "GDF000000" );
	hsh.version = 1;

	strcpy( highscore.name0, "31137 HAXOR" );
	strcpy( highscore.name1, "31137 HAXOR" );
	strcpy( highscore.name2, "31137 HAXOR" );
	strcpy( highscore.name3, "31137 HAXOR" );
	strcpy( highscore.name4, "31137 HAXOR" );
	strcpy( highscore.name5, "31137 HAXOR" );
	strcpy( highscore.name6, "31137 HAXOR" );
	strcpy( highscore.name7, "31137 HAXOR" );
	strcpy( highscore.name8, "31137 HAXOR" );
	strcpy( highscore.name9, "31137 HAXOR" );

	highscore.scores[0] = 10000;
	highscore.scores[1] = 9000;
	highscore.scores[2] = 8000;
	highscore.scores[3] = 7000;
	highscore.scores[4] = 6000;
	highscore.scores[5] = 5000;
	highscore.scores[6] = 2000;
	highscore.scores[7] = 700;
	highscore.scores[8] = 200;
	highscore.scores[9] = 100;

	file = fopen( filename, "wb" );

	fwrite( &hsh.id, sizeof(char), 10, file );
	fwrite( &hsh.version, sizeof(int), 1, file );
	
	fwrite( &highscore.name0, sizeof(char), 15, file );
	fwrite( &highscore.name1, sizeof(char), 15, file );
	fwrite( &highscore.name2, sizeof(char), 15, file );
	fwrite( &highscore.name3, sizeof(char), 15, file );
	fwrite( &highscore.name4, sizeof(char), 15, file );
	fwrite( &highscore.name5, sizeof(char), 15, file );
	fwrite( &highscore.name6, sizeof(char), 15, file );
	fwrite( &highscore.name7, sizeof(char), 15, file );
	fwrite( &highscore.name8, sizeof(char), 15, file );
	fwrite( &highscore.name9, sizeof(char), 15, file );

	fwrite( &highscore.scores, sizeof(int), 10, file );

	fclose( file );
}

void G_EnterHiScore( void )
{

}
