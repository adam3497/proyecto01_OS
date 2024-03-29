# Objetivo predeterminado
all: clean main run

# Reglas para compilar archivos .c en archivos .o
out/objects/huffman.o: huffman/huffman.c
	gcc -c $< -o $@

out/objects/freq.o: huffman/freq.c
	gcc -c $< -o $@

out/objects/file_utils.o: utilities/file_utils.c
	gcc -c $< -o $@

out/objects/main.o: main.c
	gcc -c $< -o $@

# Regla para enlazar los archivos .o y generar el ejecutable
main: out/objects/huffman.o out/objects/freq.o out/objects/file_utils.o out/objects/main.o
	gcc $^ -o $@

# Regla para limpiar los archivos generados
clean:
	rm -f out/frequencies/*
	rm -f out/bins/*
	rm -f out/decodes/*
	rm -f out/objects/*.o

run:
	./main