CC := g++
CFLAGS := -Wall
CFLAGS += -g3

ODIR := .OBJ

UI_SOURCE_DEPS := classes.hpp
CLASSES_SOURCE_DEPS := classes.hpp
MAIN_SOURCE_DEPS := classes.hpp 

all: binaries

$(ODIR):
	@mkdir $(ODIR)

binaries: mngstd 

MAIN_OBJECT_DEPS := $(ODIR)/main.o $(ODIR)/classes.o $(ODIR)/UI.o

mngstd: $(ODIR) $(MAIN_OBJECT_DEPS)
	$(CC) $(CFLAGS) $(MAIN_OBJECT_DEPS) -o mngstd 

$(ODIR)/UI.o: $(ODIR) $(UI_SOURCE_DEPS)
	$(CC) -c $(CFLAGS) UI.cpp -o $(ODIR)/UI.o

$(ODIR)/classes.o: $(ODIR) $(CLASSES_SOURCE_DEPS)
	$(CC) -c $(CFLAGS) classes.cpp -o $(ODIR)/classes.o

$(ODIR)/main.o: $(ODIR) $(MAIN_SOURCE_DEPS)
	$(CC) -c $(CFLAGS) main.cpp -o $(ODIR)/main.o

clean:
	rm -rf .OBJ mngstd 
