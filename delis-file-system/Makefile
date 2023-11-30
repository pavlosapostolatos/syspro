CC := gcc
CFLAGS := -Wall
CFLAGS += -g3
#CFLAGS +=-lpthread
ODIR := .OBJ
H_DEPS := includes.h
MAIN_OBJECT_DEPS := $(ODIR)/main.o $(ODIR)/testcase.o
all: quic
$(ODIR):
	@mkdir $(ODIR)
$(ODIR)/testcase.o: $(ODIR) $(H_DEPS)
	$(CC) -c $(CFLAGS) testcase.c -o $(ODIR)/testcase.o
$(ODIR)/main.o: $(ODIR) $(H_DEPS)
	$(CC) -c $(CFLAGS) main.c -o $(ODIR)/main.o
quic: $(ODIR) $(MAIN_OBJECT_DEPS)
	$(CC)  $(CFLAGS) $(MAIN_OBJECT_DEPS) -o quic
clean:
	rm -rf $(ODIR) quic
# run:
# 	./chef -n 10 -m 3.5
