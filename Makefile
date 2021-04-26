all: CPUsim.c
	gcc -g -Wall -o sim CPUsim.c

clean:
	$(RM) sim