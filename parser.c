/*

Where to put semantic analyser?

We should keep the most recent data in a temporary buffer, as it does go statement by statement...
e.g. 
When pushing in array reference, maintain the array reference name and number
When pushing in func parameters, maintain each parameter and number of parameters
When pushing in array initialisation, maintain name and size
When pushing in return of types, maintain type of return and the function name (that currently in)
When pushing in function calls, maintain all arguments and number of arguments

ANOTHER STACK just for this stuff, when successful push in the stack components. (It should all be in order)
If pushing in number Func call w/ARG: push in stack size

Flush all at the end of one run




**BROKEN**

a = 4 + func(1, 2, 3); ending with func call with param


***DOING
type of function (some work)
translate codes
***

must remember what can be reduced such as: (int a, int b) to (Parameters)
CHECK parser_grammar.txt

HAVE SPECIAL EXCEPTIONS, if within conditional AND/OR can be used, then reduced

there are special things to consider
int a = (5 + 5 == 10); BOOLEAN

C N {       //class initialiser
T M () {    //main
T N;        //identifier initialisation
T N ARR;
N EQ ARITHMETIC     //only one equal possible: 5 + 5, Operands (+, *, /, -, <, >, !=, ==)
N EQ N O N --> N EQ ARI
N EQ ARI O N --> N EQ ARI
Co ( S ) {  //conditional statement, can be Co (S (AND/OR) S (AND/OR) S) --> Co (S)
FI (T N, T N) --> FI (P) //function ident. + param; or FI ();

Reduce until ; or { or }
Then pop everything off and check

*/

#define LEFT_BRACKET -1
#define INT_LITERAL 1
#define IDENTIFIER 3

unsigned int bracket_count = 0;             // {} count
unsigned int in_loop = 0;                   //flag if we're in loop
unsigned int line = 1;                      //line number
unsigned int column = 0;
unsigned int order_count = 0;               //what item up to

unsigned int line_array = 0;                 //put line number into array

//For managing if/else conditionals

char *previous_string;                      //basically a look ahead

unsigned int look_ahead = 0;
unsigned int conditional_flag = 0;
unsigned int if_statement = 0;
unsigned int else_statement = 0;

unsigned int function_bracket = 0;
unsigned int in_function = 0;

char *current_line;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"   

    
void pushLine() {
    
    
    char *full_path = "parser_tests/legal-M";
    
    FILE *fp;
    printf("full path is %s\n", full_path);
    fp = fopen(full_path, "r");
    if (fp == NULL) {
        fprintf(stderr, "File is invalid\n");
        fclose(fp);
        return;
    }
    
    char buf[255];

    printf("Opening file %s\n", full_path);
    
    int number = 1;
    while (fgets(buf, 255, fp)) {
        current_line = malloc(sizeof(char) * strlen(buf) + 1);
        strcpy(current_line, buf);
        checkLine_parse(buf, number++);
        line++;
    }

    //}
    //display();
    //free(semantic_info);
    if (bracket_count != 0) {
        printf("Missing closed finishing brace }\n");
    }
    
    if (if_statement < else_statement) {
        printf("Invalid if/else statements\n");
    }
    
    //display();
    semantic_handler();
    
    fclose(fp);
    

}

char *pop() {
    
    if (s.top == 0) {
        return "";
    } else {
        --s.top;
    }
    
    return s.stk[s.top];
}

char *pop_semantic() {
    
    if (semantic_info.top == 0) {
        return "";
    } else {
        --semantic_info.top;
    }
    
    return semantic_info.stk[semantic_info.top];
}

