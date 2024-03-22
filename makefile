# Objetivo predeterminado
all: main

# Reglas para compilar archivos .c en archivos .o
out/file_utils.o: utilities/file_utils.c
	gcc -c $< -o $@

out/freq.o: huffman/freq.c
	gcc -c $< -o $@

out/main.o: main.c
	gcc -c $< -o $@

# Regla para enlazar los archivos .o y generar el ejecutable
main: out/file_utils.o out/freq.o out/main.o
	gcc $^ -o $@

# Regla para limpiar los archivos generados
clean:
	rm -f out/*.o main
