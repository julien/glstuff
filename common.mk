CC=clang

CFLAGS = -Wall -Wextra -pedantic 
CFLAGS += -I$(HOME)/.local/src/include -I../include
CFLAGS += -L$(HOME)/.local/src/lib $(HOME)/.local/src/lib/libglfw3.a -lglfw3
ifeq ($(shell uname -s), Darwin)
	CFLAGS += -framework OpenGL -framework Cocoa \
			-framework IOKit -framework CoreFoundation
endif

bin ?= app
src ?= $(shell find *.c -type f)

all: $(bin)

$(bin): $(src)
	$(CC) $(CFLAGS) -o $@ $^

clean:
ifneq (,$(wildcard $(bin)))
	@rm $(bin)
endif

debug: CFLAGS += -DDEBUG -g
debug: clean $(bin);
