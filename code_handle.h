#include "init.h"
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
   
