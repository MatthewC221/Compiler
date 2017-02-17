#include "header.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>

unsigned int function_identity = 100;           //this will be for local variables, they will have a function identity
unsigned int if_in_function = -1;                  //-1 for not in function, 0 for int function, 1 for void, 2 for bool
unsigned int main_count = 0;                    //requires only one main
 

// Different linked lists for classes, local identifiers, global identifiers, functions - functions need parameter areas

//DONE: refer to semantic_codes.txt
//DOING: array ref set, function decl w/param, return int, return IDE     


//THINGS TO CONSIDER: should we put invalid things into the linked list, e.g. if a = ab and ab is uninit. 
//does it matter if it goes into linked list???

unsigned int size_list = 0;
unsigned int expecting_return = -1;

#define integer_type 0
#define void_type 1
#define boolean_type 2

Node_function *head_function = NULL;
Node_variable *head_variable = NULL;
Node_local_variable *head_local_variable = NULL;
Node_class *head_class = NULL;
Node_array *head_array = NULL;

unsigned int current_line;

void semantic_handler() {

    int i;
    for (i = 0; i <= complete.top - 1; i++) {
        printf ("\e[1m%-40s\e[0m line num: %d\n", complete.stk[i], semantic_line[i]);
        current_line = semantic_line[i];
        insert_type(complete.stk[i]);
    }  
    
    if (main_count == 0) {
        printf("\n\e[34mprog\e[0m:%d non-existent main reference: \e[31mrequires main\e[0m\nnote: program must have one main\n\n", current_line); 
    }
       
    printList_all();
}

void insert_type(char *string1) {

    if (string1[0] == '1') {        //class        
        class_handler(string1);
        
    } else if (string1[0] == '2') {   //main
        if_in_function = 1;           //void
        main_count++;
        if (main_count == 2) {
            printf("\n\e[34mprog\e[0m:%d double main reference: \e[31mmulti-main error\e[0m\nnote: program can only have one main \n\n", current_line);    
        }         
        
    } else if (string1[0] == '3') {   //func decl
   
        function_handler(string1);
        
    } else if (string1[0] == '4') {
 
        function_param_handler(string1);
       
        //printf("Inserting function decl w/param...\n");
    } else if (string1[0] == '5') {
        //printf("Inserting class...\n");
    } else if (string1[0] == '6') {
        //printf("Inserting class...\n");
    } else if (string1[0] == '7') {
        //printf("Inserting class...\n");
    } else if (string1[0] == '8') {         //variable initil.
    
        variable_handler(string1);
        
    } else if (string1[0] == '9') {
            
        variable_set_handler(string1);        
        
    } else if (string1[0] == 'A') {
    
        array_set_handler(string1);
        
    } else if (string1[0] == 'B') {
         
        array_handler(string1);        
        
    } else if (string1[0] == 'C') {
        //printf("Inserting class...\n");
    } else if (string1[0] == 'D') {
        //printf("Inserting class...\n");
    } else if (string1[0] == 'E') {             //return int
        if (if_in_function != 0) {
            printf("\n\e[34mprog\e[0m:%d invalid return value: \e[31mexpecting other\e[0m\nnote: returning integer value in non-int function\n\n", current_line);                       
        } 
        expecting_return = -1;
    } else if (string1[0] == 'F') {             //return identifier
        int i;
        for (i = 3; i < strlen(string1); i++) {
            if (string1[i] == ' ') {
                break;
            }
        }
        
        char *temp_string = makeString(3, i-1, string1);
        int temp = find_type_var(temp_string);
        int temp1 = find_type_local_var(temp_string);
        
        if (temp == -1 && temp1 == -1) {
            printf("\n\e[34mprog\e[0m:%d invalid return value, variable non-existent: \e[31m%s\e[0m\nnote: returning integer value in non-int function\n\n", current_line, temp_string);
        } else if (temp != -1) {
            if (check_variable_exists(temp_string, 0) == 2) {
                printf("\n\e[34mprog\e[0m:%d invalid return value, uninit. value: \e[31m%s\e[0m\nnote: returning invalid type\n\n", current_line, temp_string);  
            } else if (temp != if_in_function) {
                printf("\n\e[34mprog\e[0m:%d invalid return value: \e[31mexpecting other type\e[0m\nnote: returning invalid type\n\n", current_line); 
            }
            
        } else if (temp1 != -1) {
            if (check_local_variable_exists(temp_string, 0) == 2) {
                printf("\n\e[34mprog\e[0m:%d invalid return value, uninit. value: \e[31m%s\e[0m\nnote: returning invalid type\n\n", current_line, temp_string);  
            } else if (temp1 != if_in_function) {                          
                printf("\n\e[34mprog\e[0m:%d invalid return value: \e[31mexpecting other type\e[0m\nnote: returning invalid type\n\n", current_line); 
            } else if (!check_location(temp_string)) {           //checks if the identifier is in local scope
                printf("\n\e[34mprog\e[0m:%d invalid return value: \e[31mout of scope variable\e[0m\nnote: returning invalid type\n\n", current_line);
            }
        }     
        expecting_return = -1;   
                
    } else if (string1[0] == 'G') {
        //printf("Inserting class...\n");
    } else if (string1[0] == 'H') {
        //printf("Inserting class...\n");
    } else if (string1[0] == 'I') {         //return void
        if (if_in_function != 1) {
            printf("\n\e[34mprog\e[0m:%d invalid return value: \e[31mexpecting other type, given void\e[0m\nnote: returning invalid type\n\n", current_line);
        } 
        expecting_return = -1;
    } else if (string1[0] == 'J') {
        //printf("Inserting class...\n");
    } else if (string1[0] == 'K') {
        //printf("Inserting class...\n");
    } else if (string1[0] == 'L') {
        //printf("Inserting class...\n");
    } else if (string1[0] == 'X') {
        if (expecting_return == integer_type) {
            printf("\n\e[34mprog\e[0m:%d missing return value: \e[31mexpecting int type, given other\e[0m\nnote: returning none\n\n", current_line);
        } else if (expecting_return == boolean_type) {
            printf("\n\e[34mprog\e[0m:%d missing return value: \e[31mexpecting boolean type, given other\e[0m\nnote: returning none\n\n", current_line);
        }
        
        if_in_function = -1;
        function_identity = function_identity + 100;
        expecting_return = -1;
    
    }
}

