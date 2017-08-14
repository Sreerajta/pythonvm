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
    enum { is_int, is_code, is_string,is_null,is_none } type;
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
    dataobj *varnames;
    int varcount;
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
  //printf("code size:%d\n",tempobj->code_size);//test
  tempobj->code=(int*)malloc(tempobj->code_size*sizeof(int));
  for(int i=0;i<tempobj->code_size;i++)
  {   tempobj->code[i]=fgetc(ptr);
      //printf("%x ", tempobj->code[i]);//test
  }
  
  //getting consts;
  printf("\ngetting consts\n"); //test
  fgetc(ptr); //skip 28
  
  int ntemp=0;
  ntemp+= fgetc(ptr) | (fgetc(ptr) << 8) | (fgetc(ptr) << 16) | (fgetc(ptr) << 24);
  tempobj->nconst=ntemp;
  tempobj->consts=(dataobj*) malloc(ntemp*sizeof(dataobj));
  memcpy(tempobj->consts,getconsts(ptr,ntemp),ntemp*sizeof(dataobj));
  free(retobj);
  //tempobj->consts=getconsts(ptr,ntemp);
  
  for(int s=0;s<ntemp;s++){
  printf("test %s",tempobj->consts[s].val.cval); //test
  }
  //geting names:
   printf("\ngetting names\n"); //test
   fgetc(ptr);//skip 28
  //printf("const%x",x);
  
  int nametmp=0;
  nametmp+= fgetc(ptr) | (fgetc(ptr) << 8) | (fgetc(ptr) << 16) | (fgetc(ptr) << 24);
  tempobj->ncount=nametmp;

  tempobj->names=(dataobj*) malloc(nametmp*sizeof(dataobj));
  memcpy(tempobj->names,getconsts(ptr,nametmp),nametmp*sizeof(dataobj));
  
  
  //test 
  for(int s=0;s<ntemp;s++){
  printf("test_in_names %s",tempobj->consts[s].val.cval); //test
  }

  
  //geting varnames:
  fgetc(ptr);//skip 28
  //printf("%x:xval",x);
  int vartmp=0;
  vartmp+= fgetc(ptr) | (fgetc(ptr) << 8) | (fgetc(ptr) << 16) | (fgetc(ptr) << 24);
  tempobj->varcount=vartmp;
  //printf("%ddoo",vartmp);
  tempobj->varnames=getconsts(ptr,vartmp);
  
  
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
    //printf("\nfx is:%x",fx);//test
    int flen=0;
    flen+= fgetc(ptr) | (fgetc(ptr) << 8) | (fgetc(ptr) << 16) | (fgetc(ptr) << 24);
    char *buffer=(char*) malloc(flen*sizeof(char));
    fread(buffer,flen,1,ptr);
    tempobj->filename=buffer;
   // puts(tempobj->filename); // test
    
  //geting fn name
    fgetc(ptr);
    //printf("\nfx is:%x",fx);//test
    int fnlen=0;
    fnlen+= fgetc(ptr) | (fgetc(ptr) << 8) | (fgetc(ptr) << 16) | (fgetc(ptr) << 24);
    char *bufferf=(char*) malloc(fnlen*sizeof(char));
    fread(bufferf,fnlen,1,ptr);
    tempobj->fnname=bufferf;
    //puts(tempobj->fnname);
    
  //skip first l no:
    fgetc(ptr);
    
     fgetc(ptr);
      
     fgetc(ptr);
     fgetc(ptr);
     
  //skip lnotab
     fgetc(ptr);
    //printf("\nfx is:%x",fx);//test
    int skiptemp=0;
    skiptemp+= fgetc(ptr) | (fgetc(ptr) << 8) | (fgetc(ptr) << 16) | (fgetc(ptr) << 24);
    char *bufferl=(char*) malloc(skiptemp*sizeof(char));
    fread(buffer,skiptemp,1,ptr);
     
  fclose(ptr);   
     
     
  return tempobj;
    
    
}

