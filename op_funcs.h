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
