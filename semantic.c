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
//DOING: function call w/arg, multi var init.     


//URGENT: Design flaw, should return nodes with all info...to compare

//THINGS TO CONSIDER: should we put invalid things into the linked list, e.g. if a = ab and ab is uninit. 
//does it matter if it goes into linked list???

unsigned int size_list = 0;
unsigned int expecting_return = -1;
unsigned int return_count = 0;              //number of returns in a function

unsigned int within_loop = 0;

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
        printf ("\e[1m%-55s\e[0m line num: %d\n", complete.stk[i], semantic_line[i]);
        current_line = semantic_line[i];
        insert_type(complete.stk[i]);
    }  
    
    if (main_count == 0) {
        printf("\n\e[34mprog\e[0m:%d non-existent main reference: \e[31mrequires main\e[0m\nnote: program must have one main\n\n", current_line); 
    }
       
    printList_all();
    free_all();
    

    
}

void insert_type(char *string1) {   //REPLACE WITH SWITCH LATER

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
        
        conditional_handler(string1);
        
    } else if (string1[0] == '6') {
        //printf("Inserting class...\n");
    } else if (string1[0] == '7') {
        
        special_conditional_handler(string1);
        
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
        
        return_count++;
        
    } else if (string1[0] == 'F') {             //return identifier
        int i;
        for (i = 3; i < strlen(string1); i++) {
            if (string1[i] == ' ') {
                break;
            }
        }
        
        char *temp_string = makeString(3, i, string1);
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
        
        return_count++;   
        free(temp_string);  
                
    } else if (string1[0] == 'G' || string1[0] == 'H') {
      
        int i = 3;
        for (; i < strlen(string1); i++) {
            if (string1[i] == ' ') {
                break;
            }
        }
        
        char *temp_string = makeString(3, i, string1);
        int temp_type = find_type_func(temp_string);
        
        if (temp_type != expecting_return || function_identity == -1) {
            printf("\n\e[34mprog\e[0m:%d invalid return value: \e[31mexpecting other type\e[0m\nnote: returning invalid type\n\n", current_line);
        }                
      
      
        if (string1[0] == 'H') {
            function_call_handler(string1, 1, 0);
        } else {
            function_call_handler(string1, 0, 0);
        }
        
        return_count++;
        free(temp_string);
        
    } else if (string1[0] == 'I') {         //return void
        if (if_in_function != 1) {
            printf("\n\e[34mprog\e[0m:%d invalid return value: \e[31mexpecting other type, given void\e[0m\nnote: returning invalid type\n\n", current_line);
        }         
        
    } else if (string1[0] == 'J') {         //function call w/o PARAM
        
        function_call_handler(string1, 0, 1);
        
    } else if (string1[0] == 'K') {
    
        function_call_handler(string1, 1, 1);
        
    } else if (string1[0] == 'L') {
    
        within_loop++;
    
    } else if (string1[0] == 'M') {
    
        multi_var_handler(string1);
        
    } else if (string1[0] == 'O') {
    
        if (within_loop == 0) {
            printf("\n\e[34mprog\e[0m:%d invalid special: \e[31m'break'\e[0m\nnote: break must be within loop\n\n", current_line);  
        }
              
    } else if (string1[0] == 'P') {

        if (within_loop == 0) {
            printf("\n\e[34mprog\e[0m:%d invalid special: \e[31m'continue'\e[0m\nnote: break must be within loop\n\n", current_line);  
        }
        
    } else if (string1[0] == 'Q') {
        
        within_loop = 0;

    } else if (string1[0] == 'X') {
        if (expecting_return == integer_type && return_count == 0) {
            printf("\n\e[34mprog\e[0m:%d missing return value: \e[31mexpecting int type, given other\e[0m\nnote: returning none\n\n", current_line);
        } else if (expecting_return == boolean_type && return_count == 0) {
            printf("\n\e[34mprog\e[0m:%d missing return value: \e[31mexpecting boolean type, given other\e[0m\nnote: returning none\n\n", current_line);
        }
        
        return_count = 0;
        if_in_function = -1;
        function_identity = function_identity + 100;
        
    
    }
}

