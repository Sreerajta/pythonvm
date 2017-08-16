#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<time.h>


typedef struct dataobj dataobj;
typedef struct codeobj codeobj;
char *strref[100]; // to store interned strings
int strref_count=0;

dataobj *getconsts(FILE *ptr,int size);
dataobj *retobj;


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

//defining the stack:
dataobj *stack[100]; // stack has to store multiple type of dataobj
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

//--------------------------------------------------------------------------



    





codeobj *getcode(FILE *ptr)
{
 //dev reminder: called after getting a 63, so no skip
  
  int temp=0;
  codeobj *tempobj;
  tempobj = (codeobj *) malloc(sizeof(codeobj));
  fseek ( ptr , 16 , SEEK_CUR );
  int n= fgetc(ptr);//skip 73
  temp+= fgetc(ptr) | (fgetc(ptr) << 8) | (fgetc(ptr) << 16) | (fgetc(ptr) << 24);
  tempobj->code_size=temp;
  tempobj->code=(int*)malloc(tempobj->code_size*sizeof(int));
  for(int i=0;i<tempobj->code_size;i++)
  {   tempobj->code[i]=fgetc(ptr);
      
  }
  
  //getting consts;
  fgetc(ptr); //skip 28
  int ntemp=0;
  ntemp+= fgetc(ptr) | (fgetc(ptr) << 8) | (fgetc(ptr) << 16) | (fgetc(ptr) << 24);
  tempobj->nconst=ntemp;
  tempobj->consts=(dataobj*) malloc(ntemp*sizeof(dataobj));
  memcpy(tempobj->consts,getconsts(ptr,ntemp),ntemp*sizeof(dataobj));
  free(retobj);


  //geting names:
   fgetc(ptr);//skip 28
   int nametmp=0;
   nametmp+= fgetc(ptr) | (fgetc(ptr) << 8) | (fgetc(ptr) << 16) | (fgetc(ptr) << 24);
   tempobj->ncount=nametmp;
   tempobj->names=(dataobj*) malloc(nametmp*sizeof(dataobj));
   memcpy(tempobj->names,getconsts(ptr,nametmp),nametmp*sizeof(dataobj));
   free(retobj);

  
  //geting varnames:
  fgetc(ptr);//skip 28
  int vartmp=0;
  vartmp+= fgetc(ptr) | (fgetc(ptr) << 8) | (fgetc(ptr) << 16) | (fgetc(ptr) << 24);
  tempobj->varcount=vartmp;
  tempobj->varnames=(dataobj*) malloc(vartmp*sizeof(dataobj));
  memcpy(tempobj->varnames,getconsts(ptr,vartmp),vartmp*sizeof(dataobj));
  free(retobj);
  
  //getting freevars skip
   fgetc(ptr);//skip 28
   int tmp=0;
   tmp+= fgetc(ptr) | (fgetc(ptr) << 8) | (fgetc(ptr) << 16) | (fgetc(ptr) << 24);
   tempobj->skipobj=getconsts(ptr,tmp);
  
  //getting cellvars skip
    fgetc(ptr);//skip 28
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
   // puts(tempobj->filename); // test
    
  //geting fn name
    fgetc(ptr);
    int fnlen=0;
    fnlen+= fgetc(ptr) | (fgetc(ptr) << 8) | (fgetc(ptr) << 16) | (fgetc(ptr) << 24);
    char *bufferf=(char*) malloc(fnlen*sizeof(char));
    fread(bufferf,fnlen,1,ptr);
    char *fin_fnname=(char*) malloc(fnlen*sizeof(char));
    strcpy(fin_fnname,bufferf);
    tempobj->fnname=fin_fnname;
    
    
  //skip first l no:
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
     
  fclose(ptr);   
     
     
  return tempobj;
    
    
}


