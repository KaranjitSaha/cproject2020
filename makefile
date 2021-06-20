CC = gcc
CFLAGS = -lm -I.
DEPS = bmptoico.h
OBJ = main.o ReadBMP.o WriteBMPtoICO.o

%.o: %.c $(DEPS)
	$(CC) -c $< $(CFLAGS)

bmptoico : $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -rf *.o bmptoico
