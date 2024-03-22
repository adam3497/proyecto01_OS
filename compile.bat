@echo off

rem Compiler options
set CC=gcc
set CFLAGS=-Wall -Wextra -Werror -std=c11

rem Source files
set SRCS=utilities\file_utils.c huffman\freq.c main.c

rem Output directory
set OBJDIR=out

rem Compile each source file into an object file
%CC% %CFLAGS% -c utilities\file_utils.c -o %OBJDIR%\file_utils.o
%CC% %CFLAGS% -c huffman\freq.c -o %OBJDIR%\freq.o
%CC% %CFLAGS% -c main.c -o %OBJDIR%\main.o

rem Link object files to generate the executable
%CC% %CFLAGS% %OBJDIR%\file_utils.o %OBJDIR%\freq.o %OBJDIR%\main.o -o main

echo Compilation completed.