dataobj *getconsts(FILE *ptr,int sz){
  retobj=(dataobj*) malloc(sz*sizeof(dataobj));
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
            //printf("it hpns"); 
            break;
        default:
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

void load_constant(int *instruction,dataobj *consts,int counter){
    int index=0;
    index+=instruction[counter+1] | (instruction[counter+2] << 8) ;
    dataobj *pushitem=(dataobj*) malloc(sizeof(dataobj));
    pushitem=&consts[index];
    push(pushitem);
    
   
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
}


void print_newline(){
    printf("\n");
    
}

void load_name(int *instruction,dataobj *nameind,int counter){
    int index=0;
    index+=instruction[counter+1] | (instruction[counter+2] << 8) ;
    dataobj *pushitem=(dataobj*) malloc(sizeof(dataobj));
    pushitem=&nameind[index];
    push(pushitem);
    
}

void store_name(int *instruction,dataobj *namind,int counter){
    int index=0;
    index+=instruction[counter+1] | (instruction[counter+2] << 8) ;
    dataobj *tempstoren=(dataobj*)malloc(sizeof(dataobj));
    tempstoren=pop();
    namind[index].val.ival=tempstoren->val.ival;
    
    
}

void load_fast(int *instruction,dataobj *varind,int counter){

    int index=0;
    index+=instruction[counter+1] | (instruction[counter+2] << 8) ;
    dataobj *pushitem=(dataobj*) malloc(sizeof(dataobj));
    pushitem=&varind[index];
    push(pushitem);
    
}


void store_fast(int *instruction,dataobj *varind,int counter){
    int index=0;
    index+=instruction[counter+1] | (instruction[counter+2] << 8) ;
    varind[index]=*pop();
    
    
}

void pop_jump_if_false(int *instruction,int counter){
 if(pop()->type==is_false){
    int index=0;
    index+=instruction[counter+1] | (instruction[counter+2] << 8) ;
    counter=index;
     
 }
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

  
  
void jump_fwd(int *instruction,int counter)
{
    
}
  
  
  
  
  //====================================================================
  
  
  
  
  

  void execute(int *instruction, dataobj *consts, int code_size,dataobj *varind,dataobj *namind)
    {
        
      int counter=0;  
      while(counter<code_size)
      { int c_op;
        switch(instruction[counter])
        {
            case 0x64:
                //printf("\nload constant%x",instruction[counter]);
                load_constant(instruction,consts,counter);
            
                break;
            case 0x17:
               // printf("\nbinary add%x",instruction[counter]);
                binary_add();
                break;
            case 0x47:
                //printf("\nprint instr%x",instruction[counter]);
                print_instr();
                break;
            case 0x48:
                //printf("\nprint newline%x",instruction[counter]);
                print_newline();
                break;
            case 0x53:
                //printf("\nreturn value%x",instruction[counter]);
                //return
                break;
            case 0x7c:
                //printf("\nloadfast%x",instruction[counter]);
                load_fast(instruction,varind,counter);
                break;
            case 0x83:
                //printf("\ncall function%x",instruction[counter]);
                break;
            case 0x84:
                //printf("\nmake function%x",instruction[counter]);
                break;
            case 0x7d:
                //printf("\nstore fast%x",instruction[counter]);
                store_fast(instruction,varind,counter);
                break;
            case 0x72:
                //printf("\npop jump if false%x",instruction[counter]);
                pop_jump_if_false(instruction,counter);
                break;
            case 0x1:
               // printf("\npop top%x",instruction[counter]);
                pop_top();
                break;
            case 0x6e:
                //printf("\njump forward%x",instruction[counter]);
                break;
            case 0x6b:
                c_op=0;
                c_op+=instruction[counter+1] | (instruction[counter+2] << 8) ;
                dataobj *pushitem=(dataobj*) malloc(sizeof(dataobj));
                pushitem=comp_op(c_op);
                push(pushitem);
                break;
            case 0x5a:
                //printf("\nstore name%x",instruction[counter]);
                 store_name(instruction,namind,counter);
                break;
            case 0x65:
                //printf("\nload name%x",instruction[counter]);
                load_name(instruction,namind,counter);
                break;
            default:
                //printf("\nunknown instr%x",instruction[counter]);
                break;
        }
          if(instruction[counter]>=90)
              counter+=3;
          else
              counter+=1;
          
    }
        
        
        
        
    }
  
  
  
  
  
  
  
  
  
  
  
  
  int main()
{   FILE *ptr;
    ptr=fopen("comptest.pyc","rb");
    codeobj *obj=(codeobj *) malloc(sizeof(codeobj));
     fseek ( ptr , 8 , SEEK_CUR );//skip magic num and timestamp
    int n=fgetc(ptr);// skip 63
    memcpy(obj,getcode(ptr),sizeof(codeobj));
    //printf("\nmain function:\n");
 /*   for(int i=0;i<obj->code_size;i++)
  {   
      printf("%02x ", obj->code[i]);
  }
  for(int i=0;i<obj->nconst;i++)
  {
printf("test %s",obj->consts[i].val.cval); 
      
}*/
  obj->namind=(dataobj*) malloc(obj->ncount*sizeof(dataobj));
  obj->varind=(dataobj*) malloc(obj->varcount*sizeof(dataobj));
   execute(obj->code,obj->consts,obj->code_size,obj->varind,obj->namind);
   return 0;
}

