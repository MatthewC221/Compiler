//what's left to do, starting on parser
//comments that are on lines of code need to be fixed, e.g. a++ //this is a increment
//need to partition with tokens, not just spaces and "
//the a++ isn't partitioning right

#include <stdio.h>
#include <string.h>
#include <regex.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "header.h"
#include <assert.h>

//takes arguments ./scanner parse -1 --- PARSER
//./scanner parse location_of_test --- SCANNER

/*

1 = INTLITERAL (32415/0x24141)
2 = CHARLITERAL ('a')
3 = IDENTIFIER (name)
4 = BOOLEANLITERAL (true/false)
5 = STRINGLITERAL ("hello my friends")
6 = CLASSLITERAL (class)
7 = MAIN (main)
8 = TYPE (int/char, bool)
9 = CONDITIONAL (if/while/for/etc)
10 = FUNCTION IDENTIFIER (sort out later)
11 = OPERANDS (+, *. /, -)
12 = EQUAL (=)
13 = CONDITIONAL OPERANDS (==, !=, <=, >=)
14 = RETURN
15 = SPECIAL
16 = AND/OR
17 = EOS (end of statement - ;/}/{)

*/

int type = 0;

#define PARSE 1
#define SCAN 2

int main(int argc, char *argv[]) {

    int status = 0; //if parsing or scanning

    if (argc != 3) {
        fprintf(stderr, "No compilation file specified\n");
        return EXIT_FAILURE;
    } else if ((strcmp(argv[1], "scan")) == 0) {
        status = SCAN;
    } else if ((strcmp(argv[1], "parse")) == 0) {
        status = PARSE;
    } else {
        fprintf(stderr, "Invalid input\n");
        return EXIT_FAILURE;
    }
    
    FILE *fp;
    
    char *initial_path = "tests/";   
    char *full_path = calloc(strlen(argv[2]) + strlen(initial_path) + 1, 1);
    strncpy(full_path, "tests/", 6);
    strcat(full_path, argv[2]);
    

    if (atoi(argv[2]) == -1) {
        free(full_path);
        pushLine();
    } else {
       
        fp = fopen(full_path, "r");
        if (fp == NULL) {
            fprintf(stderr, "File is invalid\n");
            free(full_path);
            return EXIT_FAILURE;
        }
        
        char buf[255];
        line_number = 1;
        
        printf("Status is %d\n\n", status);
        
        while (fgets(buf, 255, fp)) {
            checkLine(buf, line_number++);
        }
        free(full_path);
        fclose(fp);
    }

    return EXIT_SUCCESS;
}


void checkLine(char *input, int line) {    //main check line function

    //Initially, implement so we can separate words, numbers, brackets, colons, etc.

    int starting_token = -1;
    int i = 0;
    int length = strlen(input);
    bool string_lit = false;        
    
    //Currently separating based on ", spaces and \n
    
    //Basically what happens is
    //this gets passed (on symbols) abcdefg+, therefore we must minus 1
    //on symbols of ==, && (double symbols), it can become like this i==a, therefore we must check before subtracting
    
    for (i = 0; i <= length - 1; i++) {
        if (starting_token == -1) {
            starting_token = i;
        }
        
        if (input[i] == '"' && input[i - 1] != '\\') {
            type = 5;                                                          //for string literals
            string_lit = !string_lit;
        } else if (input[i] == '\\' && (string_lit)) {                                   //within string literals cancel next
            i++;
        } else if (input[i] == '\'' && (string_lit)) {
            printf ("%d:%d Failed to compile due to stringliteral failure:  %sunexpected %c\n", line_number, i, input, input[i]);
            break;
        } else if ((isspace(input[i]) || input[i] == '\n') && string_lit == false) {    //general space and newline partitioning
            if (printRange(starting_token, i, input) == 1) {
                break;
            }
            starting_token = -1;
        } else { 
            int temp = specialChar(input[i], input[i + 1]);
            if ((temp == 1) && string_lit == false) {
                if (starting_token != i) {
                    i--;
                }
                printRange(starting_token, i, input);
                starting_token = -1;
            } else if (temp == 2 && string_lit == false) {
                if (i >= starting_token + 1) {
                    i = i - 2;
                }
                printRange(starting_token, ++i, input);
                starting_token = -1;
            }
        }
    }
}