void push(char* string1, char *string2, int end) {              

//PRIMARY FUNCTION: checks if we can reduce (string1 is new string), else push in
//string2 is name of variables/classes/etc.

    column = end;
    if (s.top == (MAXSIZE - 1)) {
        printf("Stack is full\n");
    } else {
       
        if (look_ahead == 1) {
            if (strcmp(string1, "else") == 0) {
                pure_push("}");
            } else {
                if (conditional_flag > 0) {
                    conditional_flag--;
                }
            }
        }
        
        look_ahead = 0;
        
        //ABOVE ALL FOR IF/ELSE
        
        order[order_count] = 0;
        if (strcmp(string1, "IDENTIFIER") == 0) {   //identifier: abc, name, hello_world
            identifier_reduce(string1); 
            order[order_count] = IDENTIFIER;
            pure_push_names(string2);
        } else if (strcmp(string1, "MAIN") == 0) {  //main: MAIN
            check_main();
        } else if (strcmp(string1, "{") == 0) {     //to indicate end of function/cond/loop
            bracket_count++;
            if (!check_loop()) check_function();
        } else if (strcmp(string1, ";") == 0) {     //to indicate end of statement
            check_statement();                      
        } else if (strcmp(string1, "}") == 0) {     //invalid stack if items remaining
            if (s.top > 0) {
                printf("Premature end of statement, stack has %d elements: %s\n", s.top, string1);
            }
            bracket_count--;
            if (in_function == 1 && bracket_count <= function_bracket - 1) {
                in_function = 0;
                push_complete("X");
                semantic_line[line_array++] = line;
            }
            
            if (in_loop != 0 && bracket_count < in_loop) {
                push_complete("Q: EOL");
                in_loop = 0;
            }
            
            bracket_stack.top = 0;
            s.top = 0;
            order_count = 0;
            look_ahead = 1;
            
        } else if (strcmp(string1, "INTLITERAL") == 0) {    //possible int reduction, INTLIT + INTLIT ---> INTLIT
            check_integer();
            order[order_count] = INT_LITERAL;
            pure_push_names(string2);
        } else if (strcmp(string1, "(") == 0) {            
            pure_push_bracket(string1);
            pure_push_names("(");
            order[order_count] = LEFT_BRACKET;              //solution to parameters passed before brackets... a 5();
        } else if (strcmp(string1, ")") == 0) {
            check_bracket_stack();
            pure_push_names(")"); 
        } else if (strcmp(string1, "]") == 0) {
            pure_push_names("]");
            array_check();             
        } else if (strcmp(string1, "[") == 0) {
            pure_push_names("[");
            pure_push(string1); 
        } else if (if_operand(string1)) {
            pure_push_names(string2);         
            pure_push(string1);
        } else if (strcmp(string1, "SPECIAL") == 0 && strcmp(string2, "callout") == 0) {
            //pure_push_names(string2);
            pure_push(string2);
        } else if (strcmp(string1, "SPECIAL") == 0) {
            pure_push(string2);
        } else if (strcmp(string1, "BOOLEANLITERAL") == 0 && strcmp(string2, "T_F") == 0) {
            pure_push_names(string2);
            pure_push(string1); 
        } else if (strcmp(string1, "STRINGLITERAL") == 0 && strcmp(string2, "\"printf\"") == 0) {
            pure_push_names("_printf_");
            pure_push("C_LIB");
        } else if (strcmp(string1, "STRINGLITERAL") == 0) {
            check_string(string2);
        } else if (strcmp(string1, "CONDITIONAL OPERAND") == 0) {
            pure_push(string1);
            pure_push_names(string2);
        } else if (strcmp(string1, "TYPE") == 0) {
            pure_push(string1);
            if (strcmp(string2, "void") == 0) {
                pure_push_names("V");
            } else if (strcmp(string2, "int") == 0) {
                pure_push_names("N");
            } else {            //bool
                pure_push_names("T");
            }
        } else {
            pure_push(string1);
        }
        order_count++;
        if (strcmp(string1, "(") != 0 && strcmp(string1, ")") != 0) {
            previous_string = malloc(sizeof(char) * strlen(string1) + 1);
            strcpy(previous_string, string1);
        }
        
               
    }
}

void check_string(char *string1) {

    char *temp = pop();
    char *temp2 = pop();
    if (strcmp(temp, ",") == 0 && strcmp(temp2, "C_LIB") == 0) {
        pure_push("C_LIB_STR");             //call out C_LIB_STR
        pure_push_names(string1);
        pure_push_names("_");              //beginning of parameters
    } else {
        pure_push(temp2);
        pure_push(temp);
        pure_push("STRINGLITERAL");
        semantic_line[line_array++] = line;
    }

}