void special_conditional_handler(char *string1) {

    int i = 5;
    int start = i;
    int array_flag = 0;
    int function_flag = 0;
    int count_spaces = 0;
    
    for (; i < strlen(string1); i++) {
        if (string1[i] == ' ') {
            if (string1[i + 1] == '[') {
                array_flag = 1;
            } else if (string1[i + 1] == '(') {
                function_flag = 1;
                i++;
                for (; i < strlen(string1); i++) {
                    if (string1[i] == ')') {
                        i++;
                        break;
                    } else if (string1[i] == ' ') {
                        count_spaces++;
                    }
                }
            }
            break;
        }
    }

    char *temp_string = makeString(start, i-1, string1);
    
    if (function_flag) {
        char *empty = "   ";
        char *new_string = malloc(strlen(temp_string) + 4);
        strcpy(new_string, empty);
        strcat(new_string, temp_string);
        if (count_spaces >= 2) {
            function_call_handler(new_string, 1, 0);
        } else {
            function_call_handler(new_string, 0, 0);
        }    
        count_spaces = 0;   
    } else if (array_flag) {
        Node_array *temp_array = get_array_node(temp_string);
        if (temp_array == NULL || temp_array->location != function_identity) {
            printf("\n\e[34mprog\e[0m:%d array reference undeclared error (first use in this function): '\e[31m%s\e[0m'\nnote: each undeclared identifier is reported only once\n\n", current_line, temp_string); 
        }   
        free(temp_array);         
    } else if (check_IDE(temp_string)) {
        if (find_type_local_var(temp_string) == -1 && find_type_var(temp_string) == -1) {
            printf("\n\e[34mprog\e[0m:%d variable undeclared error (first use in this function): '\e[31m%s\e[0m'\nnote: each undeclared identifier is reported only once\n\n", current_line, temp_string);        
        }
    }



}

