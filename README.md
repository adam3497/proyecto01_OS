# Proyecto 1 - Algoritmo de Huffman

Este es código fuente para el primer proyecto del curso Principios de Sistemas Operativos, primer semestre 2024, de la carrera Ingeniería en Computación del Instituto Tecnológico de Costa Rica, sede San José.

Este proyecto está hecho en el lenguaje de programación C y consiste en crear un codificador y descodificador usando el Algoritmo de Huffman. Además, crear una versión serial, fork y pthread de ambos programas. 

A continuación se detallan algunos requisitos para poder ejecutar el proyecto.

## Requisitos

Se deben cumplir los siguientes requisitos para poder ejecutar el proyecto:

- El proyecto fue creado pensando en el sistema operativo Fedora Workstation 39.
- Se debe tener instalado:
    - gcc
    - gdb
    - make
    - Cmake

Estos requisitos aseguran que el proyecto pueda correr de una manera efectiva. 

De igual manera, se provee de un script para instalar dichas dependencias de manera automática, además de compilar el proyecto. 

## Instalación

El proyecto provee de un *script sh* que ayuda a instalar las dependencias necesarias para poder ejecutar los programas. Estando en una terminal, en la carpeta raíz del proyecto, escribimos:

> $ ./install.sh

o bien

> $ sh install.sh

Esto nos va a pedir la contraseña para poder instalar las dependencias, una vez colocamos la contraseña de manera satisfactoria, procede a instalar cada una de las dependencias necesarias, así como, crear varias carpetas que van a contener archivos binarios, temporales y objetos de salida generados durante la ejecución. 

## Compilación manual

De igual manera, se provee de un archivo *makefile* para poder compilar el proyecto de manera manual. 

Para compilar el módulo de Huffman:

> $ make huffman

Programas serial:

> $ make serial

Programas fork:

> $ make fork


Programas pthread: 

> $ make pthread

Si se desea compilar todos los módulos:

> $ make

**Nota:** si no se ejecuta el primer comando `make huffman`, al ejecutar los demás comandos individuales, la compilación falla, de igual manera se pude agregar la regla *huffman* antes de las reglas individuales, e.g., `make huffman serial`; y así con los demás. 

## Ejecución

Para poder ejecutar los distintos programas, se provee de 3 archivos .sh. 

#### Ejecutar programa serial

Para ejecutar el programa serial (codificador y descodificador), se debe escribir en una terminal, estando en la carpeta raíz:

> $ ./serial_run.sh

Si se desea ejecutar solamente el codificador o descodificador:

> $ ./serial_run.sh compressor

o 

> $ ./serial_run.sh decompressor


#### Ejecutar programa fork

Para ejecutar el programa fork (codificador y descodificador), se debe escribir en una terminal, estando en la carpeta raíz:

> $ ./fork_run.sh

Si se desea ejecutar solamente el codificador o descodificador:

> $ ./fork_run.sh compressor

o 

> $ ./fork_run.sh decompressor


#### Ejecutar programa pthread

Para ejecutar el programa pthread (codificador y descodificador), se debe escribir en una terminal, estando en la carpeta raíz:

> $ ./pthread_run.sh

Si se desea ejecutar solamente el codificador o descodificador:

> $ ./pthread_run.sh compressor

o 

> $ ./pthread_run.sh decompressor


## Problemas

Si existe un problema a la hora de ejecutar alguno de los archivos .sh, seguramente el problema recae en la falta de permisos. Ejecutando el siguiente comando se puede solucionar el problema:

Archivo: `install.sh`
> $ chmod +x install.sh  
> $ sudo chmod u+s install.sh

Los otros archivos:
> $ chmod +x serial_run.sh fork_run.sh pthread_run.sh
