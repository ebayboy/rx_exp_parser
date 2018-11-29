
EXE?= test

INCL := 
LIB_NAME := 

all: app

DEBUG = -g -ggdb
CFLAGS = -lstdc++ -lm

app: 
	$(CC) $(CFLAGS) $(DEBUG) main.c exp_parser.c $(INCL) $(LIB_NAME) -o $(EXE)

clean:
	rm -rf $(EXE)