void conditional_handler(char *string1) {

    int array_flag = 0;
    int func_flag = 0;
    int count_spaces = 0;
    int i = 5;
    int start = i;
    
    for (; i < strlen(string1); i++) {
        if (string1[i] == ' ') {
            if (string1[i + 1] == '[') {
                array_flag = 1;
            } 
            break;
        }
    }

    char *temp_string = makeString(start, i-1, string1);
    
    if (string1[i + 1] == '(' && check_IDE(temp_string)) {
        func_flag = 1;
        i++;
        while (1) {
            if (string1[i] == ')') {
                break;
            } else if (string1[i] == ' ') {
                count_spaces++;
            }
            i++;
        }
    }
    
    int first_type = -1;
    
    if (array_flag) {
        array_flag = 0;
        Node_array *temp_array = get_array_node(temp_string);
        if (temp_array == NULL || temp_array->location != function_identity) {
            printf("\n\e[34mprog\e[0m:%d array reference undeclared error (first use in this function): '\e[31m%s\e[0m'\nnote: each undeclared identifier is reported only once\n\n", current_line, temp_string);         
        
        } else {
            first_type = temp_array->type;
        } 
    } else if (func_flag) {
        Node_function *temp = get_function_node(temp_string);
        char *temp_string1 = makeString(start, i, string1);
        char *empty = "   ";
        char *new_string = malloc(strlen(temp_string1) + 4);
        strcpy(new_string, empty);
        strcat(new_string, temp_string1);
        if (count_spaces >= 2) {
            function_call_handler(new_string, 1, 0);
        } else {
            function_call_handler(new_string, 0, 0);
        }
        if (temp != NULL) first_type = temp->type;
        count_spaces = 0;
       
    } else if (check_IDE(temp_string)) {
        int global_type = find_type_var(temp_string);
        int local_type = find_type_local_var(temp_string);
        
        if (global_type != -1) {
            first_type = global_type;
        } else if (local_type != -1) {
            first_type = local_type;
        } else {
            printf("\n\e[34mprog\e[0m:%d variable undeclared error (first use in this function): '\e[31m%s\e[0m'\nnote: each undeclared identifier is reported only once\n\n", current_line, temp_string);  
    
        }
    } else {
        first_type = 0;
    }    
    
    int start_count = 0;
    func_flag = 0;
    array_flag = 0;
    
    for (; i < strlen(string1); i++) {
        if ((string1[i] == '>' || string1[i] == '<' || string1[i] == '=') && string1[i + 1] == ' ') {
            start_count = 1;
            i++;
            start = i + 1;
        } else if (string1[i] == ' ' && start_count == 1) {
            if (string1[i + 1] == '[') {
                array_flag = 1;
            }
            break;
        }
    } 

    char *temp_string1 = makeString(start, i-1, string1);

    int second_type = -1;
    
    if (string1[i + 1] == '(' && check_IDE(temp_string1)) {
        func_flag = 1;
        i++;
        while (1) {
            if (string1[i] == ')') {
                break;
            } else if (string1[i] == ' ') {
                count_spaces++;
            }
            i++;
        }
    }    
    
   
    if (array_flag) {
        array_flag = 0;
        Node_array *temp_array = get_array_node(temp_string1);
        if (temp_array == NULL || temp_array->location != function_identity) {
            printf("\n\e[34mprog\e[0m:%d array reference undeclared error (first use in this function): '\e[31m%s\e[0m'\nnote: each undeclared identifier is reported only once\n\n", current_line, temp_string1);         
        
        } else {
            second_type = temp_array->type;
        } 
    } else if (func_flag) {

        Node_function *temp_func = get_function_node(temp_string1);

        char *temp_string2 = makeString(start, i, string1);
        char *empty = "   ";
        char *new_string = malloc(strlen(temp_string2) + 4);
        strcpy(new_string, empty);
        strcat(new_string, temp_string2);

        if (count_spaces >= 2) {
            function_call_handler(new_string, 1, 0);
        } else {
            function_call_handler(new_string, 0, 0);
        }
        if (temp_func != NULL) {
            second_type = temp_func->type;
        }
        count_spaces = 0;
    } else if (check_IDE(temp_string1)) {
        int global_type = find_type_var(temp_string1);
        int local_type = find_type_local_var(temp_string1);
        
        if (global_type != -1) {
            second_type = global_type;
        } else if (local_type != -1) {
            second_type = local_type;
        } else {
            printf("\n\e[34mprog\e[0m:%d variable undeclared error (first use in this function): '\e[31m%s\e[0m'\nnote: each undeclared identifier is reported only once\n\n", current_line, temp_string1);  
    
        }
    } else {
        second_type = 0;
    }

    if (second_type != first_type) {
        printf("\n\e[34mprog\e[0m:%d incompatible comparison: '\e[31m%s, %s\e[0m'\nnote: conditional doesn't return boolean\n\n", current_line, temp_string, temp_string1);
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
    
        Node_array *array_temp = get_array_node(temp_string);
        int arr_type = array_temp->type;
        
        int start = i;
        int array_flag = 0;
        int function_flag = 0;
        int count_spaces = 0;
        
        for (i = start; i < strlen(string1); i++) {
            if (string1[i] == ' ' || i == strlen(string1) - 1) {
                char *temp_string1 = makeString(start, i, string1);
                
                if (string1[i + 1] == '[') {
                    for (; i < strlen(string1); i++) {
                        if (string1[i] == ']') {
                            i++;
                            array_flag = 1;
                            break;
                        }
                    }
                } else if (string1[i + 1] == '(' && check_IDE(temp_string1)) {
                    function_flag = 1;
                    i++;
                    for (; i < strlen(string1); i++) {
                        if (string1[i] == ')') {
                            i++;
                            break;
                        } else if (string1[i] == ' ') {
                            count_spaces++;
                        }
                    }
                } else if (string1[i + 1] == '(') {
                    i++;
                }
                
                if (function_flag) {
                    char *real_string = makeString(start, i, string1);
                    function_flag = 0;
                    char *empty = "   ";
                    char *new_string = malloc(strlen(real_string) + 4);
                    strcpy(new_string, empty);
                    strcat(new_string, real_string);
                    
                    if (count_spaces >= 2) {
                        function_call_handler(new_string, 1, 0);
                    } else {
                        function_call_handler(new_string, 0, 0);
                    } 
                    count_spaces = 0;
                
                    //special checks for array
                } else if (array_flag) {
                    array_flag = 0;
                    Node_array *temp_array = get_array_node(temp_string1);
                    if (temp_array == NULL || temp_array->location != function_identity) {
                        printf("\n\e[34mprog\e[0m:%d array reference undeclared error (first use in this function): '\e[31m%s\e[0m'\nnote: each undeclared identifier is reported only once\n\n", current_line, temp_string1);  
                    } else if (temp_array->type != arr_type) {
                        printf("\n\e[34mprog\e[0m:%d array reference undeclared error (first use in this function): '\e[31m%s\e[0m'\nnote: each undeclared identifier is reported only once\n\n", current_line, temp_string1);                     
                    }
                                                     
                } else if (check_IDE(temp_string1) && valid_variable(temp_string1)) {
                    printf("\n\e[34mprog\e[0m:%d variable undeclared error (first use in this function): '\e[31m%s\e[0m'\nnote: each undeclared identifier is reported only once\n\n", current_line, temp_string1);
                    
                } 
                start = i + 1;
                free(temp_string1);
            } else if (if_operand_lit(string1[i])) {
                start = i + 2;
            }
        }
    }
    
    free(temp_string);

}

