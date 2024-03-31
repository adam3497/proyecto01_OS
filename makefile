# Objetivo predeterminado
all: clean compile

# Reglas para compilar archivos .c en archivos .o
compile:
	#
	gcc -c src/huffman/huffman.c -o out/objects/huffman 
	gcc -c src/huffman/freq.c -o out/objects/freq
	gcc -c src/utilities/file_utils.c -o out/objects/file_utils
	#
	gcc src/serial/compressor.c -o src/serial/compressor
	gcc src/serial/decompressor.c -o src/serial/decompressor
	#
	gcc -c src/fork/file_locks.c -o out/objects/file_locks
	gcc src/fork/compressor.c -o src/fork/compressor
	gcc src/fork/decompressor.c -o src/fork/decompressor

# Regla para limpiar los archivos generados
clean:
	rm -f out/frequencies/*
	rm -f out/bins/*
	rm -f out/decodes/*
	rm -f out/objects/*.o