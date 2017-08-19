

#include "code_handle.h"
#include "op_funcs.h"







int
main (int argc, char **argv)
{
  FILE *ptr;


  ptr = fopen (argv[1], "rb");

  codeobj *obj = (codeobj *) malloc (sizeof (codeobj));
  fseek (ptr, 8, SEEK_CUR);	//skip magic num and timestamp
  fgetc (ptr);
  memcpy (obj, getcode (ptr), sizeof (codeobj));
  dataobj *globnames = (dataobj *) malloc (5 * sizeof (dataobj));

  obj->namind = (dataobj *) malloc (obj->ncount * sizeof (dataobj));
  obj->varind = (dataobj *) malloc (obj->varcount * sizeof (dataobj));
  execute (obj->code, obj->consts, obj->code_size, obj->varind, obj->namind, globnames);
  return 0;
}











dataobj *execute (int *instruction, dataobj * consts, int code_size, dataobj * varind, dataobj * namind, dataobj * globnames)
{


  int counter = 0;
  while (counter < code_size)
    {
      int c_op;
      int jt;

      switch (instruction[counter])
	{
	case LOAD_CONST:
	  load_constant (instruction, consts, counter);
	  counter += 3;
	  break;
	case BINARY_ADD:
	  binary_add ();
	  counter += 1;
	  break;
	case BINARY_SUBTRACT:
	  binary_sub ();
	  counter += 1;
	  break;
	case BINARY_MULTIPLY:
	  binary_mult ();
	  counter += 1;
	  break;
	case BINARY_DIVIDE:
	  binary_div ();
	  counter += 1;
	  break;
	case PRINT_ITEM:
	  print_instr ();
	  counter += 1;
	  break;
	case PRINT_NEWLINE:
	  print_newline ();
	  counter += 1;
	  break;
	case RETURN_VALUE:
	  return pop ();
	  counter += 1;
	  break;
	case LOAD_FAST:
	  load_fast (instruction, varind, counter);
	  counter += 3;
	  break;
	case CALL_FUNCTION:
	  call_func (instruction, counter, globnames);
	  counter += 3;
	  break;
	case MAKE_FUNCTION:
	  make_func (instruction, counter,globnames);
	  counter += 3;
	  break;
	case STORE_FAST:
	  store_fast (instruction, varind, counter);
	  counter += 3;
	  break;
	case POP_JUMP_IF_FALSE:
	  jt = pop_jump_if_false (instruction, counter);
	  if (jt == 0)
	    {
	      counter += 3;
	    }
	  else
	    counter = jt;
	  break;
	case BINARY_MODULO:
	  binary_mod ();
	  counter += 1;
	  break;
	case POP_TOP:
	  pop_top ();
	  counter += 1;
	  break;
	case JUMP_FORWARD:
	  jt = jump_fwd (instruction, counter);
	  counter = counter + jt + 3;
	  break;
	case SETUP_LOOP:
	  counter += 3;
	  break;
	case JUMP_ABSOLUTE:
	  counter = jump_abs (instruction, counter);
	  break;
	case POP_BLOCK:
	  counter += 1;
	  break;
	case COMPARE_OP:
	  c_op = 0;
	  c_op += instruction[counter + 1] | (instruction[counter + 2] << 8);
	  dataobj *pushitem = (dataobj *) malloc (sizeof (dataobj));
	  pushitem = comp_op (c_op);
	  push (pushitem);
	  counter += 3;
	  break;
	case STORE_NAME:
	  store_name (instruction, namind, counter, globnames);
	  counter += 3;
	  break;
	case LOAD_GLOBAL:
	  load_global (instruction, globnames, counter);
	  counter += 3;
	  break;
	case LOAD_NAME:
	  load_name (instruction, namind, counter);
	  counter += 3;
	  break;
	default:
	  printf ("\nunknown instr%x", instruction[counter]);
	  break;
	}



    }

  return 0;


}