unsigned int check_location(char *string1) {

    Node_local_variable *temp = head_local_variable;
    while (temp != NULL) {
        if (temp->location == function_identity && strcmp(temp->name, string1) == 0) {
            return 1;
        }
        temp = temp->next;
    }
    
    return 0;
}
            


void array_set_handler(char *string1) {     //doesn't check out of bounds, just checks the array exists

    int i = 3;
    int end = 0;
    
    for (; i < strlen(string1); i++) {
        if (string1[i] == ' ' && end == 0) {
            end = i - 1;
        } else if (string1[i] == ']') {
            i = i + 2;
            break;
        }
    }
    
    
    char *temp_string = makeString(3, end, string1);
    
    
    if (check_array_exists(temp_string) == 0) {
        printf("\n\e[34mprog\e[0m:%d array undeclared error (first use in this function): '\e[31m%s\e[0m'\nnote: each undeclared identifier is reported only once\n\n", current_line, temp_string);
    } else {
        int start = i;
        for (i = start; i < strlen(string1); i++) {
            if (string1[i] == ' ' || i == strlen(string1) - 1) {
                char *temp_string1 = makeString(start, i, string1);
                if (check_IDE(temp_string1) && valid_variable(temp_string1)) {
                    printf("\n\e[34mprog\e[0m:%d variable undeclared error (first use in this function): '\e[31m%s\e[0m'\nnote: each undeclared identifier is reported only once\n\n", current_line, temp_string1);
                    
                } 
                start = i + 1;
            }
        }
    }
   

}

