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

void print_bin(struct Reg reg){
  for(int i=31; i>-1; i--){
    unsigned int temp = reg.state>>i;
    unsigned int temp2 = temp & 1;
    printf("%i", temp2);
  }
}

void print_gate_array(struct Gate * array_gates, int length){
  for (int i=0; i<length; i++){
    printf("%i", i);
  }
}

void print_reg_and_gates(struct Reg reg, struct Gate * array_gates, int length){
  print_bin(reg);
  printf("          ");
  print_gate_array(array_gates, length);
  printf("\n");
}

void create_reg(struct Reg * reg,
                int initial_state,
                int * counter){

  struct Reg new_reg;
  new_reg.state = initial_state;
  new_reg.species = 'r';

  reg[*counter] = new_reg;
  *counter += 1;
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

void create_wire(struct Wire * array_wires,
                 int * counter,
                 unsigned int * source,
                 int tap_source,
                 unsigned int * destination,
                 int * tap_destination){

if (*counter >= MAX_NUM_WIRES){
 printf("Maximum number of wires has been reached!");
  }
else{
  struct Wire new_wire;
  new_wire.source = source;
  new_wire.tap_source = tap_source;
  new_wire.destination = destination;
  new_wire.tap_destination = tap_destination;
  new_wire.species = 'w';

  array_wires[*counter] = new_wire;
  *counter += 1;
  }
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

void print_gate(struct Gate * gate,
                struct Reg * array_regs,
                struct Gate * array_gates){

  unsigned int temp_source1;
  unsigned int temp_source2;
  // No, rethink this... draw on paper.  "reg" isn't a thing...
  for (int i=0; i<MAX_NUM_GATES; i++){
    if (&array_regs[i] == &gate->element1){
      temp_source1 = i*31; // this is a placeholder, for when there will be more than one reg
    }
    if (&array_gates[i].out == &gate->element1){
      temp_source1 = temp_source1 + 31 + (i * 10);
    }
    if (&array_regs[i] == &gate->element2){
      temp_source2 = i*31; // this is a placeholder, for when there will be more than one reg
    }
    if (&array_gates[i].out == &gate->element2){
      temp_source2 = temp_source2 + 31 + i;
    }
  }
    // so now we have the starting location of the reg (0) and the gate (31)
    // so we can put the links | at some number plus reg or temp_source 2 (31 plus gate num)
    temp_source1 += gate->tap1;
    temp_source2 += gate->tap2;

    for (int i=50; i>-1; i--){ // the problem is the organization: 31..0, and then 0..9
      if ((temp_source1 == i) | (temp_source2 == i)){
        printf("|");
      }
      else {
        printf(" ");
      }
      }
      }

/* This routine will be called by the PortAudio engine when audio is needed.
 ** It may called at interrupt level on some machines so don't do anything
 ** that could mess up the system like calling malloc() or free().
 */
