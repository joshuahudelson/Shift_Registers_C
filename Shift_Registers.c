#include <stdio.h>
#include <string.h>
#define int_size 31;


/*
Next steps:
- put gates in an array

*/




int get_bit(int, int);
int operate(int, int, char);
void print_bin(int);

int main(){

struct Link{
  int * element;
  int tap;
};

struct Gate{
  struct Link in1;
  struct Link in2;
  char type;
  struct Link out;
};

  int Reg1 = 4;

  struct Link link1;
  link1.element = &Reg1;
  link1.tap = 0;

  struct Link link2;
  link2.element = &Reg1;
  link2.tap = 1;

  struct Link link3;
  link3.element = &Reg1;
  link3.tap = 30;

  struct Gate Gate1;
  Gate1.in1 = link1;
  Gate1.in2 = link2;
  Gate1.type = 'X';
  Gate1.out = link3;

int get_bit(int reg, int tap){
  return (reg>>tap & 1);
}

int operate(int left, int right, char type){
  if(type == 'X'){
    return (left ^ right);
  }
  else{
    return 0;
  }
}

void compute_gate(struct Gate a_gate){
  int x = get_bit(*a_gate.in1.element, a_gate.in1.tap);
  int y = get_bit(*a_gate.in2.element, a_gate.in2.tap);
  int z = operate(x, y, a_gate.type);
  z = z<<a_gate.out.tap;
  * a_gate.out.element = (*a_gate.out.element | z); //maybe this shouldn't be an OR gate...just equals?
                                                    //so right-most inlet is always dominant?
}

void print_bin(int reg){
  for(int i=31; i>0; i--){
    int temp = reg>>i;
    int temp2 = temp & 1;
    printf("%i", temp2);
  }
  printf("\n");
}

void print_gate_state(struct Gate a_gate){
  printf("in1 element: %p\n", a_gate.in1.element);
  printf("in1 *element: %i\n", *a_gate.in1.element);
  printf("in1 tap: %i\n", a_gate.in1.tap);
  printf("in2 element: %p\n", a_gate.in2.element);
  printf("in2 *element: %i\n", *a_gate.in2.element);
  printf("in2 tap: %i\n", a_gate.in2.tap);
  printf("type: %c\n", a_gate.type);
  printf("out element: %p\n", a_gate.out.element);
  printf("out *element: %i\n", *a_gate.out.element);
  printf("out tap: %i\n", a_gate.out.tap);
}

for (int i=0;i<1000;i++){
  print_bin(Reg1);
  //printf("%i\n",Reg1);
  Reg1 = Reg1>>1;
  compute_gate(Gate1);
}
}