int xcount=0;
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
        case 0x69:
            retobj[j].type=is_int;
            int tempint=0;
            tempint= fgetc(ptr) | (fgetc(ptr) << 8) | (fgetc(ptr) << 16) | (fgetc(ptr) << 24);
            retobj[j].val.ival = tempint;
            break;
        case 0x73:
            break;
        case 0x74:
            printf("\nstring in get const\n"); //test
            retobj[j].type=is_string;
            int tempsize=0;
            tempsize+= fgetc(ptr) | (fgetc(ptr) << 8) | (fgetc(ptr) << 16) | (fgetc(ptr) << 24);
            retobj[j].size=tempsize;
            //printf("\nsize:%d\n",tempsize); //test
            char tempstr[100];
            fread(tempstr, tempsize, 1, ptr);
            tempstr[tempsize]='\0';
            retobj[j].val.cval=(char*) malloc(tempsize*sizeof(char));
            memcpy(retobj[j].val.cval,&tempstr,tempsize);
            retobj[j].val.cval=tempstr;
          //  puts(retobj[j].val.cval); //test
            if(check==0x74){
                strref[strref_count]=tempstr;
               // printf("%s\n",strref[strref_count]);
                strref_count+=1;
                
                //printf("\nstored ref at%d",strref_count-1);
            
            }
           //printf("\ninterned string: %s",strref[strref_count]); //test
            break;
        case 0x52:  
            printf("\nencountered string ref\n");//test
            retobj[j].type=is_string;
            int loc=0;
            loc+= fgetc(ptr) | (fgetc(ptr) << 8) | (fgetc(ptr) << 16) | (fgetc(ptr) << 24);
           // printf("%d",loc);
            //printf("%s",strref[loc]);
            retobj[j].val.cval=strref[loc];
         break;
        case 0x63:
            printf("\ncode obj encountered\n");
            retobj[j].type=is_code;
            retobj[j].val.codedat=getcode(ptr);
            //printf("\nfunctincode:%x\n",retobj[j].val.codedat->code[6]);//test
            break;
        case 0x28:
            printf("it hpns");
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
    result->val.ival=first->val.ival+second->val.ival; 
    push(result);
}

void load_constant(int *instruction,dataobj *consts,int counter){
    int index=0;
    index+=instruction[counter+1] | (instruction[counter+2] << 8) ;
    printf("index:%d",index);
    dataobj *pushitem=(dataobj*) malloc(sizeof(dataobj));
    pushitem=&consts[index];
    printf("\n%s",consts[index].val.cval);
    push(pushitem);
    
   
}

void print_instr(){
    dataobj *item=pop();
    if(item->type==is_int)
    {
        printf("%d",item->val.ival);
    }
    else if(item->type==is_string){
        for(int j=0;j<item->size;j++)
        {printf("%s",item->val.cval);}
    }
}


void print_newline(){
    printf("\n");
    
}



  
  
  
  
  
  
  
  
  //====================================================================
  
  
  
  
  

  void execute(int *instruction, dataobj *consts, int code_size)
    {
        
      int counter=0;  
      while(counter<code_size)
      {
        switch(instruction[counter])
        {
            case 0x64:
                printf("\nload constant%x",instruction[counter]);
                load_constant(instruction,consts,counter);
            
                break;
            case 0x17:
                printf("\nbinary add%x",instruction[counter]);
                binary_add();
                break;
            case 0x47:
                printf("\nprint instr%x",instruction[counter]);
                print_instr();
                break;
            case 0x48:
                printf("\nprint newline%x",instruction[counter]);
                print_newline();
                break;
            case 0x53:
                printf("\nreturn value%x",instruction[counter]);
                break;
            case 0x7c:
                printf("\nloadfast%x",instruction[counter]);
                break;
            case 0x83:
                printf("\ncall function%x",instruction[counter]);
                break;
            case 0x84:
                printf("\nmake function%x",instruction[counter]);
                break;
            case 0x7d:
                printf("\nstore fast%x",instruction[counter]);
                break;
            case 0x72:
                printf("\npop jump if false%x",instruction[counter]);
                break;
            case 0x1:
                printf("\npop top%x",instruction[counter]);
                break;
            case 0x6e:
                printf("\njump forward%x",instruction[counter]);
                break;
            case 0x6b:
                printf("\ncompare op%x",instruction[counter]);
                break;
            case 0x5a:
                printf("\nstore name%x",instruction[counter]);
                break;
            case 0x65:
                printf("\nload name%x",instruction[counter]);
                break;
            default:
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
    ptr=fopen("addtest.pyc","rb");
    codeobj *obj=(codeobj *) malloc(sizeof(codeobj));
     fseek ( ptr , 8 , SEEK_CUR );//skip magic num and timestamp
    int n=fgetc(ptr);// skip 63
   
    memcpy(obj,getcode(ptr),sizeof(codeobj));
    printf("\nmain function:\n");
    for(int i=0;i<obj->code_size;i++)
  {   
      printf("%02x ", obj->code[i]);
  }
  for(int i=0;i<obj->nconst;i++)
  {
printf("test %s",obj->consts[i].val.cval); 
      
}
  
  
  //dataobj *num1;
  //dataobj *num2;
 // num1=(dataobj*) malloc(sizeof(dataobj));
 // num2=(dataobj*) malloc(sizeof(dataobj));
 //num1->val.ival=5;
// printf("%d",num1->val.ival);
//num2->val.ival=7;
  //push(num1);
  //push(num2);
  //binary_add();
  
    //printf("%s",obj->consts[2].val.cval);
   execute(obj->code,obj->consts,obj->code_size);
   return 0;
}

