#include "global.h"
#include "parser.c"
#include "symtable.c"

int main() {
    symtable_insert("int", TYPE, 0);   
    symtable_insert("void", TYPE, 0);   
    symtable_insert("char", TYPE, 0);    
    symtable_insert("for", FOR, 0);    
    symtable_insert("while", WHILE, 0);    
    symtable_insert("do", DO, 0);    
    symtable_insert("return", RETURN, 0);
    symtable_insert("enum", ENUM, 0);

    parser_start();

    return 0;
}