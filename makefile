# Objetivo predeterminado
all: clean serial fork pthread

# Regla para compilar huffman y file utils
huffman:
	gcc -c src/huffman/huffman.c -o out/objects/huffman
	gcc -c src/huffman/freq.c -o out/objects/freq
	gcc -c src/utilities/file_utils.c -o out/objects/file_utils

# Regla para compilar programa serial
serial:
	gcc src/serial/compressor.c -o out/objects/serial_compressor
	gcc src/serial/decompressor.c -o out/objects/serial_decompressor

# Regla para compilar programa fork
fork:
	gcc src/fork/compressor.c -o out/objects/fork_compressor
	gcc src/fork/decompressor.c -o out/objects/fork_decompressor

# Regla para compilar programa con pthreads
pthread:
	gcc src/pthreads/compressor.c -o out/objects/pthread_compressor
	gcc src/pthreads/decompressor.c -o out/objects/pthread_decompressor

# Regla para limpiar los archivos generados
clean:
	rm -f out/frequencies/*
	rm -f out/bin/*
	rm -f out/books/*
	rm -f out/decodes/*
	rm -f out/objects/*
	rm -f out/temp/*