void array_handler(char *string1) {

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


    int end = 0;
    int int_start = 0;
    int int_end = 0;
    int type_search = 0;
    
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
    char *temp_str = makeString(int_start + 1, int_end, string1);
            
    
    if (check_INT(temp_str) == 1) {
        type_search = 1;
    } else if (check_IDE(temp_str) == 1) {
        type_search = 2;
    } else {
        printf("\n\e[34mprog\e[0m:%d array initialisation error: invalid array size, element: '\e[31m'-', '+', '/', '*'\e[0m'\nnote: valid array size is of 'positive integers'\n\n", current_line);
        return;
    }
        

    int size = 0;
    int scale = 1;

    if (type_search == 2) {
        int value = check_variable_exists(temp_str, 0);
        if (value == 2) {
            printf("\n\e[34mprog\e[0m:%d array initialisation error: invalid array size, element defined but uninitialised: '\e[31m%s\e[0m'\nnote: valid array size is of 'positive integers'\n\n", current_line, temp_str);  
            return;
        } else if (value == 0) {
            printf("\n\e[34mprog\e[0m:%d array initialisation error: invalid array size, element undefined: '\e[31m%s\e[0m'\nnote: valid array size is of 'positive integers'\n\n", current_line, temp_str); 
            return; 
        } else {
            size = value;
        }
    } else {
        for (int counter = strlen(temp_str) - 1; counter >= 0; counter--) {
            size = scale * (temp_str[counter] - '0') + size;
            scale = scale * 10;
        }
        if (size == 0) {
            printf("\n\e[34mprog\e[0m:%d array initialisation error: invalid array size: '\e[31m%s\e[0m'\nnote: valid array size is of 'positive integers'\n\n", current_line, temp_str); 
        }
    }
                                     


    if (check_class(new_string)) {
    
    } else if (check_func(new_string)) {
      
    } else if (check_variable(new_string)) {
    
    } else if (check_array(new_string)) {
    
    } else {
        insertNode_array(new_string, temp_type, size);
    }
    
    free(new_string);
    free(temp_str);
    
}

void variable_set_handler(char *string1) {      //for array set, variable sets (most important check RHS, e.g. a = b)


    //incomplete, needs to check function calls (make a function that does so)

    int i = 3;
    for (i = 3; i < strlen(string1); i++) {
        if (string1[i] == ' ') {
            break;
        }
    }

    int error_flag = 0;
    char *temp_string = makeString(3, i-1, string1);       
    int temp_val = check_local_variable_exists(temp_string, 0) + check_variable_exists(temp_string, 0);              
    
    if (temp_val == 0) {
        printf("\n\e[34mprog\e[0m:%d variable undeclared error (first use in this function): '\e[31m%s\e[0m'\nnote: each undeclared identifier is reported only once\n\n", current_line, temp_string);
        error_flag = 1;
    } else {
    
        int type_variable = find_type_var(temp_string);
        int type_variable1 = find_type_local_var(temp_string);
        int type_final = 0;
        
        if (type_variable != -1) {
            type_final = type_variable;
        } else {
            type_final = type_variable1;
        }
        int start = i + 1;
        if (type_final == 0) {       //for integer
            for (i = start; i < strlen(string1); i++) {
                if (string1[i] == ' ' || i == strlen(string1) - 1) {
                    char *temp_string1 = makeString(start, i, string1);
                    if (check_IDE(temp_string1) && valid_variable(temp_string1)) {
                        printf("\n\e[34mprog\e[0m:%d variable undeclared error (first use in this function): '\e[31m%s\e[0m'\nnote: each undeclared identifier is reported only once\n\n", current_line, temp_string1);
                        error_flag = 1;
                        
                    } 
                    start = i + 1;
                }
            }
        } else {            //for boolean
            for (i = start; i < strlen(string1); i++) {
                if (string1[i] == ' ' || i == strlen(string1) - 1) {
                    char *temp_string1 = makeString(start, i, string1);
                    if (strcmp(temp_string1, "T_F") != 0) {
                        printf("\n\e[34mprog\e[0m:%d invalid boolean variable arithmetic: '\e[31m%s\e[0m'\nnote: boolean variables can only be true/false\n\n", current_line, temp_string1);   
                        error_flag = 1;
                    }
                }
            }                     
        }
    }
    
    if (error_flag == 0) {      //will not initialise the value if error???
        temp_val = check_local_variable_exists(temp_string, 1);
        temp_val = check_variable_exists(temp_string, 1);
    }
        
} 

unsigned int valid_variable(char *string1) {            //1 if invalid


    return ((check_variable_exists(string1, 0) == 0 || check_variable_exists(string1, 0) == 2 || find_type_var(string1) != 0) && (check_local_variable_exists(string1, 0) == 0 || check_local_variable_exists(string1, 0) == 2 || find_type_local_var(string1) != 0));

}

unsigned int find_type_local_var(char *string1) {

    Node_local_variable *temp = head_local_variable;
    while (temp != NULL) {
        if (strcmp(temp->name, string1) == 0) {
            return temp->type;
        }
        temp = temp->next;
    }    

    return -1;
}




unsigned int find_type_var(char *string1) {

    Node_variable *temp = head_variable;
    while (temp != NULL) {
        if (strcmp(temp->name, string1) == 0) {
            return temp->type;
        }
        temp = temp->next;
    }    

    return -1;
}


