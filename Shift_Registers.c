#include <stdio.h>
#include <string.h>

int main(){

struct Register{
  int state[4];
  int * inlet;
  int inlet_index;
};

struct Gate{
  int * tap1;
  int * tap2;
  char type;
  int out;
};

struct Register Reg1;
int * temp1 = Reg1.state;
*temp1++ = 1; *temp1++ = 0; *temp1++ = 1; *temp1++ = 0;

struct Gate Gate1;

Gate1.tap1 = &Reg1.state[3];
Gate1.tap2 = &Reg1.state[2];
Gate1.type = 'X';
Reg1.inlet = &Gate1.out;

printf("%i", *Reg1.state);

// Sequence:
//  Set all inlets to zero
//  Go through all gates and compute outs.
//  Shift all registers.
//  Convert registers to ints/


}
