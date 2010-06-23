
SHELL	= /bin/sh
VPATH	= lib

# Large_File = -D_FILE_OFFSET_BITS=64

DIET	= diet -Os
CC	= $(DIET) gcc -I.
CFLAGS	= -Os -g -Wall -W $(Large_File)
LDFLAGS	= -s 
LIBS	= -lowfat -lpthread
STR	= strip -R .comment -R .note
LS	= ls -l

ALL =  rash

all: lib.a $(ALL)

%.o: %.c
	$(CC) $(CFLAGS) -c $^



rash: rash.c lib.a
	$(CC) $(CFLAGS) $(LDFLAGS)  $^ $(LIBS) -o $@
	
ex_%: ex_%.c lib.a
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ -lowfat

strip: $(ALL)
	$(STR) $^

show_obj: $(ALL)
	objdump -x $^

idx:
	make clean
	mkhtmlindex -m *

lib.a: linked_list.o path.o path_misc.o execution.o 
	ar cr $@ $^

clean:
	rm -f a.out *.a lib/*.o *.o *~ $(ALL) *.tar.bz2 *.tar.gz Z*

ME=$(notdir $(CURDIR))
tgz:
	make clean
	cd .. && tar --owner=root --group=root \
 --exclude $(ME)/lect_7 --exclude $(ME)/z \
 -cvzf $(ME).tar.gz $(ME)