void function_call_handler(char *string1, int param, int void_call) {         //checks if the function is void + if exists etc.

    //printf("String received was %s, param = %d\n", string1, param);

    int i = 3;
    for (; i < strlen(string1); i++) {
        if (string1[i] == ' ') {
            break;
        }
    }
    
    char *temp_string = makeString(3, i, string1);
    
    int temp = find_type_func(temp_string);
    int param_count = find_func_param(temp_string);
    
    bool error_flag = false;
    
    if (temp == -1) {       //if incompatible type
        printf("\n\e[34mprog\e[0m:%d invalid func call, function undeclared error (first use in this function): '\e[31m%s\e[0m'\nnote: each undeclared function is reported only once\n\n", current_line, temp_string);
        error_flag = true;
    } else if ((temp == 0 || temp == 2) && void_call == 1) {           //if bool or int, invalid call        
        printf("\n\e[34mprog\e[0m:%d invalid func call, no return error: '\e[31m%s\e[0m'\nnote: bool/int function expects assignment/comparison\n\n", current_line, temp_string);
        error_flag = true;   
    } else if (param == 0 && param_count != 0) {
        printf("\n\e[34mprog\e[0m:%d invalid func call: '\e[31m%s\e[0m'\nnote: function expects parameter/s\n\n", current_line, temp_string);      
        error_flag = true;   
    } else if (param == 1 && param_count == 0) {
        printf("\n\e[34mprog\e[0m:%d invalid func call, parameters given: '\e[31m%s\e[0m'\nnote: function expects none\n\n", current_line, temp_string);
        error_flag = true;
    } 
    
    if (error_flag == true) return;
    
    Node_function *temp_func = get_function_node(temp_string);
    
    if (void_call == 0 && temp_func->type == void_type) {
        printf("\n\e[34mprog\e[0m:%d invalid func call: '\e[31m%s\e[0m'\nnote: function returns void\n\n", current_line, temp_string);
        return;  
    } 
    
    
    if (param == 1 && param_count != -1) {  //function exists and we've called with arguments   
    
        i = i + 3;
        int start = i;
        int count = 0;
        for (; i < strlen(string1); i++) {
            if (string1[i] == ' ') {
                count++;
            } else if (string1[i] == '[') {
                count = count - 3;
            } else if (if_operand_lit(string1[i])) {
                i++;
            }           
        }
             
        
        
        if (count != temp_func->has_parameters) {
            printf("\n\e[34mprog\e[0m:%d invalid func call, parameter mismatch: '\e[31m%s\e[0m'\nnote: function expects other\n\n", current_line, temp_string);
        } else {
        
            int type_counter = 0;
            int array_flag = 0;
            
            for (i = start; i < strlen(string1); i++) {
                if (string1[i] == ' ') {                                        //when new var/int
                                        
                    char *temp_string1 = makeString(start, i, string1);
                    
                    if (string1[i + 1] == '[') {
                        for (; i < strlen(string1); i++) {
                            if (string1[i] == ']') {
                                i++;
                                array_flag = 1;
                                break;
                            }
                        }
                    }
                    
                    
                    if (array_flag == 1) {
                        array_flag = 0;
                        Node_array *temp_array = get_array_node(temp_string1);
                        if (temp_array == NULL || temp_array->location != function_identity) {
                            printf("\n\e[34mprog\e[0m:%d invalid func call, parameter undefined/unit.: '\e[31m%s\e[0m'\nnote: function expects other\n\n", current_line, temp_string1);                         
                        
                        } else if (temp_array->type != temp_func->parameter_types[type_counter++]) {
                            printf("\n\e[34mprog\e[0m:%d invalid func call, parameter type mismatch: '\e[31m%s\e[0m'\nnote: function expects other\n\n", current_line, temp_string1); 
                        }                             
                        
                    
                    
                    
                    } else if (check_INT(temp_string1)) {                              //if int
                        if (temp_func->parameter_types[type_counter] != 0) {
                            printf("\n\e[34mprog\e[0m:%d invalid func call, parameter mismatch: '\e[31m%s\e[0m'\nnote: function expects non-int first argument\n\n", current_line, temp_string1); 
                        }
                        
                    } else {                                                 //if identifier                    
                    
                        Node_local_variable *temp_var = get_local_var_node(temp_string1);
                        if (temp_var == NULL || temp_var->location != function_identity || temp_var->value_set == false) {
                            printf("\n\e[34mprog\e[0m:%d invalid func call, parameter undefined/unit.: '\e[31m%s\e[0m'\nnote: function expects other\n\n", current_line, temp_string1); 
                            
                             
                        } else if (temp_var->type != temp_func->parameter_types[type_counter]) {
                            printf("\n\e[34mprog\e[0m:%d invalid func call, parameter type mismatch: '\e[31m%s\e[0m'\nnote: function expects other\n\n", current_line, temp_string1); 
                        }                                           
                        type_counter++; 
                        
                    }
                    start = i + 1; 
                    free(temp_string1);                                                     
                }
            } 
        }             
    }
    
    free(temp_string);
        
        
        
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
    char *temp_string = makeString(3, i, string1);       
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
        } else if (type_variable1 != -1) {
            type_final = type_variable1;
        }
        
        
        int start = i + 1;
        int array_flag = 0;
        int function_flag = 0;
        int count_spaces = 0;
        
        if (type_final == 0) {       //for integer
            for (i = start; i < strlen(string1); i++) {
                if (string1[i] == ' ' || i == strlen(string1) - 1) {
                    char *temp_string1 = makeString(start, i, string1);
                    
                    //for arrays
                    if (string1[i + 1] == '[' && check_IDE(temp_string1)) {
                        for (; i < strlen(string1); i++) {
                            if (string1[i] == ']') {
                                i++;
                                array_flag = 1;
                                break;
                            }
                        }   //for functions
                    } else if (string1[i + 1] == '(' && check_IDE(temp_string1)) {
                        function_flag = 1;
                        i++;
                        for (; i < strlen(string1); i++) {
                            if (string1[i] == ')') {
                                i++;
                                break;
                            } else if (string1[i] == ' ') {
                                count_spaces++;
                            }
                        }
                    } else if (string1[i + 1] == '(') {
                        i++;
                    }
                
                if (function_flag) {
                    char *real_string = makeString(start, i, string1);
                    function_flag = 0;
                    char *empty = "   ";
                    char *new_string = malloc(strlen(real_string) + 4);
                    strcpy(new_string, empty);
                    strcat(new_string, real_string);
                    
                    if (count_spaces >= 2) {
                        function_call_handler(new_string, 1, 0);
                    } else {
                        function_call_handler(new_string, 0, 0);
                    } 
                    count_spaces = 0;
                    
                    //special checks for array
                } else if (array_flag) {
                    array_flag = 0;
                    Node_array *temp_array = get_array_node(temp_string1);
                    if (temp_array == NULL || temp_array->location != function_identity) {
                        printf("\n\e[34mprog\e[0m:%d array reference undeclared error (first use in this function): '\e[31m%s\e[0m'\nnote: each undeclared identifier is reported only once\n\n", current_line, temp_string1);  
                        error_flag = 1;
                        
                    } else if (temp_array->type != type_final) {
                        printf("\n\e[34mprog\e[0m:%d array reference type error: '\e[31m%s\e[0m'\nnote: incompatible type assignment\n\n", current_line, temp_string1);  
                        error_flag = 1;
                    }
                    
                } else if (check_IDE(temp_string1) && valid_variable(temp_string1)) {
                    printf("\n\e[34mprog\e[0m:%d variable undeclared error (first use in this function): '\e[31m%s\e[0m'\nnote: each undeclared identifier is reported only once\n\n", current_line, temp_string1);
                    error_flag = 1;
                    
                } 
                free(temp_string1);
                start = i + 1;
            } else if (if_operand_lit(string1[i])) {
                start = i + 2;
            }
            
        }
            
        } else {            //for boolean
            for (i = start; i < strlen(string1); i++) {
                if (string1[i] == ' ' || i == strlen(string1) - 1) {
                    char *temp_string1 = makeString(start, i, string1);
                    if (strcmp(temp_string1, "T_F") != 0) {
                        printf("\n\e[34mprog\e[0m:%d invalid boolean variable arithmetic: '\e[31m%s\e[0m'\nnote: boolean variables can only be true/false\n\n", current_line, temp_string1);   
                        error_flag = 1;
                    } else if (if_operand_lit(string1[i])) {
                        start = i + 2;
                    }
                    free(temp_string1);
                }
            }                     
        }
    }
    
    if (error_flag == 0) {      //will not initialise the value if error???
        temp_val = check_local_variable_exists(temp_string, 1);
        temp_val = check_variable_exists(temp_string, 1);
    }
    
    free(temp_string);
        
} 

