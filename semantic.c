/*

LOSE INFO
ints to chars 
longs to shorts

WONT LOSE INFO
ints to floats
shorts to longs

***COERCIONS GO UP HIERARCHY***
int - float - double
result = type of operand highest on hierarchy
int + float = float
int + double = double
float + double = double

TYPE CASTING?
A - A + (int)B

***CHECKS***
1. When building parameter descriptor have
    - name of type
    - name of param
Q: is name of type valid?
    - look up name
    - if not there look up name in \e[34mprog\e[0m symbol (could be class)
    - else fails
2. When building local descriptors have
    - name of type
    - name of param
Q: is name of type valid?
    - look up name
    - if not there look up name in \e[34mprog\e[0m symbol (could be class)
    - else fails
3. When building local symbol table, have local descriptors
Q: duplicate/shadowed (local declaration of parameter/global) variable names?
    - check during insertion 
4. When building class descriptor have  (LATER)
    - name of class + superclass
    - field symbol table
    - method symbol table  
Q: Check for:
    - superclass name corresponds to class
    - no name clashes between field names of sub and superclass
    - overidden methods match param and return type decl

LOAD INSTR
Compiler looks up var name in local symbol table, parameter symbol table, field symbol table, if not found --> semantic error

LOAD ARRAY INSTR
Compiler has var name + index expr, look up var name (if not there, error), check type of expression (if not int, error)

ADD OPERATIONS
Compiler has 2 expressions
Check for - right type + both integers
Check types (produced expression for operations)

STORE INSTR
Compiler has var name + expr

DO: Look up var name
    - if in local symbol, reference local descriptor
    - if in parameter symbol, error
    - if in field symbol, reference field descriptor
    - else error
    - check type var against type expr
    
STORE ARRAY INSTR
SAME AS ABOVE - check array index is int

RETURN INSTR
Check if return type matches func type (from func decl)

CONDITIONAL INSTR
If cond expression produces boolean

*/

//WE WILL NEED GLOBAL VAR LINKED LIST - type = int/char/bool/etc.

/* THIS MANY LISTS
Global vars
Local vars - inc parameters
Classes
Functions

*/

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "header.h"
#include <ctype.h>

unsigned int list_size = 0;     //will need many of this

// Different linked lists for classes, local identifiers, global identifiers, functions - functions need parameter areas

//DONE: function decl, class set, main func, variable initialised, array intialised
//DOING: functions, multi var intialise, var set, array ref set     


unsigned int size_list = 0;

#define integer_type 0
#define void_type 1
#define boolean_type 2

Node_function *head_function = NULL;
Node_variable *head_variable = NULL;
Node_class *head_class = NULL;
Node_array *head_array = NULL;

unsigned int current_line;
int global_var = 0;         //if the variable is outside local scope, it's global

void semantic_handler() {

    int i;
    for (i = 0; i <= complete.top - 1; i++) {
        printf ("\e[1m%-25s\e[0m line num: %d\n", complete.stk[i], semantic_line[i]);
        current_line = semantic_line[i];
        insert_type(complete.stk[i]);
    }  
    
    printList_all();
}

void insert_type(char *string1) {

    if (string1[0] == '1') {        //class        
        class_handler(string1);
        
    } else if (string1[0] == '2') {   //main
        
    } else if (string1[0] == '3') {   //func decl
        int temp_type = -1;
        switch (string1[3]) {
            case 'N':
                temp_type = integer_type;
                break;
            case 'V':
                temp_type = void_type;
                break;
            case 'T':
                temp_type = boolean_type;
                break;
        }   
        function_handler(string1, temp_type);
        
    } else if (string1[0] == '4') {
        //printf("Inserting function decl w/param...\n");
    } else if (string1[0] == '5') {
        //printf("Inserting class...\n");
    } else if (string1[0] == '6') {
        //printf("Inserting class...\n");
    } else if (string1[0] == '7') {
        //printf("Inserting class...\n");
    } else if (string1[0] == '8') {         //variable initil.
    
        int temp_type = -1;
        switch (string1[3]) {
            case 'N':
                temp_type = integer_type;
                break;
            case 'V':
                temp_type = void_type;
                break;
            case 'T':
                temp_type = boolean_type;
                break;
        }   
        variable_handler(string1, temp_type);
        
    } else if (string1[0] == '9') {
        //printf("Inserting class...\n");
    } else if (string1[0] == 'A') {
        //printf("Inserting class...\n");
    } else if (string1[0] == 'B') {
        
        int temp_type = -1;
        switch (string1[3]) {
            case 'N':
                temp_type = integer_type;
                break;
            case 'V':
                temp_type = void_type;
                break;
            case 'T':
                temp_type = boolean_type;
                break;
        }   
        array_handler(string1, temp_type);        
        
    } else if (string1[0] == 'C') {
        //printf("Inserting class...\n");
    } else if (string1[0] == 'D') {
        //printf("Inserting class...\n");
    } else if (string1[0] == 'E') {
        //printf("Inserting class...\n");
    } else if (string1[0] == 'F') {
        //printf("Inserting class...\n");
    } else if (string1[0] == 'G') {
        //printf("Inserting class...\n");
    } else if (string1[0] == 'H') {
        //printf("Inserting class...\n");
    } else if (string1[0] == 'I') {
        //printf("Inserting class...\n");
    } else if (string1[0] == 'J') {
        //printf("Inserting class...\n");
    } else if (string1[0] == 'K') {
        //printf("Inserting class...\n");
    } else if (string1[0] == 'L') {
        //printf("Inserting class...\n");
    } else if (string1[0] == 'X') {
        global_var = 0;
        //printf("Inserting class...\n");
    
    }
}

