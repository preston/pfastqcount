all: 
	cc pfastqcount.c -O3 -Wall -o pfastqcount

clean:
	rm pfastqcount