void parameter_reduce() {

//Check if function gets passed var e.g. func(5, a, 2);
//Illegal to create function with int main(2, 3); and illegal to call with char/int identifier: func (int a, int b);

    char *temp = pop();       
    char *temp1 = pop();       
    char *temp2 = pop();       

    if (strcmp(temp1, ",") == 0 && (strcmp(temp2, "CHAR/INT IDENTIFIER") == 0 || strcmp(temp2, "PARAMETERS") == 0)) {       //(int a, int b) || (int a, int b, int c), etc.
        pure_push("PARAMETERS");
    } else {
        pure_push(temp2);
        pure_push(temp1);
        pure_push(temp);
    }


}

void logic_check() {





}


void function_call_check() {

    //This should occur after creating pushing FUNC CALL / FUNC CALL w/ARG
    char *temp = pop();
    char *temp1 = pop();
    char *temp2 = pop();

    if (strcmp(temp1, "EQUAL") == 0 && strcmp(temp2, "IDENTIFIER") == 0) {      //a = func();
        pure_push(temp2);
        pure_push(temp1);
        pure_push("INTLITERAL");
        
    } else if (if_operand(temp1) && if_integer(temp2)) {            //b = a + 2 + ... + 3 * func();
        pure_push("INTLITERAL");
        
    } else if (strcmp(temp1, ",") == 0 && if_integer(temp2)) {                  //func(b, func(1,2)); DOESN'T CURRENTLY WORK...
        pure_push("ARGUMENTS_DOUBLE");
        
    } else if (strcmp(temp1, ",") == 0 && (strcmp(temp2, "ARGUMENTS") == 0 || strcmp(temp2, "ARGUMENTS_DOUBLE") == 0)) {
        pure_push("ARGUMENTS");     //SAME AS ABOVE, doesn't work
        
    } else {
        pure_push(temp2);
        pure_push(temp1);
        pure_push(temp);
    }

}

void check_main() {
    
    char *temp1 = pop();
    if (strcmp(temp1, "TYPE") == 0) {
        pure_push("MAIN FUNC");
    }     
    
}

void pure_push(char *string1) {

    if (strcmp(string1, "") != 0) {
        memcpy(s.stk[s.top++], string1, strlen(string1) + 1);
       // strncpy(s.stk[s.top], string1, strlen(string1) + 1);
       // s.top++;
    }
}

void pure_push_names(char *string1) {

    if (strcmp(string1, "") != 0) {
        memcpy(semantic_info.stk[semantic_info.top++], string1, strlen(string1) + 1);
    }
}

void push_complete(char *string1) {

    if (strcmp(string1, "") != 0) {
        strcpy(complete.stk[complete.top++], string1);
    }
}

void array_check() {

    char *temp1 = pop();    //intliteral
    char *temp2 = pop();    //[
    char *temp3 = pop();

    if ((strcmp(temp1, "INTLITERAL") == 0 || strcmp(temp1, "IDENTIFIER") == 0) && strcmp(temp2, "[") == 0) {    
        if (strcmp(temp3, "CHAR/INT IDENTIFIER") == 0) {        //int a[5], int a[b], int a[func()], etc.
            pure_push("ARR DECL");
            //pure_push_names("]");
        } else if (strcmp(temp3, "IDENTIFIER") == 0 || strcmp(temp3, "INTLITERAL") == 0) {      //array references get reduced too soon, will check 7[5] errors in semantic
            pure_push("ARRAY REFERENCE");                            //a[5], a[i]
            //pure_push_names("]");
        } else {
            pure_push(temp3);
        }
    } else {
        pure_push(temp3);
        pure_push(temp2);
        pure_push(temp1);
    }



}

char *join_string (char *string1, char *string2) {

    //printf("string1 = %s, string2 = %s\n", string1, string2);
    char *string3 = calloc(strlen(string1) + strlen(string2) + 3, 1);
    strcpy(string3, string1);
    strcat(string3, ": ");
    strcat(string3, string2);


    return string3;
}


void pure_push_bracket(char *string1) {

    if (strcmp(string1, "") != 0) {
        strcpy(bracket_stack.stk[bracket_stack.top++], string1);
    }


}

