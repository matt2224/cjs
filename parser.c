#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "lexer.c"

token lookahead;
char token_value_str[20] = "";
int token_value_num = 0;

void parser_expr();

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

void parser_id() {
    parser_emit("%s", token_value_str);
    parser_match(ID);
}

void parser_arg_list_item() {
    parser_expr();
}

void parser_arg_list_cont() {
    if (lookahead == COMMA) {
        parser_emit(",");   
        parser_match(COMMA);
        parser_arg_list_item();
        parser_arg_list_cont();
    } else {
        // epsilon
    }
}

void parser_arg_list() {
    if (lookahead == NUM || 
        lookahead == STRING ||
        lookahead == MINUS ||
        lookahead == PLUS ||
        lookahead == L_BRACE) {
        parser_arg_list_item();
        parser_arg_list_cont();
    } else {
        // epsilon
    }
}

void parser_id_expr_cont() {
    if (lookahead == L_PAREN) {
        parser_emit("(");
        parser_match(L_PAREN);
        parser_arg_list();
        parser_emit(")");
        parser_match(R_PAREN);
    } else if (lookahead == PLUS) {
        parser_emit("++");
        parser_match(PLUS);
        parser_match(PLUS);
        parser_id();
    } else if (lookahead == MINUS) {
        parser_emit("--");
        parser_match(MINUS);
        parser_match(MINUS);
        parser_id();
    } else if (lookahead == L_BRACKET) {
        parser_match(L_BRACKET);
        parser_emit("[");
        parser_expr();
        parser_emit("]");
        parser_match(R_BRACKET);
    } else {
        // epsilon
    }
}

void parser_num() {
    parser_emit("%d", token_value_num);
    parser_match(NUM);
}

void parser_array_init_list_item() {
    parser_expr();
}

void parser_array_init_list_cont() {
    if (lookahead == COMMA) {
        parser_match(COMMA);
        parser_emit(",");
        parser_array_init_list_item();
        parser_array_init_list_cont();
    } else {
        // epsilon
    }
}

void parser_array_init_list() {
    parser_array_init_list_item();
    parser_array_init_list_cont();
}

void parser_expr() {
    if (lookahead == NUM) {
        parser_num();
    } else if (lookahead == STRING) {
        parser_emit("\"%s\"", token_value_str);
        parser_match(STRING);
    } else if (lookahead == PLUS) {
        parser_emit("++");
        parser_match(PLUS);
        parser_match(PLUS);
        parser_id();
    } else if (lookahead == MINUS) {
        parser_emit("--");
        parser_match(MINUS);
        parser_match(MINUS);
        parser_id();
    }  else if (lookahead == L_BRACE) {
        parser_emit("[");
        parser_match(L_BRACE);
        parser_array_init_list();
        parser_emit("]");
        parser_match(R_BRACE);
    } else if (lookahead == ID) {
        parser_id();
        parser_id_expr_cont();
    }
}

void parser_var_dec_cont() {
    if (lookahead == EQUALS) {
        parser_match(EQUALS);
        parser_emit("=");
        parser_expr();
    } else if (lookahead == L_BRACKET) {
        parser_match(L_BRACKET);
        parser_emit("=new Array(");
        parser_num();
        parser_emit(")");
        parser_match(R_BRACKET);
    } else {
        // epsilon
    }
}

void parser_stmt() {
    if (lookahead == RETURN) {
        parser_match(RETURN);
        parser_emit("return ");
        parser_expr();
    } else if (lookahead == TYPE) {
        parser_match(TYPE);
        parser_emit("var ");
        parser_id();
        parser_var_dec_cont();
    } else {
        parser_expr();
    }
}

void parser_stmt_list() {
    if (lookahead == RETURN ||
        lookahead == TYPE ||
        lookahead == ID ||
        lookahead == STRING ||
        lookahead == NUM ||
        lookahead == PLUS ||
        lookahead == MINUS ||
        lookahead == L_BRACE) {
        parser_stmt();
        parser_emit(";");
        parser_match(SEMICOLON);
        parser_stmt_list();
    } else {
        // epsilon
    }
}

void parser_param_list_item() {
    parser_match(TYPE);
    parser_id();
}

void parser_param_list_cont() {
    if (lookahead == COMMA) {
        parser_emit(",");
        parser_match(COMMA);
        parser_param_list_item();
        parser_param_list_cont();
    } else {
        // epsilon
    }
}

void parser_param_list() {
    if (lookahead == TYPE) {
        parser_param_list_item();
        parser_param_list_cont();
    } else {
        // epsilon
    }
}

void parser_function_def() {
    parser_match(TYPE);

    parser_emit("function ");
    parser_id();
    parser_emit("(");
    parser_match(L_PAREN);
    parser_param_list();
    parser_emit("){");
    parser_match(R_PAREN);
    parser_match(L_BRACE);
    parser_stmt_list();
    parser_emit("}");
    parser_match(R_BRACE);
}

void parser_enum_list_item() {
    static enum_val_count = 0;
    parser_id();
    parser_emit("=%d;", enum_val_count++);
}

void parser_enum_list_cont() {
    if (lookahead == COMMA) {
        parser_match(COMMA);
        parser_enum_list_item();
        parser_enum_list_cont();
    } else {
        // epsilon
    }
}

void parser_enum_def() {
    parser_match(ENUM);
    parser_match(L_BRACE);
    parser_enum_list_item();
    parser_enum_list_cont();
    parser_match(R_BRACE); 
}

void parser_root_defs() {
    if (lookahead == TYPE) {
        parser_function_def();
    } else if (lookahead == ENUM) {
        parser_enum_def();
    } else {
        // epsilon
        return;
    }

    parser_root_defs();
}

void parser_start() {
    lookahead = lexer_next(token_value_str, &token_value_num);

    parser_root_defs();
    parser_emit("main();");
    parser_match(EOF);
}