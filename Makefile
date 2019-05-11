CC = gcc
CFLAGS = -Wall -O2
OBJFILES = Auxiliares

all: ma sv cv ag

ma: $(OBJFILES)
	$(CC) $(CFLAGS) -o ma ma.c Auxiliares.o

sv: $(OBJFILES)
	$(CC) $(CFLAGS) -o sv sv.c Auxiliares.o

cv: $(OBJFILES)
	$(CC) $(CFLAGS) -o cv cv.c Auxiliares.o

ag: $(OBJFILES)
	$(CC) $(CFLAGS) -o ag ag.c Auxiliares.o

Auxiliares:
	$(CC) $(CFLAGS) -c Auxiliares.c