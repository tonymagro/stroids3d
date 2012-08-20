# Generated automatically from Makefile.in by configure.
# AutoConf makefile template by Tony Magro
#

# where to install the binaries
prefix=/usr/local
exec_prefix=${prefix}
BINDIR=${exec_prefix}/bin

# where to install the man page
MANDIR=${prefix}/man

CC		= gcc
EXE		= @EXE@


# Main Target
NAME = stroids

GPROF_OPTS = 
CC = gcc
CFLAGS = $(GPROF_OPTS) -g -O2 `sdl-config --cflags`
LDFLAGS = $(GPROF_OPTS)

LIBS = -lGL -lGLU `sdl-config --libs` -lSDL_mixer

GAME     = game/g_particles.c game/g_player.c game/g_stroids.c game/g_stars.c \
           scommon/s_shared.c scommon/command.c scommon/buffer.c scommon/s_common.c \
           game/g_powerup.c game/g_hud.c game/g_keys.c game/g_intro.c \
           game/e_spacestation.c game/g_highscore.c game/g_projectile.c game/g_menu.c game/g_limbo.c

ENGINE   = game/g_main.c snd_sdl/sdl_sound.c scommon/libtarga.c scommon/s_math.c \
           os_sdl/os_sdl.c  scommon/milkshape.c scommon/swap.c scommon/memory.c \
           os_sdl/os_sdl_input.c scommon/filesystem.c scommon/cvariables.c \
           scommon/cmd_buffer.c server/server_main.c client/client_main.c client/console.c

RENDERER = ren_gl/ren_gl.c ren_gl/ren_draw.c ren_gl/ren_model.c ren_gl/ren_texture.c ren_gl/ren_textbox.c

SOURCES  = $(ENGINE) $(RENDERER) $(GAME)

OBJS 	 = $(SOURCES:.c=.o)


# Code compilation

all: $(NAME)
	
$(NAME): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS) $(CFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

#ctags: 
#	-ctags *.c *.h $(tagsincludedir)/GL/*.h $(tagsincludedir)/SDL/*.h

test: all
	-./$(NAME)


clean:
	-rm -f $(OBJS)