unsigned int valid_variable(char *string1) {            //1 if invalid


    return ((check_variable_exists(string1, 0) == 0 || check_variable_exists(string1, 0) == 2 || find_type_var(string1) != 0) && (check_local_variable_exists(string1, 0) == 0 || check_local_variable_exists(string1, 0) == 2 || find_type_local_var(string1) != 0));

}

unsigned int find_type_local_var(char *string1) {

    Node_local_variable *temp = head_local_variable;
    while (temp != NULL) {
        if (strcmp(temp->name, string1) == 0 && function_identity == temp->location) {
            return temp->type;
        }
        temp = temp->next;
    }    

    return -1;
}

unsigned int find_type_func(char *string1) {

    Node_function *temp = head_function;
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

void multi_var_handler(char *string1) {

    int temp_type = -1;
    
    switch(string1[3]){ 
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
    
        int i = 5;
        int start = i;
        
        for (; i < strlen(string1); i++) {
            if (string1[i] == ' ' || i == strlen(string1) - 1) {
                char *new_string = makeString(start, i, string1);
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
                start = i + 1;
            }
        } 
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
    
    char *new_string = makeString(5, i, string1);
    
    if (check_class(new_string)) {
    
    } else if (check_func(new_string)) {
      
    } else if (check_variable(new_string)) {
    
    } else if (check_array(new_string)) {
                 
    } else {
    
        insertNode_func(string1, temp_type, 1);
    }
     
    free(new_string);

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
        if (strcmp(temp->name, string1) == 0 && temp->location == function_identity) {
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

unsigned int find_func_param(char *string1) {

    Node_function *temp = head_function;
    while (temp != NULL) {
        if (strcmp(temp->name, string1) == 0) {
            return temp->has_parameters;
        }
        temp = temp->next;
    }

    return -1;

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
    newNode->location = function_identity;
    
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

Node_function *get_function_node(char *string1) {

    if (head_function == NULL) return NULL;
    Node_function *temp = head_function;
    while (temp != NULL) {
        if (strcmp(temp->name, string1) == 0) {
            return temp;
        }
        temp = temp->next;
    }
    
    return temp;
}

Node_local_variable *get_local_var_node(char *string1) {        //returns var in local scope

    Node_local_variable *temp = head_local_variable;
    while (temp != NULL) {
        if (strcmp(temp->name, string1) == 0 && temp->location == function_identity) {
            return temp;
        }
        temp = temp->next;
    }
    
    return temp;

}

Node_array *get_array_node(char *string1) {

    Node_array *temp = head_array;
    while (temp != NULL) {
        if (strcmp(temp->name, string1) == 0 && temp->location == function_identity) {
            return temp;
        }
        temp = temp->next;
    }
    
    return temp;
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
      
        free(temp_string);
        
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
                    free(temp_string);
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
    
    
    printf("Function list: \n");
    
    if (head_function != NULL) {  
        Node_function *temp2 = head_function;  
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
    printf("Local array list: \n");
    
    if (temp4 != NULL) {
        while (temp4->next != NULL) {
            printf("\e[32m%s\e[0m(%d)(%d)(%d)->", temp4->name, temp4->type, temp4->size_array, temp4->location);
            temp4 = temp4->next;
        }
        printf("\e[32m%s\e[0m(%d)(%d)(%d)\n", temp4->name, temp4->type, temp4->size_array, temp4->location);
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
    
    if (input == NULL || strlen(input) == 0) {
        return 0;
    }
       
    if (!isalpha(input[0]) && input[0] != '_') {
        return 0;
    } else {
        if (strlen(input) > 1) {
            for (int i = 1; i < strlen(input); i++) {
                if (!isdigit(input[i]) && !isalpha(input[i]) && input[i] != '_') {
                    return 0;
                }
            }   
        }                        
    }

    return return_value;
}


void free_all() {

/*
    Node_class *free_list = head_class;
    
    while (free_list != NULL) {
        Node_class *temp = free_list->next;
        free(free_list->name);
        free(free_list);
        free_list = NULL;
        free_list = temp;
    }
   
    free(head_class);
    
    Node_function *free_list1 = head_function;
    while (free_list1 != NULL) {
        Node_function *temp = free_list1->next;
        free(free_list1->name);
        free(free_list1);
        free_list = NULL;
        free_list1 = temp;
    }
      
    free(head_function);
  //  head_function = NULL;
    

    
    Node_variable *free_list2 = head_variable;

    while (free_list2 != NULL) {
        Node_variable *temp = free_list2->next;
        free(free_list2->name);
        free(free_list2);
        free_list2 = NULL;
        free_list2 = temp;
    }
    
    free(head_variable);
    head_variable = NULL;
    
    Node_array *free_list3 = head_array;

    while (free_list3 != NULL) {
        Node_array *temp = free_list3->next;
        free(free_list3);
        free_list3 = temp;
    }
    
    free(head_array);
      
    Node_local_variable *free_list4 = head_local_variable;
    
    while (free_list4 != NULL) {
        Node_local_variable *temp = free_list4->next;
        free(free_list4->name);
        free(free_list4);
        free_list4 = temp;
    }
*/
    
}






























