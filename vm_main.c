#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<time.h>

int gflag=0;
typedef struct dataobj dataobj;
typedef struct codeobj codeobj;
char *strref[100]; 
int strref_count=0;
int globcount=0;



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


dataobj *stack[100]; 
int sp=0;

void push(dataobj *obj){
    if(sp<100)
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

//--------------------------------------------------------------------------



    





codeobj *getcode(FILE *ptr)
{
    
 
  
  int temp=0;
  codeobj *tempobj;
  tempobj = (codeobj *) malloc(sizeof(codeobj));
  int tempargc=0;
  tempargc+=fgetc(ptr) | (fgetc(ptr) << 8) | (fgetc(ptr) << 16) | (fgetc(ptr) << 24);
  tempobj->argcount=tempargc;
  fseek ( ptr , 12 , SEEK_CUR );
  int n= fgetc(ptr);
  temp+= fgetc(ptr) | (fgetc(ptr) << 8) | (fgetc(ptr) << 16) | (fgetc(ptr) << 24);
  tempobj->code_size=temp;
  tempobj->code=(int*)malloc(tempobj->code_size*sizeof(int));
  for(int i=0;i<tempobj->code_size;i++)
  {   tempobj->code[i]=fgetc(ptr);
  }
  
  
  
  //getting consts
  fgetc(ptr); 
  int ntemp=0;
  ntemp+= fgetc(ptr) | (fgetc(ptr) << 8) | (fgetc(ptr) << 16) | (fgetc(ptr) << 24);
  tempobj->nconst=ntemp;
  
  tempobj->consts=(dataobj*) malloc(ntemp*sizeof(dataobj));
  memcpy(tempobj->consts,getconsts(ptr,ntemp),ntemp*sizeof(dataobj));



  //geting names:
   fgetc(ptr);
   int nametmp=0;
   nametmp+= fgetc(ptr) | (fgetc(ptr) << 8) | (fgetc(ptr) << 16) | (fgetc(ptr) << 24);
   tempobj->ncount=nametmp;
   tempobj->names=(dataobj*) malloc(nametmp*sizeof(dataobj));
   memcpy(tempobj->names,getconsts(ptr,nametmp),nametmp*sizeof(dataobj));
   //free(retobj);

  
  //geting varnames:
  fgetc(ptr);
  int vartmp=0;
  vartmp+= fgetc(ptr) | (fgetc(ptr) << 8) | (fgetc(ptr) << 16) | (fgetc(ptr) << 24);
  tempobj->varcount=vartmp;
  tempobj->varnames=(dataobj*) malloc(vartmp*sizeof(dataobj));
  memcpy(tempobj->varnames,getconsts(ptr,vartmp),vartmp*sizeof(dataobj));
  
  
  //skipping freevars
   fgetc(ptr);
   int tmp=0;
   tmp+= fgetc(ptr) | (fgetc(ptr) << 8) | (fgetc(ptr) << 16) | (fgetc(ptr) << 24);
   tempobj->skipobj=getconsts(ptr,tmp);
  
  //skipping cellvars
    fgetc(ptr);
    tmp=0;
    tmp+= fgetc(ptr) | (fgetc(ptr) << 8) | (fgetc(ptr) << 16) | (fgetc(ptr) << 24);
    tempobj->skipobj=getconsts(ptr,tmp);
  
  //getting file name
    fgetc(ptr);
    int flen=0;
    flen+= fgetc(ptr) | (fgetc(ptr) << 8) | (fgetc(ptr) << 16) | (fgetc(ptr) << 24);
    char *buffer=(char*) malloc(flen*sizeof(char));
    fread(buffer,flen,1,ptr);
    tempobj->filename=buffer;
    
    
  //geting fn name
    int x=fgetc(ptr);
    int fnlen=0;
    fnlen+= fgetc(ptr) | (fgetc(ptr) << 8) | (fgetc(ptr) << 16) | (fgetc(ptr) << 24);
    char *bufferf=(char*) malloc(fnlen*sizeof(char));
    fread(bufferf,fnlen,1,ptr);
    char *fin_fnname=(char*) malloc(fnlen*sizeof(char));
    strcpy(fin_fnname,bufferf);
    tempobj->fnname=fin_fnname;
    
    if(x == 0x74)
        strref[strref_count++] = tempobj->fnname;

    
    
  //skip first_line no:
    fgetc(ptr);
    
     fgetc(ptr);
      
     fgetc(ptr);
     fgetc(ptr);
     
  //skip lnotab
     fgetc(ptr);
    int skiptemp=0;
    skiptemp+= fgetc(ptr) | (fgetc(ptr) << 8) | (fgetc(ptr) << 16) | (fgetc(ptr) << 24);
    char *bufferl=(char*) malloc(skiptemp*sizeof(char));
    fread(buffer,skiptemp,1,ptr);
     

     
  return tempobj;
    
    
}


dataobj *getconsts(FILE *ptr,int sz){
  
    dataobj *retobj=(dataobj*) malloc(sz*sizeof(dataobj));

  for(int j=0;j<sz;j++)
  {  
   int check=fgetc(ptr);
   
    switch(check)
    {  
        case 0x4e:
            retobj[j].type=is_null;
            break;
        case 0x30:
            retobj[j].type=is_none;
            break;
        case 0x46:
            retobj[j].type=is_false;
            break;
        case 0x54:
            retobj[j].type=is_true;
            break;
        case 0x69:
            retobj[j].type=is_int;
            int tempint=0;
            tempint= fgetc(ptr) | (fgetc(ptr) << 8) | (fgetc(ptr) << 16) | (fgetc(ptr) << 24);
            retobj[j].val.ival = tempint;
            break;
        case 0x73:
        case 0x74:
            retobj[j].type=is_string;
            int tempsize=0;
            tempsize+= fgetc(ptr) | (fgetc(ptr) << 8) | (fgetc(ptr) << 16) | (fgetc(ptr) << 24);
            retobj[j].size=tempsize;
            char tempstr[100];
            fread(tempstr, tempsize, 1, ptr);
            tempstr[tempsize]='\0';
            char *fstr=(char*) malloc(tempsize*sizeof(char));
            strcpy(fstr,tempstr);
            
            retobj[j].val.cval=fstr;
            if(check==0x74){
                strref[strref_count]=tempstr;
                strref_count+=1;
            
            }
            break;
        case 0x52:  
            retobj[j].type=is_string;
            int loc=0;
            loc+= fgetc(ptr) | (fgetc(ptr) << 8) | (fgetc(ptr) << 16) | (fgetc(ptr) << 24);
            retobj[j].val.cval=strref[loc];
         break;
        case 0x63:
            retobj[j].type=is_code;
            retobj[j].val.codedat=getcode(ptr);
            break;
        case 0x28:
            break;
        default:
            printf("%d",check);
            break;
        
    } 
        
    
      
}
return retobj;
    

}
   
  


  //==================================================================== operations:
  
  void pop_top(){
    pop();
}

void binary_add(){
    dataobj *first=pop();
    dataobj *second=pop();
    dataobj *result;
    result=(dataobj*) malloc(sizeof(dataobj));
    result->type=is_int;
    result->val.ival=first->val.ival+second->val.ival; 
    push(result);
}

void binary_sub(){
    dataobj *first=pop();
    
    dataobj *second=pop();
    dataobj *result2;
    result2=(dataobj*) malloc(sizeof(dataobj));
    result2->type=is_int;
    result2->val.ival=second->val.ival-first->val.ival; 
    push(result2);
}


void binary_mult(){
    dataobj *first=pop();
    dataobj *second=pop();
    dataobj *result;
    result=(dataobj*) malloc(sizeof(dataobj));
    result->type=is_int;
    result->val.ival=first->val.ival*second->val.ival; 
    push(result);
}



void binary_div(){
    dataobj *first=pop();
    dataobj *second=pop();
    dataobj *result;
    result=(dataobj*) malloc(sizeof(dataobj));
    result->type=is_int;
    result->val.ival=second->val.ival/first->val.ival; 
    push(result);
}
void binary_mod(){
    dataobj *first=pop();
    dataobj *second=pop();
    dataobj *result;
    result=(dataobj*) malloc(sizeof(dataobj));
    result->type=is_int;
    result->val.ival=second->val.ival%first->val.ival; 
    push(result);
    
    
}







void load_constant(int *instruction,dataobj *consts,int counter){
    int index=0;
    index+=instruction[counter+1] | (instruction[counter+2] << 8) ;
    dataobj *pushnitem=(dataobj*) malloc(sizeof(dataobj));
    pushnitem=&consts[index];
    push(pushnitem);
   
}

void print_instr(){
    dataobj *item=pop();
    if(item->type==is_int)
    {
        printf("%d",item->val.ival);
    }
    else if(item->type==is_string){
        printf("%s",item->val.cval);
    }
    else if(item->type==is_true){
        printf("True");
    }
    else if(item->type==is_false){
        printf("False");
    }
    else
    {
        printf("%d",item->val.ival);
    }
}


void print_newline(){
    printf("\n");
    
}

void load_name(int *instruction,dataobj *namind,int counter){
    int index=0;
    index+=instruction[counter+1] | (instruction[counter+2] << 8) ;
    dataobj *pushitem=(dataobj*) malloc(sizeof(dataobj));
    pushitem=&namind[index];
    push(pushitem);
}

void store_name(int *instruction,dataobj *namind,int counter,dataobj *globnames){
    int index=0;
    index+=instruction[counter+1] | (instruction[counter+2] << 8) ;
    dataobj *tempstoren=(dataobj*)malloc(sizeof(dataobj));
    tempstoren=pop();
    if(tempstoren->type==is_int){
     namind[index].type=is_int;
     namind[index].val.ival=tempstoren->val.ival;
     int *i1=(int*) malloc(sizeof(int));
      memcpy(i1,&tempstoren->val.ival,sizeof(int)); 
     if(gflag==0){
        globnames[globcount].type==is_int;
      globnames[globcount].val.ival=*i1;
      printf("glob:%d\n",globnames[globcount].val.ival);
      globcount+=1;
      gflag=1;      
      
        }
        
        
    }
    else if(tempstoren->type==is_string){
        namind[index].type=is_string;
        namind[index].val.cval=tempstoren->val.cval;}
    else if(tempstoren->type==is_code)
        {  
            namind[index].type=is_code;
            namind[index].val.codedat=tempstoren->val.codedat;}

    
}

void load_fast(int *instruction,dataobj *varind,int counter){

    int index=0;
    index+=instruction[counter+1] | (instruction[counter+2] << 8) ;
    dataobj *pushfitem=(dataobj*) malloc(sizeof(dataobj));
    pushfitem=&varind[index];
    push(pushfitem);
   
}


void store_fast(int *instruction,dataobj *varind,int counter){
    int index=0;
    index+=instruction[counter+1] | (instruction[counter+2] << 8) ;
     dataobj *tempstorev=(dataobj*)malloc(sizeof(dataobj));
    tempstorev=pop();
    if(tempstorev->type==is_int){
    varind[index].val.ival=tempstorev->val.ival;}
    else if(tempstorev->type==is_string){
        varind[index].val.cval=tempstorev->val.cval;}
    else if(tempstorev->type==is_code)
        {  
            varind[index].val.codedat=tempstorev->val.codedat;}
    
}

int pop_jump_if_false(int *instruction,int counter){
 if(pop()->type==is_false){
    int index=0;
    index+=instruction[counter+1] | (instruction[counter+2] << 8) ;
    return index;
    
     
 }
 else
     return 0;
}

dataobj *comp_op(int operand)
{
    dataobj *obj1,*obj2;
    int op1,op2;
    obj1=pop();
    obj2=pop();
    dataobj *tempdat = (dataobj *) malloc(sizeof(dataobj));
    op1=obj2->val.ival;
    op2=obj1->val.ival;
    switch(operand)
    {
        case 0:
            
            if(op1<op2)
            {
              tempdat->type=is_true;  
            }
            else
            {tempdat->type=is_false;}
            break;
        case 1:
            if(op1<=op2)
                {
              tempdat->type=is_true;  
            }
            else
            {tempdat->type=is_false;}
            break;
        case 2:
            if(op1==op2)
                {
              tempdat->type=is_true;  
            }
            else
            {tempdat->type=is_false;}
            break;
        case 3:
            if(op1!=op2)
                {
              tempdat->type=is_true;  
            }
            else
            {tempdat->type=is_false;}
            break;
        case 4:
            if(op1>op2)
                {
              tempdat->type=is_true;  
            }
            else
            {tempdat->type=is_false;}
            break;
        case 5:
            if(op1>=op2)
                {
              tempdat->type=is_true;  
            }
            else
            {tempdat->type=is_false;}
            break;
        default:
            break;
        
        
    }
    return tempdat;
}

  
  
int jump_fwd(int *instruction,int counter)
{
    int operand=0;
    operand+=instruction[counter+1] | (instruction[counter+2] << 8) ;
    return operand;
    
}

int jump_abs(int *instruction,int counter){
    int operand=0;
    operand+=instruction[counter+1] | (instruction[counter+2] << 8) ;
    return operand;
}


void make_func(int *instruction, int counter)
{

int operand=0;
operand+=instruction[counter+1] | (instruction[counter+2] << 8) ;
dataobj *fun;

fun=pop();

push(fun);

}






void call_func(int *instruction, int counter,dataobj *globnames)
{

int loc=0;
loc+=instruction[counter+1] | (instruction[counter+2] << 8) ;
codeobj *funcode=(codeobj*) malloc(sizeof(codeobj));
dataobj *fun;
fun=getfun(loc);

memcpy(funcode, fun->val.codedat, sizeof (codeobj));


funcode->varind=(dataobj*) malloc(funcode->argcount*sizeof(dataobj));

for(int i=funcode->argcount-1;i>=0;i--){

memcpy(&funcode->varind[i],pop(),sizeof(dataobj));

}


dataobj *tempres=(dataobj*) malloc(sizeof(dataobj));
pop();
tempres=execute(funcode->code,funcode->consts,funcode->code_size,funcode->varind,funcode->namind,globnames);

push(tempres);





}
  
void load_global(int *instruction,dataobj *globnames,int counter){
   
   // not working as expected when recursion comes
    int index=0;
    index+=instruction[counter+1] | (instruction[counter+2] << 8) ;
    dataobj *pushitemg=(dataobj*) malloc(sizeof(dataobj));
    pushitemg->val.ival=globnames[index].val.ival;
    pushitemg->type=is_int;
    push(pushitemg);
    
}
  
  
  //====================================================================
  
  
  
  
  

  dataobj *execute(int *instruction, dataobj *consts, int code_size,dataobj *varind,dataobj *namind,dataobj *globnames)
    {
       globnames=(dataobj*) malloc(5*sizeof(dataobj));
        int rglcount;
      int counter=0;  
      while(counter<code_size)
      { int c_op;
        int jt;
        int skipcount=0;
        switch(instruction[counter])
        {
            case 0x64:
                //printf("\nload constant%x",instruction[counter]);
                load_constant(instruction,consts,counter);
                counter+=3;
                break;
            case 0x17:
               // printf("\nbinary add%x",instruction[counter]);
                binary_add();
                counter+=1;
                break;
            case 0x18:
               // printf("\n subtract");
                binary_sub();
                counter+=1;
                break;
            case 0x14:
                binary_mult();
                counter+=1;
                break;
            case 0x1b:
                binary_div();
                counter+=1;
                break;
            case 0x47:
                //printf("\nprint instr%x",instruction[counter]);
                print_instr();
                counter+=1;
                break;
            case 0x48:
               // printf("\nprint newline%x",instruction[counter]);
                print_newline();
                counter+=1;
                break;
            case 0x53:
                //printf("\nreturn value%x",instruction[counter]);
                return pop();
                counter+=1;
                break;
            case 0x7c:
               // printf("\nloadfast%x",instruction[counter]);
                load_fast(instruction,varind,counter);
                counter+=3;
                break;
            case 0x83:
                //printf("\ncall function%x",instruction[counter]);
                call_func(instruction,counter,globnames);
                counter+=3;
                break;
            case 0x84:
                //printf("\nmake function%x",instruction[counter]);
                make_func(instruction,counter);
                counter+=3;
                break;
            case 0x7d:
                //printf("\nstore fast%x",instruction[counter]);
                store_fast(instruction,varind,counter);
                counter+=3;
                break;
            case 0x72:
                //printf("\npop jump if false%x",instruction[counter]);
                jt=pop_jump_if_false(instruction,counter);
                if(jt==0)
                {counter+=3;}
                else
                  counter=jt;
                break;
            case 0x16:
                binary_mod();
                counter+=1;
                break;
            case 0x1:
                //printf("\npop top%x",instruction[counter]);
                pop_top();
                counter+=1;
                break;
            case 0x6e:
                //printf("\njump forward%x",instruction[counter]);
                jt=jump_fwd(instruction,counter);
                
                counter=counter+jt+3;
                break;
            case 0x78:
                //printf("\nsetup loop");
                counter+=3;
                break;
            case 0x71:
                //printf("\njump abs");
                counter=jump_abs(instruction,counter);
                
                break;
            case 0x57:
                //printf("\npop-block");
                counter+=1;
                break;
            case 0x6b:
                //printf("\ncomparison op");
                c_op=0;
                c_op+=instruction[counter+1] | (instruction[counter+2] << 8) ;
                dataobj *pushitem=(dataobj*) malloc(sizeof(dataobj));
                pushitem=comp_op(c_op);
                push(pushitem);
                counter+=3;
                break;
            case 0x5a:
                // printf("\nstore name%x",instruction[counter]);
                 store_name(instruction,namind,counter,globnames);
                 counter+=3;
                 break;
            case 0x74:
                load_global(instruction,globnames,counter);
                counter+=3;
                 break;
            case 0x65:
               // printf("\nload name%x",instruction[counter]);
                load_name(instruction,namind,counter);
                counter+=3;
                break;
            default:
               printf("\nunknown instr%x",instruction[counter]);
               break;
        }
          
       
          
    }
        
        return 0;
        
        
    }
  
  
  
  
  
  
  
  
  
  
  
  
  int main()
{   FILE *ptr;
   
    
    ptr=fopen("t8.pyc","rb");
    
    codeobj *obj=(codeobj *) malloc(sizeof(codeobj));
     fseek ( ptr , 8 , SEEK_CUR );//skip magic num and timestamp
    int n=fgetc(ptr);
   memcpy(obj,getcode(ptr),sizeof(codeobj));
   dataobj *globnames;
   
   obj->namind=(dataobj*) malloc(obj->ncount*sizeof(dataobj));
   obj->varind=(dataobj*) malloc(obj->varcount*sizeof(dataobj));
   globcount=obj->ncount;
   execute(obj->code,obj->consts,obj->code_size,obj->varind,obj->namind,globnames);
   return 0;
}