void checkLine_parse(char *input, int line) {    //parse checkline

    //Initially, implement so we can separate words, numbers, brackets, colons, etc.

    int starting_token = -1;
    int i;
    int length = strlen(input);
    bool string_lit = false;        
    
    for (i = 0; i <= length - 1; i++) {
        if (starting_token == -1) {
            starting_token = i;
        }
        
        if (input[i] == '"' && input[i - 1] != '\\') {
            type = 5;                                                          //for string literals
            string_lit = !string_lit;
        } else if (input[i] == '\\' && (string_lit)) {                                   //within string literals cancel next
            i++;
        } else if (input[i] == '\'' && (string_lit)) {
            printf ("%d:%d Failed to compile due to stringliteral failure:  %sunexpected %c\n", line_number, i, input, input[i]);
            break;
        } else if ((isspace(input[i]) || input[i] == '\n') && string_lit == false) {    //general space and newline partitioning
            pushRange(starting_token, i, input); 
            starting_token = -1;
        } else { 
            int temp = specialChar(input[i], input[i + 1]);
            if ((temp == 1) && string_lit == false) {
                if (starting_token != i) {
                    i--;
                }
                pushRange(starting_token, i, input);
                starting_token = -1;
            } else if (temp == 2 && string_lit == false) {
                if (i >= starting_token + 1) {
                    i = i - 2;
                }
                pushRange(starting_token, ++i, input);
                starting_token = -1;
            }
        }
    }
}


int specialChar(char current, char infront) {

    int return_value = 0;

    if (current == ',') {
        return_value = 1;
    } else if (current == '!' && infront == '=') {
        return_value = 2;
        type = 13;
    } else if (current == '*') {
        return_value = 1;
        type = 11;
    } else if (current == '+') {
        return_value = 1;
        type = 18;
    } else if (current == '{' || current == '}' || current == ';') {
        return_value = 1;
        type = 17;
    } else if (current == '(') {
        return_value = 1;
    } else if (current == ')') {
        return_value = 1;
    } else if (current == ']') {
        return_value = 1;
    } else if (current == '[') {
        return_value = 1;
    } else if (current == '/' && infront != '/') {
        return_value = 11;
        //printf("current = %c, infront = %c\n", current, infront);
    } else if (current == '-' && !isdigit(infront)) {
        return_value = 1;
        type = 18;
    } else if (current == '>' || current == '<') {
        type = 13;
        if (infront == '=') {
            return_value = 2;
        } else {
            return_value = 1;
        }
    } else if (current == '=') {
        if (infront == '=') {
            type = 13;      //operand
            return_value = 2;
        } else {
            type = 12;      //not operand
            return_value = 1;
        }
    } else if (current == '|' && infront == '|') {
        return_value = 2;
        type = 16;
    }
    


    return return_value;

}


int printRange(int start, int end, char *string) {      

    //Checks what has been passed and throws errors


    if (start == end && (string[start] == '\n' || string[start] == ' ')) {
        return EXIT_SUCCESS;
    }

    char *new_string = makeString(start, end, string);

    if (strcmp(new_string, "//") == 0) {           
        return 1;
    } else if (checkCharLiteral(new_string)) {
		printf("Failed to compile due to charliteral failure\n");
	} else if (checkHex(new_string)) {
		printf("Failed to compile due to hexadecimal reference failure\n");
	} else if (checkIntegers(new_string)) {
        printf("Failed to compile due to invalid integer assignment\n");
    } else if (checkIdentifier(new_string)) {
        printf("Failed to compile due to invalid identifier assignment\n");
    } else {
    
        checkTokens(new_string);    //doesn't have a return
		printf("%d ", line_number);
            switch (type) {
				case 1:
					printf("INTLITERAL ");
					break;
				case 2:
					printf("CHARLITERAL ");
					break;
				case 3:
					printf("IDENTIFIER ");
					break;
				case 4:
				    printf("BOOLEANLITERAL ");
			        break;
		        case 5:
		            printf("STRINGLITERAL ");       //string literal done seperately, it's okay
		            break;
			}
        printf("%s\n", new_string);
    }

	type = 0;
	free(new_string);

    return EXIT_SUCCESS;
}


