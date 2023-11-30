CC := gcc
CFLAGS := -Wall
CFLAGS += -g3
CFLAGS +=-lpthread
ODIR := .OBJ
H_DEPS := includes.h
OBJECT_DEPS :=
all: chef saladmaker
$(ODIR):
	@mkdir $(ODIR)
chef: chef.c $(OBJECT_DEPS)
	$(CC) chef.c $(OBJECT_DEPS) $(CFLAGS) -o chef

saladmaker: saladmaker.c $(OBJECT_DEPS)
	$(CC) saladmaker.c $(OBJECT_DEPS)  $(CFLAGS) -o saladmaker
clean:
	rm -f chef saladmaker *.o
run:
	./chef -n 10 -m 3.5
