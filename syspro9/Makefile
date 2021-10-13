CC := g++
#CFLAGS := -Wall
CFLAGS += -std=c++11 -g3
CFLAGS += -O0
LFLAGS := -lpthread

ODIR := .OBJ

MAIN_SOURCE_DEPS := main.cpp includes.hpp includes1.hpp lib.cpp main_bloom.cpp modules2.cpp
MODULES2_SOURCE_DEPS := modules2.cpp includes.hpp includes1.hpp
MAIN_BLOOM_SOURCE_DEPS := main_bloom.cpp includes.hpp includes1.hpp
MONITOR_SOURCE_DEPS := monitor.cpp includes.hpp includes1.hpp lib.cpp  main_bloom.cpp modules2.cpp
LIB_SOURCE_DEPS := lib.cpp includes.hpp includes1.hpp

all: binaries

$(ODIR):
	@mkdir $(ODIR)

binaries: monitorServer travelMonitorClient 

MONITOR_OBJECT_DEPS := $(ODIR)/monitor.o $(ODIR)/lib.o $(ODIR)/main_bloom.o $(ODIR)/modules2.o

monitorServer: $(ODIR) $(MONITOR_OBJECT_DEPS)
	$(CC) $(CFLAGS) $(MONITOR_OBJECT_DEPS) -o monitorServer $(LFLAGS)

MAIN_OBJECT_DEPS := $(ODIR)/main.o $(ODIR)/lib.o $(ODIR)/main_bloom.o $(ODIR)/modules2.o

travelMonitorClient: $(ODIR) $(MAIN_OBJECT_DEPS)
	$(CC) $(CFLAGS) $(MAIN_OBJECT_DEPS) -o travelMonitorClient $(LFLAGS)

$(ODIR)/main_bloom.o: $(ODIR) $(MAIN_SOURCE_DEPS)
	$(CC) -c $(CFLAGS) main_bloom.cpp -o $(ODIR)/main_bloom.o

$(ODIR)/modules2.o: $(ODIR) $(MAIN_SOURCE_DEPS)
	$(CC) -c $(CFLAGS) modules2.cpp -o $(ODIR)/modules2.o

$(ODIR)/main.o: $(ODIR) $(MAIN_SOURCE_DEPS)
	$(CC) -c $(CFLAGS) main.cpp -o $(ODIR)/main.o

$(ODIR)/monitor.o: $(ODIR) $(MONITOR_SOURCE_DEPS)
	$(CC) -c $(CFLAGS) monitor.cpp -o $(ODIR)/monitor.o

$(ODIR)/lib.o: $(ODIR) $(LIB_SOURCE_DEPS)
	$(CC) -c $(CFLAGS) lib.cpp -o $(ODIR)/lib.o

.PHONY: clean
clean:
	rm -rf .OBJ monitorServer travelMonitorClient main_bloom 
