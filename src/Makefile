all: build run clean

build:
	gcc -std=c99 util.c user_ns.c container.c mounts.c -o contain.out
run:
	./contain.out /bin/ash
clean:
	rm -f contain.out
