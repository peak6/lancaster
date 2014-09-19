SRCS = \
advert.c \
clock.c \
dump.c \
dict.c \
error.c \
poller.c \
receiver.c \
sender.c \
signals.c \
sock.c \
storage.c \
table.c \
thread.c \
twist.c \
xalloc.c \
yield.c

BUILD_VERSION = $(shell git log --pretty=format:'%ad %h%d' --abbrev-commit --date=short -1)

CFLAGS = -ansi -pedantic -Wall -Wextra -Werror -g \
	-D_POSIX_C_SOURCE=200112L -D_BSD_SOURCE -DBUILD_VERSION='"$(BUILD_VERSION)"'
LDLIBS = -lm
OBJS = $(SRCS:.c=.o)

DEPFLAGS = \
-I/usr/include/linux \
-I/usr/include/x86_64-linux-gnu \
-I/usr/lib/gcc/x86_64-linux-gnu/4.6/include \
-I/usr/lib/gcc/x86_64-pc-cygwin/4.8.3/include

ifneq (,$(findstring Darwin,$(shell uname -s)))
CFLAGS += -D_DARWIN_C_SOURCE
else
CFLAGS += -pthread
LDFLAGS += -pthread
LDLIBS += -lrt
endif

ifneq (,$(findstring CYGWIN,$(shell uname -s)))
SO_EXT = .dll
else
CFLAGS += -fPIC
SO_EXT = .so
endif

all: libcachester$(SO_EXT) publisher subscriber reader writer inspector grower components

#
# These are the component libraries that need to be built
#
COMPONENTS = submgr
.PHONY  : components $(COMPONENTS)

components: $(COMPONENTS)

$(COMPONENTS):
	$(MAKE) -C $@

release: CFLAGS += -DNDEBUG -O3
release: all

debug: CFLAGS += -DDEBUG_PROTOCOL
debug: all

publisher: publisher.o libcachester.a

subscriber: subscriber.o libcachester.a

reader: reader.o libcachester.a

writer: writer.o libcachester.a

inspector: inspector.o libcachester.a

grower: grower.o libcachester.a

libcachester.a: $(OBJS)
	ar -r libcachester.a $(OBJS)

libcachester$(SO_EXT): $(OBJS)
	$(CC) -shared $(LDFLAGS) $^ $(LDLIBS) -o $@

DEPEND.mk:
	touch DEPEND.mk

depend: DEPEND.mk
	makedepend -f DEPEND.mk $(DEPFLAGS) -DMAKE_DEPEND -- $(CFLAGS) -- \
	    writer.c publisher.c subscriber.c reader.c inspector.c grower.c \
	    $(SRCS)

clean:
	rm -rf libcachester.a libcachester$(SO_EXT) \
	    writer writer.o publisher publisher.o \
	    subscriber subscriber.o reader reader.o \
		inspector.o inspector grower.o grower \
		reader.dSYM writer.dSYM \
		publisher.dSYM subscriber.dSYM \
		inspector.dSYM grower.dSYM \
	    $(OBJS)
	@ cd submgr; $(MAKE) clean

distclean: clean
	rm -f DEPEND.mk *~ *.bak core core.* *.stackdump

.PHONY: all release debug depend clean distclean

include DEPEND.mk