void check_bracket_stack() {

    //printf("PREV = %s\n", previous_string);
    if (bracket_stack.top == 0) {
        fprintf(stderr, "Unmatched bracket \"(\"\n");
    } else {
        bracket_stack.top--;
        char *temp = pop();
        char *temp1 = pop();
              
        if (order[order_count - 1] == LEFT_BRACKET && order[order_count - 2] == INT_LITERAL) {      //8()
            printf("Invalid integer call: %d:%d\n", line, column);
            pure_push(temp1);
            pure_push(temp);           

            
        } else if (if_integer(temp) && strcmp(temp1, "IDENTIFIER") == 0 && order[order_count - 1] != LEFT_BRACKET) {        //func(1)
            pure_push("FUNC CALL w/ARG");                     
            function_call_check();
                             
                       
        } else if ((strcmp(temp, "ARGUMENTS") == 0 || strcmp(temp, "ARGUMENTS_DOUBLE") == 0) && strcmp(temp1, "IDENTIFIER") == 0 && order[order_count - 1] != LEFT_BRACKET) {   //func(1, 2)
            pure_push("FUNC CALL w/ARG");
            function_call_check(); 
            
        } else if ((strcmp(temp, "CHAR/INT IDENTIFIER") == 0 || strcmp(temp, "PARAMETERS") == 0) && strcmp(temp1, "CHAR/INT IDENTIFIER") == 0 && order[order_count - 1] != LEFT_BRACKET) {       //int func(int a, int b)
            pure_push("FUNC DECLARATION w/PARAM");
            
        } else if (strcmp(temp, "CHAR/INT IDENTIFIER") == 0) {      //int func()                    
            pure_push(temp1);
            pure_push("FUNC DECLARATION"); 
            
        } else if (strcmp(temp, "IDENTIFIER") == 0 && strcmp(temp1, "CONDITIONAL OPERAND") != 0) {       //func()
            pure_push(temp1);
            pure_push("FUNC CALL");
            function_call_check();

        } else if ((strcmp(temp, "ARGUMENTS") == 0 || strcmp(temp, "ARGUMENTS_DOUBLE") == 0) && strcmp(temp1, "INTLITERAL") == 0 && strcmp(previous_string, "IDENTIFIER") == 0) {
            char *temp2 = pop();
            if (if_operand(temp2) || strcmp(temp2, "EQUAL") == 0) {  
                pure_push(temp2);
                check_integer();          
            } else {
                pure_push(temp2);
            }           //issue here is we're reducing before arguments part so it becomes, ident = int arg_double
               
        } else {
            char *temp2 = pop();
            char *temp3 = pop();
            
            if ((if_integer(temp)) && strcmp(temp1, "CONDITIONAL OPERAND") == 0 && (if_integer(temp2)) && strcmp(temp3, "CONDITIONAL") == 0) {      //if (a == b) 
                pure_push("CONDITIONAL STATEMENT");
            } else {
                pure_push(temp3);
                pure_push(temp2);
                pure_push(temp1);
                pure_push(temp);
            }
        }
    }
    
}

unsigned int if_integer(char *string1) {        //abc, 5, abc(), abc(1, 2), true, false


    return (strcmp(string1, "IDENTIFIER") == 0 || strcmp(string1, "INTLITERAL") == 0 || strcmp(string1, "FUNC CALL") == 0 || strcmp(string1, "FUNC CALL w/ARG") == 0 || strcmp(string1, "BOOLEANLITERAL") == 0 || strcmp(string1, "ARRAY REFERENCE") == 0);
}

unsigned int if_operand(char *string1) {        //+, -, *, /
    return (strcmp(string1, "OPERAND") == 0 || strcmp(string1, "PLUS/MINUS") == 0);
}

void display () {
    int i;
    if (complete.top == -1) {
        printf ("Stack is empty\n");
    } else {
        printf ("The status of the stack is \n\n");
       // for (i = complete.top - 1; i >= 0; i--)   depending what order to print
        for (i = 0; i <= complete.top - 1; i++)
        {
            printf ("%s\n", complete.stk[i]);
        }
    }
    printf ("\n");
}