void array_handler(char *string1, unsigned int type) {

    int end = 0;
    int spaces = 0;
    int int_start = 0;
    int int_end = 0;
    
    int i;
    for (i = 5; i < strlen(string1); i++) {
        if (string1[i] == ' ' && end == 0) {
            end = i;
        } else if (string1[i] == '[') {
            int_start = i + 1;
        } else if (string1[i] == ']') {
            int_end = i - 2;                   
        }
    }       
    
    char *new_string = makeString(5, end, string1); 
    
    int scale = 1;
    int literal_size = 0;
        
    for (i = int_end; i > int_start; i--) {
        if (!isdigit(string1[i])) {
            printf("\n\e[34mprog\e[0m:%d array initialisation error: invalid array size, element: '\e[31m%c\e[0m'\nnote: valid array size is of 'positive integers'\n\n", current_line, string1[i]);
            return;
        }
        literal_size = scale * (string1[i] - '0') + literal_size;
        scale = scale * 10;
    }
        
    if (check_class(new_string)) {
        //printf("Context class/function/variable/array initialisation error: \e[31m%s\e[0m already exists as a class\n", string1);
    
    } else if (check_func(new_string)) {
        //printf("Context class/function/variable/array initialisation error: \e[31m%s\e[0m already exists as a function\n", string1);
      
    } else if (check_variable(new_string)) {
        //printf("Context class/function/variable/array initialisation error: \e[31m%s\e[0m already exists as a 
    
    } else if (check_array(new_string)) {
    
    } else {
        insertNode_array(new_string, type, literal_size);
    }
    
    free(new_string);
    
}

void variable_handler(char *string1, unsigned int type) {   //need check global/local
        
    char *new_string = makeString(5, strlen(string1) - 1, string1); 

    if (check_class(new_string)) {
    
    } else if (check_func(new_string)) {
      
    } else if (check_variable(new_string)) {
    
    } else if (check_array(new_string)) {
                 
    } else {
        insertNode_var(new_string, type);
    }
    
    free(new_string);
        
}

void function_handler(char *string1, unsigned int type) {   

    int end = 0;
    
    if (string1[0] == '3') {    //no param
        int i;
        for (i = 5; i < strlen(string1); i++) {
            if (string1[i] == ' ') {
                end = i;
                break;
            }
        }
     }       
    
    global_var = 1;
    
    char *new_string = makeString(5, end, string1); 

    if (check_class(new_string)) {
        //printf("Context class/function/variable/array initialisation error: \e[31m%s\e[0m already exists as a class\n", string1);
    
    } else if (check_func(new_string)) {
        //printf("Context class/function/variable/array initialisation error: \e[31m%s\e[0m already exists as a function\n", string1);
      
    } else if (check_variable(new_string)) {
        //printf("Context class/function/variable/array initialisation error: \e[31m%s\e[0m already exists as a 
    
    } else if (check_array(new_string)) {
    
    } else {
        insertNode_func(new_string, type);
    }
    
    free(new_string);
    
}


void class_handler(char *string1) {


    char *new_string = makeString(3, strlen(string1) - 1, string1);    

    if (check_class(new_string)) {
        //printf("Context class/function/variable/array initialisation error: \e[31m%s\e[0m already exists as a class\n", string1);
    
    } else if (check_func(new_string)) {
        //printf("Context class/function/variable/array initialisation error: \e[31m%s\e[0m already exists as a function\n", string1);
      
    } else if (check_variable(new_string)) {
        //printf("Context class/function/variable/array initialisation error: \e[31m%s\e[0m already exists as a 
    
    } else if (check_array(new_string)) {
    
    } else {
        insertNode_class(new_string);
    }
    
    free(new_string);



}


