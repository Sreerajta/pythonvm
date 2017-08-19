#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<time.h>


#define LOAD_CONST 0x64
#define POP_TOP 0X1
#define BINARY_MULTIPLY 0X14
#define BINARY_DIVIDE 0x1b
#define BINARY_MODULO 0X16
#define BINARY_ADD 0x17
#define BINARY_SUBTRACT 0x18
#define PRINT_ITEM 0x47
#define PRINT_NEWLINE 0x48
#define POP_BLOCK 0x57 
#define STORE_NAME 0X5a
#define RETURN_VALUE 0x53

#define LOAD_NAME 0x65  
#define COMPARE_OP 0x6b
#define JUMP_FORWARD 0x6e
#define JUMP_ABSOLUTE 0x71 
#define POP_JUMP_IF_FALSE 0x72 
#define LOAD_GLOBAL 0x74
#define SETUP_LOOP 0x78
#define LOAD_FAST 0x7c
#define STORE_FAST 0x7d
#define CALL_FUNCTION 0x83
#define MAKE_FUNCTION 0x84
#define S_SIZE 100
#define MAXSIZE 1024

typedef struct dataobj dataobj;
typedef struct codeobj codeobj;
char *strref[S_SIZE]; 
int strref_count=0;
int globcount=0;
int gflag=0;

void print_instr();
dataobj *getconsts(FILE *ptr,int size);
dataobj *execute(int *instruction, dataobj *consts, int code_size,dataobj *varind,dataobj *namind,dataobj *globnames);



struct dataobj {
    enum { is_int, is_code, is_string,is_null,is_none,is_false,is_true } type;
    union {
        int ival;
        codeobj *codedat;
        char *cval;
    } val;
    int size;
};


struct codeobj{
    
    int *code;
    int code_size;
    
    int argcount;
    
    dataobj *consts;
    int nconst;
    
    dataobj *names;
    int ncount;
    dataobj *namind;
    
    dataobj *varnames;
    int varcount;
    dataobj *varind;
    
    char *fnname;
    
    char *filename;
    
    dataobj *skipobj;
    
    
    
};


dataobj *stack[MAXSIZE]; 
int sp=0;

void push(dataobj *obj){
    if(sp<MAXSIZE)
    {    
        stack[sp++]=obj;
        
    }
    else{
        printf("stack full");
    }
    
    
}


dataobj *pop(){
    
    if(sp>0){
        return stack[--sp];
    }
    else{
        printf("stack empty");
    }
}

dataobj *getfun(int loc)
{ 
    return stack[sp-loc-1];}