void display_current () {
    int i;
    if (s.top == -1) {
        printf ("Stack is empty\n");
    } else {
        printf ("The status of the stack is \n\n");
       // for (i = complete.top - 1; i >= 0; i--) depending what order to print
        for (i = 0; i <= s.top - 1; i++)
        {
            printf ("%s\n", s.stk[i]);
        }
    }
    printf ("\n");
}

void display_semantic () {
    int i;
    if (semantic_info.top == -1) {
        printf ("Stack is empty\n");
    } else {
        printf ("\nThe status of the stack is \n\n[ ");
       // for (i = complete.top - 1; i >= 0; i--) depending what order to print
        for (i = 0; i <= semantic_info.top - 1; i++)
        {
            printf ("%s ", semantic_info.stk[i]);
        }
    }
    printf ("]\n");
}

char *semantic_string () {
    int i;
    char *string = "";
    if (semantic_info.top == 0) {
        return "";
    } else {
        unsigned int counter = 0;
        for (i = 0; i < semantic_info.top; i++) {
            counter = counter + strlen(semantic_info.stk[i]);
       //     printf("info %d:%d = %s\n", i, semantic_info.stk[i]);
        }
       // printf("size is %d:%d, space created = %d:%d\n", semantic_info.top, counter + semantic_info.top + 1);
        string = calloc(counter + semantic_info.top, 1);
        strcpy(string, semantic_info.stk[0]);
        for (i = 1; i < semantic_info.top; i++) {
            //snprintf(string, strlen(string), " ");
            //snprintf(string, strlen(string), semantic_info.stk[i]);
            strcat(string, " ");
            strcat(string, semantic_info.stk[i]);
        }
    }

    return string;
}

void identifier_reduce(char *string) {    

    char *former_string = pop();
    if (get_next() == '(' && if_operand(former_string)) {            //not an identifier, is a function
        printf("AVOIDED\n");
        pure_push(former_string);
        pure_push("IDENTIFIER");
        //check_bracket_stack();
        return;
    }

    char *former_string1 = pop();   
    
        
    if (strcmp(former_string, "CLASSLITERAL") == 0) {       //class Program
        pure_push(former_string1);
        pure_push("CLASS IDENTIFIER");
        
        
    } else if (strcmp(former_string, "TYPE") == 0) {        //int a
        pure_push(former_string1);
        pure_push("CHAR/INT IDENTIFIER");
        parameter_reduce();
    
    } else if (strcmp(former_string, ",") == 0 && (strcmp(former_string1, "CHAR/INT IDENTIFIER") == 0 || strcmp(former_string1, "MULTI CHAR/INT IDENTIFIER") == 0)) {
        pure_push("MULTI CHAR/INT IDENTIFIER");
        
    } else if (if_operand(former_string) && if_integer(former_string1)) {       //a + a -> a
        pure_push("INTLITERAL");
        
        
    } else if (strcmp(former_string, "PLUS/MINUS") == 0 && !if_integer(former_string1)) {   //-a 
        pure_push(former_string1);
        check_integer();
        
    } else if (strcmp(former_string, ",") == 0 && (if_integer(former_string1))) {           //a, b
        pure_push("ARGUMENTS_DOUBLE");
        
    } else if (strcmp(former_string, ",") == 0 && (strcmp(former_string1, "C_LIB_STR") == 0 || strcmp(former_string1, "C_LIB_STR+PARAM") == 0)) {       //callout ("printf", "string", i)
        pure_push("C_LIB_STR+PARAM");
        
    } else if (if_operand(former_string) && strcmp(former_string1, "ARGUMENTS_DOUBLE") == 0) {
        pure_push("ARGUMENTS_DOUBLE");              //for i = 0, a * a {
        
    } else if (strcmp(former_string, ",") == 0 && (if_integer(former_string1) || strcmp(former_string1, "ARGUMENTS") == 0 || strcmp(former_string1, "ARGUMENTS_DOUBLE") == 0)) {    //a, b, c, d
        pure_push("ARGUMENTS");
        
        
    } else {
        pure_push(former_string1);
        pure_push(former_string);
        pure_push(string);
    }

}

