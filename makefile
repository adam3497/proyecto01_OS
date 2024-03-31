# Objetivo predeterminado
all: clean compile

# Reglas para compilar archivos .c en archivos .o
compile:
	#
	gcc -c src/huffman/huffman.c -o out/objects/huffman
	gcc -c src/huffman/freq.c -o out/objects/freq
	gcc -c src/utilities/file_utils.c -o out/objects/file_utils
	#
	gcc src/serial/compressor.c -o out/objects/serial_compressor
	gcc src/serial/decompressor.c -o out/objects/serial_decompressor
	#
	gcc -c src/fork/file_locks.c -o out/objects/file_locks
	gcc src/fork/compressor.c -o out/objects/fork_compressor
	gcc src/fork/decompressor.c -o out/objects/fork_decompressor

# Regla para limpiar los archivos generados
clean:
	rm -f out/frequencies/*
	rm -f out/bin/*
	rm -f out/decodes/*
	rm -f out/objects/*