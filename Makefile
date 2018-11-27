CC = gcc
#CFLAGS = -g
CFLAGS = -Wall -g
#LDFLAGS = -lm

PROGRAMS =  743588_ED2_T03B

all: $(PROGRAMS)

clean:
	rm -f *.o *~ $(PROGRAMS)