unsigned int check_loop() {
    
    char *temp1 = pop();
    char *temp2 = pop();
    char *temp3 = pop();
    char *temp4 = pop();
    char *semantic = semantic_string();
    
    unsigned int return_value = 0;
    
    if (strcmp(temp1, "ARGUMENTS_DOUBLE") == 0 && strcmp(temp2, "EQUAL") == 0 && strcmp(temp3, "IDENTIFIER") == 0 && strcmp(temp4, "for") == 0) {
        char *string = join_string("L", semantic);
        push_complete(string);
        free(string);
        return_value = 1;
        semantic_info.top = 0;
        semantic_line[line_array++] = line;
        in_loop = bracket_count;
    } else {
        pure_push(temp4);
        pure_push(temp3);
        pure_push(temp2);
        pure_push(temp1);
    }
    
    if (strcmp(semantic, "") != 0) {
        free(semantic);
    }
   
    
    
    return return_value;

}
    
void check_function() {        //for functions, classes and conditionals

    unsigned int stack_size = s.top;
    char *temp = pop();
    
    char *semantic = semantic_string();
    
    if (strcmp(temp, "CLASS IDENTIFIER") == 0) {        
        if (stack_size == 1) {
            char *string = join_string("1", semantic);
            push_complete(string);
            free(string);
            semantic_line[line_array++] = line;
        } else {
            printf("Invalid class declaration: %d:%d %s\n", line, column, temp);
        }
        
        
    } else if (strcmp(temp, "MAIN FUNC") == 0) {    //MAIN
        if (stack_size == 1) {
            if (in_function == 1) {
                printf("Invalid function declaration within another function: %d:%d\n", line, column);
            } else {
                in_function = 1;
                function_bracket = bracket_count;
                char *string = join_string("2", semantic);
                push_complete(string);
                free(string);
                semantic_line[line_array++] = line;
            }
        } else {
            printf("Invalid main function: %d:%d %s\n", line, column, temp);
        }
    
        
    } else if (strcmp(temp, "FUNC DECLARATION") == 0) { //NORMAL FUNC
        if (stack_size == 1) {
            if (in_function == 1) {
                printf("Invalid function declaration within another function: %d:%d\n", line, column);
            } else {
                in_function = 1;
                function_bracket = bracket_count;
                char *string = join_string("3", semantic);
                push_complete(string);
                free(string);
                semantic_line[line_array++] = line;
            }
            //pure_push_names("End ARG");
            //display_semantic();
        } else {
            printf("Invalid function declaration: %d:%d %s\n", line, column, temp);
        }
        
    } else if (strcmp(temp, "FUNC DECLARATION w/PARAM") == 0) { //FUNC DECL WITH PARAMETERS
        if (stack_size == 1) {
            //push_complete("FUNCTION DECLARED w/PARAM");
            if (in_function == 1) {
                printf("Invalid function declaration within another function: %d:%d\n", line, column);
            } else {
                in_function = 1;
                function_bracket = bracket_count;
                char *string = join_string("4", semantic);
                push_complete(string);
                free(string);
                semantic_line[line_array++] = line;
            }
           // pure_push_names("End ARG");
           // display_semantic();
        } else {
            printf("Invalid parameters to function declaration: %d:%d %s\n", line, column, temp);
        }
        
        
    } else if (strcmp(temp, "CONDITIONAL STATEMENT") == 0) {    //CONDITIONALS
        if (stack_size == 1) {
            char *string = join_string("5", semantic);
            push_complete(string);
            free(string);
            conditional_flag++;
            if_statement++;
            semantic_line[line_array++] = line;
        } else {
            printf("Invalid conditional: %d:%d %s\n", line, column, temp);
        }        
    
    } else {
        char *temp2 = pop();
        if (strcmp(temp, "else") == 0 && strcmp(temp2, "}") == 0) {     //ELSE
            if (stack_size != 2 || conditional_flag < 1) {
                printf("Invalid else statement: %d:%d %s\n", line, column, temp);
            } else {
                char *string = join_string("6", semantic);
                push_complete(string);
                free(string);
                else_statement++;
                semantic_line[line_array++] = line;
            }
            
            
        } else if (if_integer(temp) && strcmp(temp2, "CONDITIONAL") == 0) {     //if (a) {
            if (stack_size == 2) {
                char *string = join_string("7", semantic);
                push_complete(string);
                free(string);
                conditional_flag++;
                if_statement++;
                semantic_line[line_array++] = line;
            } else {
                printf("Invalid conditional statement: %d:%d %s\n", line, column, temp);
            }
            
            
        } else {
            printf("Invalid use of function/class/conditional: %d:%d %s\n", line, column, temp);
        }
    }

    if (bracket_stack.top != 0) {
        fprintf(stderr, "Unmatched bracket \")\"\n");
    }
    bracket_stack.top = 0;
    s.top = 0;
    semantic_info.top = 0;
    if (strcmp(semantic, "") != 0) {
        free(semantic);
    }

}
    
