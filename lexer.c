#include <stdlib.h>
#include <string.h>
#import "symtable.c"

#define BUFFER_SIZE 25
#define EOF_MARKER '`'

char buffer[BUFFER_SIZE] = "";
int lexeme_beginning = 0;
int forward = 0;
int end_reached = 0;

lexer_error() {
    printf("Lex error\n");
    exit(1);
}

token lexer_next(char *out_token_value_str, int *out_token_value_num) {
    int state = 0;  

    // remove last token from buffer.
    int n = BUFFER_SIZE - forward;
    strncpy(buffer, buffer+forward, n);
    buffer[n] = '\0';     
    lexeme_beginning = 0;
    forward = 0;

    int buffer_end = strlen(buffer);

    if (!end_reached) {
        // fill buffer
        while (buffer_end < BUFFER_SIZE - 1) {
            char c = getchar();
            if (!feof(stdin)) {
                buffer[buffer_end++] = c;
            } else {
                end_reached = 1;
                // EOF marker.
                buffer[buffer_end++] = EOF_MARKER;
                break;
            }
        }      
        buffer[buffer_end] = '\0';    
    }

    while (1) {
        char c = buffer[forward++];
        switch(state) {
            case 0: if (c == ' ' || c == '\t' || c == '\n') {
                state = 0;
                lexeme_beginning++;
            } else {
                switch(c) {
                    case ';': return SEMICOLON;
                    case ')': return R_PAREN;
                    case '(': return L_PAREN; 
                    case '}': return R_BRACE;
                    case '{': return L_BRACE; 
                    case '[': return L_BRACKET;
                    case ']': return R_BRACKET;  
                    case ',': return COMMA;
                    case '+': return PLUS;
                    case '-': return MINUS;
                    case '=': return EQUALS;
                    case '<': state = 4; break;
                    case '>': state = 5; break;
                    case EOF_MARKER: return EOF;
                    default: if (isalpha(c)) { state = 1; }
                             else if (isdigit(c)) { state = 2; }
                             else if (c == '"') { state = 3; }
                             else { lexer_error(); }
                             break;
                }
            } break;
            case 1: if (isdigit(c) || isalpha(c)) {
                state = 1;
            } else {
                // retract forward pointer.
                forward--;
                int n = forward - lexeme_beginning;

                strncpy(out_token_value_str, buffer+lexeme_beginning, n);
                out_token_value_str[n] = '\0';

                symtable_record_ptr rec = symtable_get(out_token_value_str);
                if (rec) {
                    return rec->token;
                } else {
                    return ID;
                }
            } break;
            case 2: if (isdigit(c)) {
                state = 2;
            } else {
                // retract forward pointer.
                forward--;

                *out_token_value_num = atoi(buffer+lexeme_beginning);
                return NUM;
            } break;
            case 3: if (c != '"') {
                state = 3;
            } else {
                // subtract 2 so that we don't get the quotes
                int n = forward - lexeme_beginning - 2;

                strncpy(out_token_value_str, buffer+lexeme_beginning+1, n);
                out_token_value_str[n] = '\0';
                return STRING;
            } break;
            case 4: if (c == '=') {
                return LTE;
            } else {
                // retract forward pointer.
                forward--;
                return LT;
            } break;
            case 5: if (c == '=') {
                return GTE;
            } else {
                // retract forward pointer.
                forward--;
                return GT;
            } break;
            default: lexer_error();
        }
    }
}