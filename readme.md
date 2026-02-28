# Murasaki-CC

A small C-written compiler front-end and interpreter for a minimal expression language.

# Features

File loading

UTF-8 BOM handling

Lexer (numbers, print, +, -, ;)

Recursive descent parser

AST construction

Tree-walk interpreter

Dynamic memory management

# Example

Input:

print 5 + 3 - 2;
print 10 + 1;

Output:

6
11
# Architecture
source file
   ↓
lexer
   ↓
parser
   ↓
AST
   ↓
interpreter
# Build

Using clang:

clang main.c lexer.c parser.c interpreter.c -o murasaki-cc
Run
./murasaki-cc test.mr