unsigned int if_operand_lit(char a) {

    return (a == '+' || a == '-' || a == '/' || a == '*');
}
        


void variable_handler(char *string1) {   //need check global/local

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
    if (temp_type == void_type) {
        printf("can't be void\n");
    } else {
        
        char *new_string = makeString(5, strlen(string1) - 1, string1); 

        if (check_class(new_string)) {
        
        } else if (check_func(new_string)) {
          
        } else if (check_variable(new_string)) {
        
        } else if (check_array(new_string)) {
                     
        } else {
            if (if_in_function == -1) { //if global
                insertNode_var(new_string, temp_type);
            } else {
                insertNode_local_var(new_string, temp_type);
            }
        }
        
        free(new_string);
    }
        
}

void function_param_handler(char *string1) {

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
    
    if_in_function = temp_type;
    expecting_return = temp_type;
    
    int i;
    for (i = 5; i < strlen(string1); i++) {
        if (string1[i] == ' ') {
            break;
        }
    }
    
    char *new_string = makeString(5, i-1, string1);
    
    if (check_class(new_string)) {
    
    } else if (check_func(new_string)) {
      
    } else if (check_variable(new_string)) {
    
    } else if (check_array(new_string)) {
                 
    } else {
    
        insertNode_func(string1, temp_type, 1);
    }
     


}