char *makeString(int start, int end, char *string) {

    int i; //for new_string
    int j; //for incrementing the given string
	
	if (string[end] == ' ' || string[end] == '\n') end = end - 1;

    char *new_string = calloc(end - start + 2, 1);
    
    assert(new_string != NULL);

    for (i = 0, j = start; j <= end; j++, i++) {
        new_string[i] = string[j];
    }

    return new_string;
}

int checkCharLiteral(char *test) {

    //Checking for charliterals, throw error for more than one within ' '

    int length = strlen(test);
    int i;
    int count_single = 0;
	int error = 0;
	int special_count = 0; 
	int real_counter = 0;

	int possible_return = 1; //if this isn't set to 0, leave function

	if (test[length - 1] == 39 && test[0] == 39) {
		special_count = 1;
		possible_return = 0;
		real_counter = -2; 
	}  

	if (possible_return == 1) { //not a char literal, exit prematurely
		return 0;
	}

    for (i = 0; i < length; i++, real_counter++) {
        if (test[i] == 39) {
            count_single++;
        } else if (test[i] == 92 && (test[i + 1] != 39 && test[i + 1] != 34 && test[i + 1] != 92 && test[i + 1] != 74 && test[i + 1] != 110 && test[i + 1] != 116)) {
            fprintf(stderr, "%d:%d Unexpected char:     \"%c\"\n", line_number, i, test[i + 1]);
			error = 1;
        } else if (test[i] == 92 && (test[i + 1] == 34 || test[i + 1] == 92 || test[i + 1] == 74 || test[i + 1] == 110 || test[i + 1] == 116 || test[i + 1] == 39)) {
            i++; //if cancel with \, ignore next character
        } else if (test[i] == 34) {
			fprintf(stderr, "%d:%d Unexpected char:     \"%c\"\n", line_number, i, test[i + 1]);
			error = 1;
		}
    }

	int return_value = 0;

    if (count_single % 2 != 0 || error == 1) {
        fprintf(stderr, "%d:%d Unexpected char:     \"%c\"\n", line_number, i, test[length - 1]);
		return_value = 1;
    } else if (special_count == 1 && real_counter != 1) {
		fprintf(stderr, "%d:%d Char literal can only have one element:      \"%c\"\n", line_number, i, test[real_counter - 1]);
		return_value = 1;
	}

	type = 2;

    return return_value;
}

int checkHex(char *input) {

	int length = 2;
	if (input[0] == '0' && input[1] == 'x') {
	    type = 1;
		if (strlen(input) - length <= 0) {
			fprintf(stderr, "%d Unexpected hexadecimal:        \"%s\"\n", line_number, input);
			type = 0;
			return 1;
		}
	}

	return 0;
}

int checkNewLine(char *input) {

	return (input[strlen(input) - 1] == '\n');

}

int checkIdentifier(char *input) { 		//check for identifiers - numbers, letters and _ only

    int return_value = 0;
    int newLine = 0;

    int length = strlen(input) - 1;

    if (isalpha(input[0]) || input[0] == '_') {
        type = 3;   
        int i;
        for (i = 1; i < length; i++) {
            if (!isalpha(input[i]) && input[i] != '_' && !isdigit(input[i])) {
                if (newLine) {
                    printf("Unexpected char %c\nInvalid identifier:     %s", input[i], input);
                } else {
                    printf("Unexpected char %c\nInvalid identifier:     %s\n", input[i], input);
                }
                type = 0;
                return_value = 1;
                break;
            }
        }
    }        

    return return_value;

}

