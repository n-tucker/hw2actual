#Makefile

CC = gcc
CFLAGS = -Wall

CSRC = hw2.c
HSRC = hw2.h
OBJ = $(CSRC:.c=.o)

%o:%c $(HSRC)
	$(CC) $(CFLAGS) -c $<

        
# Additional targets
.PHONY: clobber
.PHONY: clean
.PHONY: test

# Target rules

hw2: $(OBJ)
	$(CC) $(CFLAGS) -o hw2 $(OBJ)

clobber:
	rm -f $(OBJ) cipher

clean:
	rm -f $(OBJ)

test:
	./cipher
