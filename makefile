all: proyecto_3

proyecto_3: proyecto_3.c
	gcc -o proyecto_3 proyecto_3.c `pkg-config --cflags --libs gtk+-3.0 cairo`

clean:
	rm -f ./proyecto_3 || true

