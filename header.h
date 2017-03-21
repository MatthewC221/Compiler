#ifndef SCANNER_H_
#define SCANNER_H_

//ALL SCANNER FUNCTIONS

#include <stdbool.h>

int line_number;

char *makeString(int start, int end, char *string);	//makes the string to check
void checkLine(char *input, int line);				//main scanner
int printRange(int start, int end, char *string); 	//prints the range
int checkNewLine(char *input); 						//check if there is a trailing new line

int specialChar(char behind, char infront);
void checkTokens(char *input);
extern int checkIntegers(char *input);
int checkHex(char *input); 							//checking for hexadecimals
int checkBracket(char test);						//checking for brackets/braces (){}
int checkCharLiteral(char *string);						//checks for charliteral
extern int checkIdentifier(char *string);
void pushRange(int start, int end, char *string);
void checkString(char *temp_string);
//FINISHED SCANNER

//PARSER FUNCTIONS within scanner

//PARSER STRUCTS

#define MAXSIZE 256 
#define MAXLINES 200

unsigned int order[MAXSIZE];                //for if parameters are within brackets
unsigned int semantic_line[MAXSIZE];          //throw errors for semantic

struct Stack {
    char stk[MAXLINES][MAXSIZE];
    int top;
};

struct Stack semantic_info;      //extra context, what parameters, name of identifiers, etc.
struct Stack complete;
struct Stack s;
struct Stack bracket_stack;     // for ( ) brackets, check if previous was identifier/int literal
                                // for [ ] brackets, check if identifier/int literal in between (arrays)

void checkLine_parse(char *input, int line);
//parser functions
void push(char *string1, char *string2, int end);
char *pop(void);
void display(void);
void display_current(void);
void pushLine(char *file);
void identifier_reduce();
void check_function(); //checks if the line is valid
void check_statement();
void pure_push(char *string1);
void pure_push_bracket(char *string1);
void check_bracket_stack();
void check_integer();
void array_check();
void push_complete(char *string1);  //push valid statements
void check_main();              //checks for main func
void function_call_check();
void parameter_reduce();
unsigned int if_integer(char *string1);
unsigned int if_operand(char *string1);
unsigned int check_loop();
void check_string(char *string1);

void copy(char *buf);
char get_next();

//semantic structs
typedef struct node_var {       //both global and non-global
    char *name;
    struct node_var *next;
    unsigned int line_number;
    bool value_set;
    int value;
    unsigned int type;
} Node_variable;

typedef struct node_array {       //both global and non-global
    char *name;
    struct node_array *next;
    unsigned int line_number;
    unsigned int size_array;
    unsigned int location;
    unsigned int *value_set;
    unsigned int *value;
    unsigned int type;
} Node_array;

typedef struct node_class {     //for classes
    char *name;
    struct node_class *next;
    unsigned int line_number;
} Node_class;

typedef struct node_function {
    char *name;
    struct node_function *next;
    unsigned int line_number;
    unsigned int type;
    unsigned int has_parameters;            //indicates if there are arguments to pass
    unsigned int *parameter_types;          //creates array of the types of parameters
} Node_function;

typedef struct node_local_var {
    char *name;
    struct node_local_var *next;
    unsigned int line_number;
    bool value_set;
    int value;
    unsigned int type;
    unsigned int location;
} Node_local_variable;


//semantic addons
void display_semantic ();
void pure_push_names(char *string1);
char *pop_semantic();
char *join_string(char *string1, char *string2); //pops 5 a into a-5
char *semantic_string ();

//pure semantic
void insert_type(char *string1);

void insertNode_class(char *string1);
void insertNode_var(char *string1, unsigned int type);
void insertNode_func(char *string1, unsigned int type, int if_parameter);
void insertNode_array(char *string1, unsigned int type_var, unsigned int size);
void insertNode_local_var(char *string1, unsigned int type);

Node_variable *makeNode_var(char *string1, unsigned int type);
Node_local_variable *makeNode_local_var(char *string1, unsigned int type_var);
Node_class *makeNode_class(char *string1);
Node_function *makeNode_function(char *string1, unsigned int type, int if_parameter);
Node_array *makeNode_array(char *string1, unsigned int type, unsigned int size);

Node_function *get_function_node(char *string1);
Node_local_variable *get_local_var_node(char *string1);
Node_array *get_array_node(char *string1);

void printList_all();
//void printList_function();
//void printList_class();

void semantic_handler();        //main handler function that reads from stack

void class_handler(char *string1);      //handles class
void function_handler(char *string1);       //handles func w/o param
void variable_handler(char *string1);       //handles variable init
void array_handler(char *string1);               //handles array init
void conditional_handler(char *string1);
void special_conditional_handler(char *string1);

void variable_set_handler(char *string1);                           //handles variable set
void array_set_handler(char *string1);   
void function_param_handler(char *string1);                           //handles array set
void multi_var_handler(char *string1);
void function_call_handler(char *string1, int param, int void_call);

unsigned int check_variable(char *string1);                         //checks if variable/class/func/array exists
unsigned int check_class(char *string1);
unsigned int check_func(char *string1);
unsigned int check_array(char *string1);
unsigned int check_local_variable(char *string1);

unsigned int check_IDE(char *input);                                //checks if string is integer/IDE
unsigned int check_INT(char *string);

unsigned int find_type_var(char *string1);                          //finds the type of the var
unsigned int find_type_local_var(char *string1);
unsigned int find_type_func(char *string1);
unsigned int find_func_param(char *string1);

unsigned int check_variable_exists(char *string, int make_true);    //checks if var exists, if does set it to init.
unsigned int check_array_exists(char *string1);
unsigned int check_local_variable_exists(char *string1, int make_true);     //checks if var exists in the local scope

unsigned int if_operand_lit(char a);                                //checks if char is operand
unsigned int valid_variable(char *string1);
unsigned int check_location(char *string1);

void free_all();



#endif
