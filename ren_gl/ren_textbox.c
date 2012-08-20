#include "ren_gl.h"

#define MAX_TEXTBOXES	10

typedef struct textBox_s
{
	vec4_t forecolor;
	vec4_t backcolor;
	float x;
	float y;
	float width;
	float height;
}textBox_t;

textBox_t textBoxes[MAX_TEXTBOXES];

int textBoxCount = 0;

int Ren_CreateTextBox( float x, float y, float width, float height, vec4_t fore_color, vec4_t backcolor )
{
	int count;

	count = textBoxCount;

	textBoxCount++;

	return count;
}

void Ren_DrawTextBox( float x, float y, float width, float height )
{

}

