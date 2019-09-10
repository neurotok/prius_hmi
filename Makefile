CC = gcc 
src = $(wildcard *.c)
obj = $(src:.c=.o)

LDFLAGS =  -lm -lGL -lglfw

prius: $(obj)
	$(CC) -g  -Wall -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) prius