unsigned int check_array(char *string1) {
    
    if (head_array != NULL) { 
        if (strcmp(head_array->name, string1) == 0) {
            printf("\n\e[34mprog\e[0m:%d class/function/variable/array initialisation error: redefinition of '\e[31m%s\e[0m'\nnote: previous definition of '\e[31m%s\e[0m' was here \e[34mprog\e[0m:%d\n\n", current_line, string1, string1, head_array->line_number);  
            return 1;
        }
                          
        Node_array *temp = head_array;
                
        while (temp->next != NULL) {
            if (strcmp(temp->name, string1) == 0) {
                printf("\n\e[34mprog\e[0m:%d class/function/variable/array initialisation error: redefinition of '\e[31m%s\e[0m'\nnote: previous definition of '\e[31m%s\e[0m' was here \e[34mprog\e[0m:%d\n\n", current_line, string1, string1, head_array->line_number);  
                return 1;
            }
            temp = temp->next;
        }
    }

    return 0;
}

unsigned int check_class(char *string1) {
    
    if (head_class != NULL) { 
        if (strcmp(head_class->name, string1) == 0) {
            printf("\n\e[34mprog\e[0m:%d class/function/variable/array initialisation error: redefinition of '\e[31m%s\e[0m'\nnote: previous definition of '\e[31m%s\e[0m' was here \e[34mprog\e[0m:%d\n\n", current_line, string1, string1, head_class->line_number);  
            return 1;
        }
                          
        Node_class *temp = head_class;
                
        while (temp->next != NULL) {
            if (strcmp(temp->name, string1) == 0) {
                printf("\n\e[34mprog\e[0m:%d class/function/variable/array initialisation error: redefinition of '\e[31m%s\e[0m'\nnote: previous definition of '\e[31m%s\e[0m' was here \e[34mprog\e[0m:%d\n\n", current_line, string1, string1, head_class->line_number);  
                return 1;
            }
            temp = temp->next;
        }
    }

    return 0;
}

unsigned int check_func(char *string1) {

    if (head_function != NULL) { 
        if (strcmp(head_function->name, string1) == 0) {
            printf("\n\e[34mprog\e[0m:%d class/function/variable/array initialisation error: redefinition of '\e[31m%s\e[0m'\nnote: previous definition of '\e[31m%s\e[0m' was here \e[34mprog\e[0m:%d\n\n", current_line, string1, string1, head_function->line_number);  
            return 1;
        }
                          
        Node_function *temp = head_function;
                
        while (temp->next != NULL) {
            if (strcmp(temp->name, string1) == 0) {
                printf("\n\e[34mprog\e[0m:%d class/function/variable/array initialisation error: redefinition of '\e[31m%s\e[0m'\nnote: previous definition of '\e[31m%s\e[0m' was here \e[34mprog\e[0m:%d\n\n", current_line, string1, string1, head_function->line_number);  
                return 1;
            }
            temp = temp->next;
        }
    }

    return 0;
}

unsigned int check_variable(char *string1) {

    if (head_variable != NULL) { 
        if (strcmp(head_variable->name, string1) == 0) {
            printf("\n\e[34mprog\e[0m:%d class/function/variable/array initialisation error: redefinition of '\e[31m%s\e[0m'\nnote: previous definition of '\e[31m%s\e[0m' was here \e[34mprog\e[0m:%d\n\n", current_line, string1, string1, head_variable->line_number);  
            return 1;
        }
                          
        Node_variable *temp = head_variable;
                
        while (temp->next != NULL) {
            if (strcmp(temp->name, string1) == 0) {
                printf("\n\e[34mprog\e[0m:%d class/function/variable/array initialisation error: redefinition of '\e[31m%s\e[0m'\nnote: previous definition of '\e[31m%s\e[0m' was here \e[34mprog\e[0m:%d\n\n", current_line, string1, string1, head_variable->line_number);  
                return 1;
            }
            temp = temp->next;
        }
    }

    return 0;
}


Node_array *makeNode_array(char *string1, unsigned int type_var, unsigned int size) {
    
    Node_array *newNode = malloc(sizeof(struct node_array));
    newNode->name = malloc(strlen(string1) + 1);
    strcpy(newNode->name, string1);
    newNode->line_number = current_line;
    newNode->type = type_var;
    newNode->size_array = size;
    
    newNode->value_set = malloc(sizeof(int) * size);
    int i;
    for (i = 0; i < size; i++) {        //meaning nothing has been initialised
        newNode->value_set[i] = 0;
    }
    
    newNode->value = malloc(sizeof(int) * size);    //getting rid for initialisation
        
    return newNode;
}



