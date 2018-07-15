all:
	gcc -o floristApp 131044013_main.c -lpthread -lrt -lm
clean:
	rm floristApp
