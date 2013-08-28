#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "lexer.c"

token lookahead;
char token_value_str[20] = "";
int token_value_num = 0;

parser_error() {
    printf("Parser error\n");
    exit(1);
}

void parser_match(token expected) {

    if (lookahead != expected) {
        parser_error();
    } else if (lookahead != EOF) {
        // fetch next token
        lookahead = lexer_next(token_value_str, &token_value_num);
    }
}

void parser_emit(char *s, ... ) {
    va_list argptr;
    va_start(argptr, s);

    vprintf(s, argptr);
}

void parser_literal() {
    if (lookahead == NUM) {
        parser_emit("%d", token_value_num);
        parser_match(NUM);
    } else if (lookahead == STRING) {
        parser_emit("\"%s\"", token_value_str);
        parser_match(STRING);
    }
}

void parser_stmt() {
    if (lookahead == RETURN) {
        parser_match(RETURN);
        parser_emit("return ");
        parser_literal();
    } else if (lookahead == TYPE) {
        parser_match(TYPE);
        parser_emit("%s=", token_value_str);
        parser_match(ID);
        parser_match(EQUALS);
        parser_literal();
    } else {
        parser_error();
    }
}

void parser_stmt_list() {
    if (lookahead == RETURN ||
        lookahead == TYPE) {
        parser_stmt();
        parser_emit(";");
        parser_match(SEMICOLON);
        parser_stmt_list();
    } else {
        // epsilon
    }
}

void parser_args_list_item() {
    parser_match(TYPE);
    parser_emit("%s", token_value_str);
    parser_match(ID);
}

void parser_args_list_cont() {
    if (lookahead == COMMA) {
        parser_emit(",");
        parser_match(COMMA);
        parser_args_list_item();
        parser_args_list_cont();
    } else {
        // epsilon
    }
}

void parser_args_list() {
    if (lookahead == TYPE) {
        parser_args_list_item();
        parser_args_list_cont();
    } else {
        // epsilon
    }
}

void parser_function_def() {
    parser_match(TYPE);

    parser_emit("function %s(", token_value_str);
    parser_match(ID);
    parser_match(L_PAREN);
    parser_args_list();
    parser_emit("){");
    parser_match(R_PAREN);
    parser_match(L_BRACE);
    parser_stmt_list();
    parser_emit("}");
    parser_match(R_BRACE);
}

void parser_function_defs() {
    if (lookahead == TYPE) {
        parser_function_def();
        parser_function_defs();
    } else {
        // epsilon
    }
}

void parser_start() {
    lookahead = lexer_next(token_value_str, &token_value_num);

    parser_function_defs();
    parser_emit("main();");
    parser_match(EOF);
}