Node_variable *makeNode_var(char *string1, unsigned int type_var) {
    
    Node_variable *newNode = malloc(sizeof(struct node_var));
    newNode->name = malloc(strlen(string1) + 1);
    strcpy(newNode->name, string1);
    newNode->line_number = current_line;
    newNode->type = type_var;
    newNode->value_set = 0;
    return newNode;
}

Node_class *makeNode_class(char *string1) {

    Node_class *newNode = malloc(sizeof(struct node_class));
    newNode->name = malloc(strlen(string1) + 1);
    strcpy(newNode->name, string1);
    newNode->line_number = current_line;
    return newNode;
}

Node_function *makeNode_function(char *string1, unsigned int type_var) {

    Node_function *newNode = malloc(sizeof(struct node_function));
    newNode->name = malloc(strlen(string1) + 1);
    strcpy(newNode->name, string1);
    newNode->line_number = current_line;
    newNode->type = type_var;
    return newNode;
}


void insertNode_var(char *string1, unsigned int type_var) {

    Node_variable *new = makeNode_var(string1, type_var);
    assert(new != NULL);
    size_list++;
    if (head_variable == NULL) {
        head_variable = new;
        new->next = NULL;
    } else {
        Node_variable *temp = head_variable;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new;
        new->next = NULL;
    }
}

void insertNode_class(char *string1) {

    Node_class *new = makeNode_class(string1);
    assert(new != NULL);
    size_list++;
    if (head_class == NULL) {
        head_class = new;
        head_class->next = NULL;
    } else {
        Node_class *temp = head_class;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new;
        new->next = NULL;
    }
}

void insertNode_func(char *string1, unsigned int type) {

    Node_function *new = makeNode_function(string1, type);
    assert(new != NULL);
    size_list++;
    if (head_function == NULL) {
        head_function = new;
        head_function->next = NULL;
    } else {
        Node_function *temp = head_function;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new;
        new->next = NULL;
    }
}

void insertNode_array(char *string1, unsigned int type_var, unsigned int size) {

    Node_array *new = makeNode_array(string1, type_var, size);
    assert(new != NULL);
    size_list++;
    if (head_array == NULL) {
        head_array = new;
        new->next = NULL;
    } else {
        Node_array *temp = head_array;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new;
        new->next = NULL;
    }
}

void printList_all() {

    Node_class *temp1 = head_class;
    
    printf("Class list: \n");
    while (temp1->next != NULL) {
        printf("\e[32m%s\e[0m->", temp1->name);
        temp1 = temp1->next;
    }
    printf("\e[32m%s\e[0m\n", temp1->name);
    
    Node_function *temp2 = head_function;
    
    printf("Function list: \n");
    while (temp2->next != NULL) {
        printf("\e[32m%s\e[0m(%d)->", temp2->name, temp2->type);
        temp2 = temp2->next;
    }
    printf("\e[32m%s\e[0m(%d)\n", temp2->name, temp2->type);
    
    Node_variable *temp3 = head_variable;
    
    printf("Variable list: \n");
    while (temp3->next != NULL) {
        printf("\e[32m%s\e[0m(%d)->", temp3->name, temp3->type);
        temp3 = temp3->next;
    }
    printf("\e[32m%s\e[0m(%d)\n", temp3->name, temp3->type);
    
    Node_array *temp4 = head_array;
    printf("Array list: \n");
    while (temp4->next != NULL) {
        printf("\e[32m%s\e[0m(%d)(%d)->", temp4->name, temp4->type, temp4->size_array);
        temp4 = temp4->next;
    }
    printf("\e[32m%s\e[0m(%d)(%d)\n", temp4->name, temp4->type, temp4->size_array);
       
}



/*
1=CLASS SET
2=MAIN FUNC
3=FUNCTION DECLARATION
4=FUNCTION DECLARATION w/PARAM
5=CONDITIONAL
6=ELSE
7=CONDITIONAL_STATEMENT:special
8=VARIABLE INITIALISED
9=VARIABLE VALUE SET
A=ARRAY REFERENCE SET
B=ARRAY INITIALISATION
C=Callout(PARAM)
D=Callout()
E=RETURN INTEGER
F=RETURN IDENTIFIER
G=RETURN FUNC CALL
H=RETURN FUNC CALL w/ARG
I=RETURN VOID
J=FUNC CALL
K=FUNC CALL w/ARG
L=LOOP STATEMENT
M=END FUNCTION
*/





































