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
    
    
};




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
  int x=fgetc(ptr); //skip 28
  int ntemp=0;
  ntemp+= fgetc(ptr) | (fgetc(ptr) << 8) | (fgetc(ptr) << 16) | (fgetc(ptr) << 24);
  tempobj->nconst=ntemp;
  tempobj->consts=getconsts(ptr,ntemp);
  //printf("%d",tempobj->consts[0].val.ival); //test
  
  //geting names:
  x=fgetc(ptr);//skip 28
  int nametmp=0;
  nametmp+= fgetc(ptr) | (fgetc(ptr) << 8) | (fgetc(ptr) << 16) | (fgetc(ptr) << 24);
  tempobj->ncount=nametmp;
  tempobj->names=getconsts(ptr,nametmp);
  
  
  
  
  
  return tempobj;
    
    
}


dataobj *getconsts(FILE *ptr,int size){

  dataobj *retobj=(dataobj*) malloc(size*sizeof(dataobj));
  for(int j=0;j<size;j++)
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
        case 0x74:
           // printf("\nstring in get const\n"); //test
            retobj[j].type=is_string;
            int tempsize=0;
            tempsize+= fgetc(ptr) | (fgetc(ptr) << 8) | (fgetc(ptr) << 16) | (fgetc(ptr) << 24);
            retobj[j].size=tempsize;
            //printf("\nsize:%d\n",tempsize); //test
            char tempstr[100];
            fread(tempstr, tempsize, 1, ptr);
            tempstr[tempsize]='\0';
            retobj[j].val.cval=tempstr;
          //  puts(retobj[j].val.cval); //test
            if(check==0x74){
                strref[strref_count]=tempstr;
                printf("%s\n",strref[strref_count]);
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
            printf("%d",loc);
            printf("%s",strref[loc]);
            retobj[j].val.cval=strref[loc];
         break;
        case 0x63:
            printf("\ncode obj encountered\n");
        default:
            break;
        
    } 
        
    
      
}
    return retobj;
  }  



  void execute(int *instruction, dataobj *consts, int code_size)
    {
        
      int counter=0;  
      for(int i=0;i<code_size;i++)
      {
        switch(instruction[counter])
        {
            case 0x64:
                printf("\nload constant");
                break;
            case 0x17:
                printf("\nbinary add");
                break;
            case 0x47:
                printf("\nprint instr");
                break;
            case 0x48:
                printf("\nprint newline");
            case 0x53:
                printf("\nreturn value");
                break;
            
        }
          if(instruction[counter]>90)
              counter+=3;
          else
              counter+=1;
          
    }
        
        
        
        
    }
  
  
  
  
  
  
  
  
  
  
  
  
  int main()
{   FILE *ptr;
    ptr=fopen("test3.pyc","rb");
    codeobj *obj=(codeobj *) malloc(sizeof(codeobj));
     fseek ( ptr , 8 , SEEK_CUR );//skip magic num and timestamp
    int n=fgetc(ptr);// skip 63
   
    obj=getcode(ptr);
    printf("\nmain function:\n");
    for(int i=0;i<obj->code_size;i++)
  {   
      printf("%02x ", obj->code[i]);
  }
    
    execute(obj->code,obj->consts,obj->code_size);
    return 0;
}