void checkTokens(char *input) {		//check for decaf identifiers

  //  char *input;

 //   input = calloc(sizeof(char) * strlen(input) + 1, 1);
  //  strncpy(input, input, strlen(input));
    
    if (strcmp(input, "if") == 0) {
        type = 9;
    } else if (strcmp(input, "boolean") == 0) {
        type = 8;
    } else if (strcmp(input, "callout") == 0) {
        type = 15;
    } else if (strcmp(input, "class") == 0) {
        type = 6;
    } else if (strcmp(input, "false") == 0) {
        type = 4;   //booleanliteral
    } else if (strcmp(input, "main") == 0) {
        type = 7;
    } else if (strcmp(input, "true") == 0) {
        type = 4;
    } else if (strcmp(input, "int") == 0) {
        type = 8;
    } else if (strcmp(input, "return") == 0) {
        type = 14;
    } else if (strcmp(input, "void") == 0) {
        type = 8;
    } else if (strcmp(input, "for") == 0) {
        type = 17;
    } else if (strcmp(input, "break") == 0) {
        type = 15;
    } else if (strcmp(input, "continue") == 0) {
        type = 15;
    } else if (strcmp(input, "else") == 0) {
        type = 17;
    } else if (strcmp(input, "/") == 0) {
        type = 11;
    }
  
    return;
}


int checkIntegers(char *input) {

    int return_value = 0;

    int length = strlen(input) - 1;

    if ((isdigit(input[0]) && input[1] != 'x') || (input[0] == '-' && length != 0)) {
        type = 1;   
        int i;
        for (i = 1; i < length; i++) {
            if (!isdigit(input[i])) {
                type = 0;
                return_value = 1;
                break;
            }
        }
    }

    if (return_value == 1) {
        printf("Invalid integer:    %s\n", input);
    }
    return return_value;

}

void checkString(char *temp_string) {

    int length = strlen(temp_string) - 1;
    if (temp_string[0] == '"' && temp_string[length] == '"') {
        type = 5;
    }

}

void pushRange(int start, int end, char *string) {  //parser func

    if (start == end && (string[start] == '\n' || string[start] == ' ' || string[start] == '\t')) {
        return;
    }
    char *new_string = makeString(start, end, string);
    
    //printf("new_string is %s\n", new_string);
    
    if (strcmp(new_string, "//") == 0) {
        return;
    } else if (checkCharLiteral(new_string)) {
		printf("Failed to compile due to charliteral failure\n");
	} else if (checkHex(new_string)) {
		printf("Failed to compile due to hexadecimal reference failure\n");
	} else if (checkIntegers(new_string)) {
        printf("Failed to compile due to invalid integer assignment\n");
    } else if (checkIdentifier(new_string)) {
        printf("Failed to compile due to invalid identifier assignment\n");
    } else {
        checkTokens(new_string);
        checkString(new_string);
        switch (type) {
            case 0:
                push(new_string, "");
                break;
		    case 1:
			    push("INTLITERAL", new_string);
			    break;
		    case 2:
			    push("CHARLITERAL", "");
			    break;
		    case 3:
			    push("IDENTIFIER", new_string);
			    break;
		    case 4:
		        push("BOOLEANLITERAL", "T_F");
	            break;
            case 5:
                push("STRINGLITERAL", new_string);       //string literal done seperately, it's okay
                break;
		    case 6:
		        push("CLASSLITERAL", new_string);
	            break;
		    case 7:
		        push("MAIN", "");
	            break;
		    case 8:
		        push("TYPE", new_string);
	            break;
		    case 9:
		        push("CONDITIONAL", "");
	            break;
	        case 10:
	            push("FUNCTION IDENTIFIER", "");
                break;
	        case 11:
	            push("OPERAND", new_string);
                break;
	        case 12:
	            push("EQUAL", "");
                break;
	        case 13:
	            push("CONDITIONAL OPERAND", new_string);
                break;
	        case 14:
	            push("RETURN", "");
                break;
	        case 15:
	            push("SPECIAL", new_string);
                break;
	        case 16:
	            push("AND/OR", "");
                break;
            case 17:
                push(new_string, "");   //else, for
                break;
            case 18:
                push("PLUS/MINUS", new_string);
                break;
	    }
    }
    
    free(new_string);
    
    type = 0;    
    
}