void check_statement() {        

//for complete statements, push into another stack/queue, empty after each successful statement
//this is important because we could have int int a; and we are ignoring the first invalid int
 
    unsigned int stack_size = s.top;   
    char *temp = pop();         //if it's not what we are looking for, push back in
    char *temp1 = pop();
    char *temp2 = pop();
    
    char *semantic = semantic_string();
    //printf("temp2: %s, temp1: %s, temp: %s\n", temp2, temp1, temp);
    
    if (strcmp(temp, "CHAR/INT IDENTIFIER") == 0) {     //for var initialisation
        if (stack_size == 1) {
            char *string = join_string("8", semantic);
            push_complete(string);
            free(string);
            semantic_line[line_array++] = line;
        } else {
            printf("Invalid variable initialisation: %d:%d\n", line, column);
        }
        
        
    } else if (if_integer(temp) && strcmp(temp1, "EQUAL") == 0 && (strcmp(temp2, "IDENTIFIER") == 0)) {       //for var setting
        if (stack_size == 3) {
            //push_complete("VARIABLE VALUE SET");
            char *string = join_string("9", semantic);
            push_complete(string);
            free(string);
            semantic_line[line_array++] = line;
            //display_semantic();
        } else {
            printf("Invalid variable value: %d:%d\n", line, column);
        }
        
    } else if (strcmp(temp, "break") == 0) {
        if (stack_size == 1) {
            push_complete("O: break");
        } else {
            printf("Invalid break statement: %d:%d\n", line, column);
        }
        
    } else if (strcmp(temp, "continue") == 0) {
        if (stack_size == 1) {
            push_complete("P: continue");
        } else {
            printf("Invalid continue statement: %d:%d\n", line, column);
        }
        
    } else if (strcmp(temp, "MULTI CHAR/INT IDENTIFIER") == 0) {
        if (stack_size == 1) {
            char *string = join_string("M", semantic);
            push_complete(string);
            free(string);
            semantic_line[line_array++] = line; 
        } else {
            printf("Invalid multi-variable initialisation: %d:%d\n", line, column);
        }          
        
    } else if (if_integer(temp) && strcmp(temp1, "EQUAL") == 0 && strcmp(temp2, "ARRAY REFERENCE") == 0) {
        if (stack_size == 3) {
            //push_complete("ARRAY REFERENCE SET");
            char *string = join_string("A", semantic);
            push_complete(string);
            free(string);
            semantic_line[line_array++] = line;
            //display_semantic();
        } else {
            printf("Invalid array reference: %d:%d\n", line, column);
        }
        
        
    } else if (strcmp(temp, "ARR DECL") == 0) {
        if (stack_size == 1) {
            char *string = join_string("B", semantic);
            push_complete(string);
            free(string);
            semantic_line[line_array++] = line;
           // push_complete("ARRAY INITIALISATION");
           // display_semantic();
        } else {
            printf("Invalid array initialisation: %d:%d\n", line, column);
        }
    
    } else if (strcmp(temp, "C_LIB_STR+PARAM") == 0 && strcmp(temp1, "callout") == 0) {
        if (stack_size == 2) {
            //push_complete("callout(C_LIB_STR+PARAM)");

            char *string = join_string("C", semantic);
            push_complete(string);
            free(string);
            semantic_line[line_array++] = line;
            //pure_push_names("))");
            //display_semantic();
        } else {
            printf("Invalid callout attempt: %d:%d\n", line, column);
        } 
        
    } else if (strcmp(temp, "C_LIB_STR") == 0 && strcmp(temp1, "callout") == 0) {
        if (stack_size == 2) {
            //push_complete("callout(C_LIB_STR)");

            char *string = join_string("D", semantic);
            push_complete(string);
            free(string);
            semantic_line[line_array++] = line;
            //display_semantic();
        } else {
            printf("Invalid callout attempt: %d:%d\n", line, column);
        }    
        
    } else if (strcmp(temp, "INTLITERAL") == 0 && strcmp(temp1, "RETURN") == 0 && stack_size == 2) {
        char *string = join_string("E", semantic);
        push_complete(string);
        free(string);
        semantic_line[line_array++] = line;
        
    } else if (strcmp(temp, "IDENTIFIER") == 0 && strcmp(temp1, "RETURN") == 0 && stack_size == 2) {
        char *string = join_string("F", semantic);
        push_complete(string);
        free(string);
        semantic_line[line_array++] = line;
      //  push_complete("RETURN IDENTIFIER");
      //  display_semantic();
        
    } else if (strcmp(temp, "FUNC CALL") == 0 && strcmp(temp1, "RETURN") == 0 && stack_size == 2) {
        char *string = join_string("G", semantic);
        push_complete(string);
        free(string);
        semantic_line[line_array++] = line;
     //   push_complete("RETURN FUNC_CALL");
      //  display_semantic();
    } else if (strcmp(temp, "FUNC CALL w/ARG") == 0 && strcmp(temp1, "RETURN") == 0 && stack_size == 2) {
        char *string = join_string("H", semantic);
        push_complete(string);
        free(string);
        semantic_line[line_array++] = line;
        
    } else if (strcmp(temp, "RETURN") == 0 && stack_size == 1) {
        push_complete("I");
        semantic_line[line_array++] = line;
        
    } else if (strcmp(temp, "FUNC CALL") == 0 && stack_size == 1) {
        char *string = join_string("J", semantic);
        push_complete(string);
        free(string);
        semantic_line[line_array++] = line;
        //push_complete("FUNC_CALL");
        //display_semantic();
        
    } else if (strcmp(temp, "FUNC CALL w/ARG") == 0 && stack_size == 1) {
        char *string = join_string("K", semantic);
        push_complete(string);
        free(string);
        semantic_line[line_array++] = line;
        
    } else {
        printf("Invalid statement: %d:%d %s\n", line, column, temp);
    }
    
    if (bracket_stack.top != 0) {
        fprintf(stderr, "Unmatched bracket \")\"\n");
    }
    
    bracket_stack.top = 0;
    s.top = 0;
    order_count = 0;
    semantic_info.top = 0;
    
    if (strcmp(semantic, "") != 0) {
        free(semantic);
    }
    
    
}
    
void check_integer() {   

    char *temp1 = pop();    
    char *temp2 = pop();    
    
    if (if_operand(temp1) && if_integer(temp2)) {       //3 + 5 -> 8
        pure_push("INTLITERAL");
    } else if (strcmp(temp1, ",") == 0 && if_integer(temp2)) {      //3, 5 
        pure_push("ARGUMENTS_DOUBLE");
    } else if (if_operand(temp1) && strcmp(temp2, "ARGUMENTS_DOUBLE") == 0) {
        pure_push("ARGUMENTS_DOUBLE");  
    } else if (strcmp(temp1, ",") == 0 && (strcmp(temp2, "ARGUMENTS") == 0 || strcmp(temp2, "ARGUMENTS_DOUBLE") == 0)) {    //3, 5, 4
        pure_push("ARGUMENTS");
    } else {
        pure_push(temp2);
        pure_push(temp1);
        pure_push("INTLITERAL");        
    }
    
}

char get_next() {

    int temp = column + 1;
    for (; temp < strlen(current_line); temp++) {
        if (current_line[temp] != ' ') {
            //printf("line number %d, col %d, returning %c\n", line, column, current_line[temp]);
            return current_line[temp];
        }
    }
    
    return '!';
}




























