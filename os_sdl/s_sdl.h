#include "SDL.h"

void Input_UpdateJoyStick( void );
void Input_UpdateMouse( void );

/*
typedef struct scanascii_item_s
{
	int scancode;
	char ascii;
	char name[64];
} scanascii_item_t;

char scan2ascii( int scancode )
{
	return scantable[
}

scanascii_item_t scantable[] = {

{SDLK_BACKSPACE,'\b',"backspace" },
{SDLK_TAB,		'\t',"tab" },
//SDLK_CLEAR   clear 
{SDLK_RETURN,	'\r',"return" },
//SDLK_PAUSE   pause 
//{SDLK_ESCAPE,	'^[',"escape" }, 
{SDLK_SPACE,	' ',"space" }, 
{SDLK_EXCLAIM,	'!' ,"exclaim" },
{SDLK_QUOTEDBL,	'"' ,"quotedbl"},
{SDLK_HASH,		'#' ,"hash"},
{SDLK_DOLLAR,	'$' ,"dollar"},
{SDLK_AMPERSAND,'&' ,"ampersand"},
{SDLK_QUOTE,	'\'' ,"quote"},
{SDLK_LEFTPAREN,'(' ,"left parenthesis" },
{SDLK_RIGHTPAREN,')' ,"right parenthesis" },
{SDLK_ASTERISK,	'*' ,"asterisk" },
{SDLK_PLUS,		'+' ,"plus sign"},
{SDLK_COMMA,	',' ,"comma"},
{SDLK_MINUS,	'-' ,"minus sign" },
{SDLK_PERIOD,	'.' ,"period"},
{SDLK_SLASH,	'/' ,"forward slash"},
{SDLK_0,'0' ,"0"},
{SDLK_1,'1' ,"1"},
{SDLK_2,'2' ,"2"},
{SDLK_3,'3' ,"3"},
{SDLK_4,'4' ,"4"},
{SDLK_5,'5' ,"5"},
{SDLK_6,'6' ,"6"},
{SDLK_7,'7' ,"7"},
{SDLK_8,'8' ,"8"},
{SDLK_9,'9' ,"9"},
{SDLK_COLON,	':' ,"colon"},
{SDLK_SEMICOLON,';' ,"semicolon"},
{SDLK_LESS,		'<' ,"lessthan sign"},
{SDLK_EQUALS,	'=' ,"equals sign"},
{SDLK_GREATER,	'>' ,"greaterthan sign"},
{SDLK_QUESTION,	'?' ,"question mark"},
{SDLK_AT,		'@' ,"at" },
{SDLK_LEFTBRACKET,'[' ,"left bracket"},
{SDLK_BACKSLASH,'\\' ,"backslash"},
{SDLK_RIGHTBRACKET,']' ,"right bracket"},
{SDLK_CARET,	'^' ,"caret"},
{SDLK_UNDERSCORE,'_' ,"underscore"},
{SDLK_BACKQUOTE, '`',"grave"}, 
{SDLK_a,'a' ,"a"},
{SDLK_b,'b' ,"b"},
{SDLK_c,'c' ,"c"},
{SDLK_d,'d' ,"d"},
{SDLK_e,'e' ,"e"},
{SDLK_f,'f' ,"f"},
{SDLK_g,'g' ,"g"},
{SDLK_h,'h' ,"h"},
{SDLK_i,'i' ,"i"},
{SDLK_j,'j' ,"j"},
{SDLK_k,'k' ,"k"},
{SDLK_l,'l' ,"l"},
{SDLK_m,'m' ,"m"},
{SDLK_n,'n' ,"n"},
{SDLK_o,'o' ,"o"},
{SDLK_p,'p' ,"p"},
{SDLK_q,'q' ,"q"},
{SDLK_r,'r' ,"r"},
{SDLK_s,'s' ,"s"},
{SDLK_t,'t' ,"t"},
{SDLK_u,'u' ,"u"},
{SDLK_v,'v' ,"v"},
{SDLK_w,'w' ,"w"},
{SDLK_x,'x' ,"x"},
{SDLK_y,'y' ,"y"},
{SDLK_z,'z' ,"z"},
//{SDLK_DELETE,'^?' ,"delete"},
//SDLK_KP0   keypad 0 
//SDLK_KP1   keypad 1 
//SDLK_KP2   keypad 2 
//SDLK_KP3   keypad 3 
//SDLK_KP4   keypad 4 
//SDLK_KP5   keypad 5 
//SDLK_KP6   keypad 6 
//SDLK_KP7   keypad 7 
//SDLK_KP8   keypad 8 
//SDLK_KP9   keypad 9 
{SDLK_KP_PERIOD,	'.' ,"keypad period"},
{SDLK_KP_DIVIDE,	'/' ,"keypad divide"},
{SDLK_KP_MULTIPLY,	'*' ,"keypad multiply"},
{SDLK_KP_MINUS,		'-' ,"keypad minus"},
{SDLK_KP_PLUS,		'+' ,"keypad plus"},
{SDLK_KP_ENTER,		'\r',"keypad enter"},
{SDLK_KP_EQUALS,	'=' ,"keypad equals"}

};

*/

/*
SDLK_UP   up arrow 
SDLK_DOWN   down arrow 
SDLK_RIGHT   right arrow 
SDLK_LEFT   left arrow 
SDLK_INSERT   insert 
SDLK_HOME   home 
SDLK_END   end 
SDLK_PAGEUP   page up 
SDLK_PAGEDOWN   page down 
SDLK_F1   F1 
SDLK_F2   F2 
SDLK_F3   F3 
SDLK_F4   F4 
SDLK_F5   F5 
SDLK_F6   F6 
SDLK_F7   F7 
SDLK_F8   F8 
SDLK_F9   F9 
SDLK_F10   F10 
SDLK_F11   F11 
SDLK_F12   F12 
SDLK_F13   F13 
SDLK_F14   F14 
SDLK_F15   F15 
SDLK_NUMLOCK   numlock 
SDLK_CAPSLOCK   capslock 
SDLK_SCROLLOCK   scrollock 
SDLK_RSHIFT   right shift 
SDLK_LSHIFT   left shift 
SDLK_RCTRL   right ctrl 
SDLK_LCTRL   left ctrl 
SDLK_RALT   right alt 
SDLK_LALT   left alt 
SDLK_RMETA   right meta 
SDLK_LMETA   left meta 
SDLK_LSUPER   left windows key 
SDLK_RSUPER   right windows key 
SDLK_MODE   mode shift 
SDLK_HELP   help 
SDLK_PRINT   print-screen 
SDLK_SYSREQ   SysRq 
SDLK_BREAK   break 
SDLK_MENU   menu 
SDLK_POWER   power 
SDLK_EURO   euro
*/ 

