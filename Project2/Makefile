CC = gcc
CFLAGS= -Wall

objects = myshell.o utility.o

myshell: $(objects)
	$(CC) $(CFLAGS) -o myshell $(objects)

myshell.o: myshell.c myshell.h
	$(CC) $(CFLAGS) -c myshell.c
