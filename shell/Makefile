#---------------------------------------------------------------------
# Makefile
# Author: Jinyuan Qi
#---------------------------------------------------------------------
# Macros
CC = gcc217
# CC = gcc217m
CFLAGS =
# CFLAGS = -g
# CFLAGS = -D NDEBUG
# CFLAGS = -D NDEBUG -O

#---------------------------------------------------------------------
# Build rules for non-file targets
#---------------------------------------------------------------------

all: ishlex ishsyn ish

clean:
	rm -f ishlex ishsyn ish *.o

#---------------------------------------------------------------------
# Dependency rules for file targets
#---------------------------------------------------------------------
ish: dynarray.o lexAnalyzer.o command.o synAnalyzer.o readline.o ish.o
		$(CC) $(CFLAGS) dynarray.o lexAnalyzer.o command.o \
		synAnalyzer.o readline.o ish.o -o ish

ishsyn: dynarray.o  lexAnalyzer.o   command.o \
	synAnalyzer.o  ishsyn.o  readline.o
		$(CC) $(CFLAGS) dynarray.o  lexAnalyzer.o command.o \
		synAnalyzer.o  ishsyn.o readline.o -o ishsyn

ishlex: dynarray.o  lexAnalyzer.o   ishlex.o  readline.o
		$(CC) $(CFLAGS) dynarray.o lexAnalyzer.o ishlex.o \
		readline.o -o ishlex
 
# Partial Builds of .o files
dynarray.o: dynarray.c dynarray.h
		$(CC) $(CFLAGS) -c dynarray.c

lexAnalyzer.o: lexAnalyzer.c dynarray.h  lexAnalyzer.h  ishlex.h
		$(CC) $(CFLAGS) -c lexAnalyzer.c

ishlex.o: ishlex.c dynarray.h lexAnalyzer.h  ishlex.h readline.h
		$(CC) $(CFLAGS) -c ishlex.c

command.o: command.c dynarray.h ishsyn.h
		$(CC) $(CFLAGS) -c command.c 

synAnalyzer.o: synAnalyzer.c dynarray.h lexAnalyzer.h synAnalyzer.h \
		ishsyn.h 
		$(CC) $(CFLAGS) -c  synAnalyzer.c 

ishsyn.o:  ishsyn.c dynarray.h lexAnalyzer.h  command.h  synAnalyzer.h \
           ishsyn.h  readline.h
		$(CC) $(CFLAGS) -c ishsyn.c

ish.o: ish.c dynarray.h lexAnalyzer.h command.h synAnalyzer.h ish.h \
       readline.h
		$(CC) $(CFLAGS) -c ish.c

