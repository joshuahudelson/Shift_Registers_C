#include <stdio.h>
#include "shift_shaper.h"

// FUNCTIONS--------------------------------------------------------------------


unsigned int get_bit(unsigned int reg, int tap){
  return ((reg>>tap) & 1);
}

unsigned int operate(unsigned int left, unsigned int right, char type){
  if (type == 'X'){
    return (left ^ right);
  }
  else if (type == 'A'){
    return (left & right);
  }
  else if (type == 'O'){
    return (left | right);
  }
  else{
    return 0;
  }
}

void shift_reg(unsigned int * reg, int number){
  *reg = *reg>>number;
}

void print_gate_array(struct Gate * array_gates, int length){
  for (int i=0; i<length; i++){
    printf("%i", i);
  }
}

void create_gate(struct Gate * gate, //array of gates
                 int * counter,
                 unsigned int * element1,
                 int tap1,
                 unsigned int * element2,
                 int tap2,
                 char type){

  if (*counter >= MAX_NUM_GATES){
    printf("Maximum number of gates has been reached!");
  }
  else{
    struct Gate new_gate;
    new_gate.element1 = element1;
    new_gate.tap1 = tap1;
    new_gate.element2 = element2;
    new_gate.tap2 = tap2;
    new_gate.type = type;
    new_gate.out = 0;
    new_gate.species = 'g';

    gate[*counter] = new_gate;
    *counter += 1;
    printf("Made a gate!");
  }
}


struct Logic_Module create_logic_module(unsigned int * reg,
                                        unsigned int reg_tap){

  struct Gate array_of_gates[10];
  struct Logic_Module lm;
  unsigned int zero = 0;

  lm.array_of_gates = array_of_gates;
  lm.counter = zero; // How many of the array locations are being used.
  lm.reg = reg;
  lm.reg_inlet_value = reg_tap;
  lm.final_value = zero;

  return lm;
}


void compute_gate(struct Gate * a_gate){
  unsigned int x = get_bit(*a_gate->element1, a_gate->tap1);
  unsigned int y = get_bit(*a_gate->element2, a_gate->tap2);
  unsigned int z = operate(x, y, a_gate->type);
  a_gate->out =  z;
}


void compute_wire(struct Wire * a_wire){
  unsigned int x = get_bit(*a_wire->source, 0);
  x = x<<*a_wire->tap_destination;
  *a_wire->destination = (*a_wire->destination | x);
}

void compute_gate_array(struct Gate * a_gate, unsigned int array_length){
  for (int i=0; i<array_length; i++){
    compute_gate(&a_gate[i]);
  }
}
