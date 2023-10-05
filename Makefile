CC = gcc
CFLAGS = -Wall

tu_shell: p1.c
	$(CC) $(CFLAGS) -o mishell p1.c
	./mishell

clean:
	rm -f mishell