void function_handler(char *string1) {   

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

    expecting_return = temp_type;
    if_in_function = temp_type;

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
    
    char *new_string = makeString(5, end, string1); 

    if (check_class(new_string)) {
        //printf("Context class/function/variable/array initialisation error: \e[31m%s\e[0m already exists as a class\n", string1);
    
    } else if (check_func(new_string)) {
        //printf("Context class/function/variable/array initialisation error: \e[31m%s\e[0m already exists as a function\n", string1);
      
    } else if (check_variable(new_string)) {
        //printf("Context class/function/variable/array initialisation error: \e[31m%s\e[0m already exists as a 
    
    } else if (check_array(new_string)) {
    
    } else {
        insertNode_func(new_string, temp_type, 0);
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
                           
    Node_array *temp = head_array;
                
    while (temp != NULL) {
        if (strcmp(temp->name, string1) == 0) {
            printf("\n\e[34mprog\e[0m:%d class/function/variable/array initialisation error: redefinition of '\e[31m%s\e[0m'\nnote: previous definition of '\e[31m%s\e[0m' was here \e[34mprog\e[0m:%d\n\n", current_line, string1, string1, temp->line_number);  
            return 1;
        }
        temp = temp->next;
    }

    return 0;
}

unsigned int check_class(char *string1) {
    
    Node_class *temp = head_class;
            
    while (temp != NULL) {
        if (strcmp(temp->name, string1) == 0) {
            printf("\n\e[34mprog\e[0m:%d class/function/variable/array initialisation error: redefinition of '\e[31m%s\e[0m'\nnote: previous definition of '\e[31m%s\e[0m' was here \e[34mprog\e[0m:%d\n\n", current_line, string1, string1, temp->line_number);  
            return 1;
        }
        temp = temp->next;
    }

    return 0;
}

unsigned int check_func(char *string1) {


                          
    Node_function *temp = head_function;
            
    while (temp != NULL) {
        if (strcmp(temp->name, string1) == 0) {
            printf("\n\e[34mprog\e[0m:%d class/function/variable/array initialisation error: redefinition of '\e[31m%s\e[0m'\nnote: previous definition of '\e[31m%s\e[0m' was here \e[34mprog\e[0m:%d\n\n", current_line, string1, string1, temp->line_number);  
            return 1;
        }
        temp = temp->next;
    }

    return 0;
}

unsigned int check_variable(char *string1) {    //checks if it exists, if does throw err

    Node_variable *temp = head_variable;            
    while (temp != NULL) {
        if (strcmp(temp->name, string1) == 0) {
            printf("\n\e[34mprog\e[0m:%d class/function/variable/array initialisation error: redefinition of '\e[31m%s\e[0m'\nnote: previous definition of '\e[31m%s\e[0m' was here \e[34mprog\e[0m:%d\n\n", current_line, string1, string1, temp->line_number);  
            return 1;
        }
        temp = temp->next;
    }

    return 0;
}

unsigned int check_local_variable(char *string1) {    //checks if it exists, if does throw err

    Node_local_variable *temp = head_local_variable;
                
    while (temp != NULL) {
        if (strcmp(temp->name, string1) == 0 && function_identity == temp->location) {
            printf("\n\e[34mprog\e[0m:%d class/function/variable/array initialisation error: redefinition of '\e[31m%s\e[0m'\nnote: previous definition of '\e[31m%s\e[0m' was here \e[34mprog\e[0m:%d\n\n", current_line, string1, string1, temp->line_number);  
            return 1;
        }
        temp = temp->next;
    }

    return 0;
}


unsigned int check_variable_exists(char *string1, int make_true) {     

    //return 0 if non-existent, 2 if defined but no value, value of var is exists and defined
    //make true if setting var initialise
                          
    Node_variable *temp = head_variable;
            
    while (temp != NULL) {
        if (strcmp(temp->name, string1) == 0) {
            if (temp->value_set == false) {
                if (make_true == 1) temp->value_set = true;
                
                return 2;
            } else {
                return 555;      //for later computation
            }
        }
        temp = temp->next;
    }

    return 0;
}

unsigned int check_local_variable_exists(char *string1, int make_true) {     

    //return 0 if non-existent, 2 if defined but no value, value of var is exists and defined
    //make true if setting var initialise
                          
    Node_local_variable *temp = head_local_variable;
            
    while (temp != NULL) {
        if (strcmp(temp->name, string1) == 0 && temp->location == function_identity) {      //in current func
            if (temp->value_set == false) {
                if (make_true == 1) temp->value_set = true;
                
                return 2;
            } else {
                return 555;      //for later computation
            }
        }
        temp = temp->next;
    }

    return 0;
}

unsigned int check_array_exists(char *string1) {     

    //return 0 if non-existent, 2 if defined but no value, value of var is exists and defined
    //make true if setting var initialise
                          
    Node_array *temp = head_array;
            
    while (temp != NULL) {
        if (strcmp(temp->name, string1) == 0) {                
            return 2;
        }
        temp = temp->next;
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
        newNode->value_set[i] = false;
    }
    
    newNode->value = malloc(sizeof(int) * size);    //getting rid for initialisation
        
    return newNode;
}

Node_local_variable *makeNode_local_var(char *string1, unsigned int type_var) {
    
    Node_local_variable *newNode = malloc(sizeof(struct node_local_var));
    newNode->name = malloc(strlen(string1) + 1);
    strcpy(newNode->name, string1);
    newNode->line_number = current_line;
    newNode->type = type_var;
    newNode->value_set = false;
    newNode->value = 0;
    newNode->location = function_identity;
    return newNode;
}


Node_variable *makeNode_var(char *string1, unsigned int type_var) {
    
    Node_variable *newNode = malloc(sizeof(struct node_var));
    newNode->name = malloc(strlen(string1) + 1);
    strcpy(newNode->name, string1);
    newNode->line_number = current_line;
    newNode->type = type_var;
    newNode->value_set = false;
    newNode->value = 0;
    return newNode;
}

Node_class *makeNode_class(char *string1) {

    Node_class *newNode = malloc(sizeof(struct node_class));
    newNode->name = malloc(strlen(string1) + 1);
    strcpy(newNode->name, string1);
    newNode->line_number = current_line;
    return newNode;
}

Node_function *makeNode_function(char *string1, unsigned int type_var, int if_parameter) {

    Node_function *newNode = malloc(sizeof(struct node_function));
    newNode->line_number = current_line;
    newNode->type = type_var;
    if (if_parameter == 0) {
        newNode->name = malloc(strlen(string1) + 1);
        strcpy(newNode->name, string1);
        newNode->has_parameters = 0;
    } else {
        int temp = 5;
        for (; temp < strlen(string1); temp++) {
            if (string1[temp] == ' ') {
                break;
            }
        }
        
        char *temp_string = makeString(5, temp-1, string1);
        newNode->name = malloc(strlen(temp_string) + 1);
        strcpy(newNode->name, temp_string);
      
        int start_param = 0;
        int count = 0;
        for (int i = 0; i < strlen(string1); i++) {
            if (string1[i] == '(') {
                i++;
                start_param = i + 1;
            } else if (string1[i] == ' ' && start_param != 0) {
                count++;
            }
        }        
        
        int parameter_count = 0;
        int start_variable = start_param;
        
        newNode->parameter_types = malloc(sizeof(int) * count/2);
        for (int i = start_param; i < strlen(string1); i++) {
            if (string1[i] == ' ') {
                count++;
                if (count % 2 != 0) {
                    switch(string1[i - 1]) {
                        case 'N':
                            newNode->parameter_types[parameter_count++] = 0;
                            break;
                        case 'V':
                            printf("Invalid void argument\n");
                            break;
                        case 'T':
                            newNode->parameter_types[parameter_count++] = 2;
                            break;
                    }
                    start_variable = i + 1;
                } else {
                    char *temp_string = makeString(start_variable, i, string1);
                    insertNode_local_var(temp_string, newNode->parameter_types[parameter_count - 1]);
                }
            }            
        }
        newNode->has_parameters = parameter_count;
    }
    
    
    return newNode;
}

void insertNode_local_var(char *string1, unsigned int type_var) {

    if (!check_variable(string1) && !check_local_variable(string1)) {
        Node_local_variable *new = makeNode_local_var(string1, type_var);
        assert(new != NULL);
        size_list++;
        if (head_local_variable == NULL) {
            head_local_variable = new;
            new->next = NULL;
        } else {
            Node_local_variable *temp = head_local_variable;
            while (temp->next != NULL) {
                temp = temp->next;
            }
            temp->next = new;
            new->next = NULL;
        }
    } else {
        printf("Conflicting local, global\n");
    }
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

void insertNode_func(char *string1, unsigned int type, int if_parameter) {

    Node_function *new = makeNode_function(string1, type, if_parameter);
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

    printf("\n\n\n");

    Node_class *temp1 = head_class;
    
    printf("Class list: \n");
    
    if (temp1 != NULL) {
        while (temp1->next != NULL) {
            printf("\e[32m%s\e[0m->", temp1->name);
            temp1 = temp1->next;
        }
        printf("\e[32m%s\e[0m\n", temp1->name);
    }
    
    Node_function *temp2 = head_function;
    
    printf("Function list: \n");
    
    if (temp2 != NULL) {    
        while (temp2->next != NULL) {
            printf("\e[32m%s\e[0m(%d)(%d)->", temp2->name, temp2->type, temp2->has_parameters);
            temp2 = temp2->next;
        }
        printf("\e[32m%s\e[0m(%d)(%d)\n", temp2->name, temp2->type, temp2->has_parameters);    
    }
    
    Node_variable *temp3 = head_variable;
    
    printf("Global variable list: \n");
    
    if (temp3 != NULL) {
        while (temp3->next != NULL) {
            printf("\e[32m%s\e[0m(%d)(%d)->", temp3->name, temp3->type, temp3->value_set);
            temp3 = temp3->next;
        }
        printf("\e[32m%s\e[0m(%d)(%d)\n", temp3->name, temp3->type, temp3->value_set);
    }
    
    Node_array *temp4 = head_array;
    printf("Array list: \n");
    
    if (temp4 != NULL) {
        while (temp4->next != NULL) {
            printf("\e[32m%s\e[0m(%d)(%d)->", temp4->name, temp4->type, temp4->size_array);
            temp4 = temp4->next;
        }
        printf("\e[32m%s\e[0m(%d)(%d)\n", temp4->name, temp4->type, temp4->size_array);
    }
      

    Node_local_variable *temp5 = head_local_variable;
    printf("Local variable list: \n");
    
    if (temp5 != NULL) {
        while (temp5->next != NULL) {
            printf("\e[32m%s\e[0m(%d)(%d)->", temp5->name, temp5->type, temp5->location);
            temp5 = temp5->next;
        }
        printf("\e[32m%s\e[0m(%d)(%d)\n", temp5->name, temp5->type, temp5->location);
    }      

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

unsigned int check_INT(char *input) {      //return 1 if +ve integer, 2 if -ve integer

    int return_value = 1;
    
    if (!isdigit(input[0]) && input[0] != '-') {
        return 0;        
    } else {
        for (int i = 1; i < strlen(input); i++) {
            if (!isdigit(input[i])) {
                return 0;
            }
        }                          
    }        
    
    if (input[0] == '-') return_value = 2;
    return return_value;


}

unsigned int check_IDE(char *input) {      //return 1 if identifier

    int return_value = 1;
    
    if (!isalpha(input[0]) && input[0] != '_') {
        return 0;
    } else {
        for (int i = 1; i < strlen(input); i++) {
            if (!isdigit(input[i]) && !isalpha(input[i]) && input[i] != '_') {
                return 0;
            }
        }                          
    }

    return return_value;
}

































