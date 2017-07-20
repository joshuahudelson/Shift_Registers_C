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
                 char type,
                 char species,
                 int gate1,
                 int gate2){

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
    new_gate.species = species;
    new_gate.gate1 = gate1;
    new_gate.gate2 = gate2;

    gate[*counter] = new_gate;
    *counter += 1;
    printf("\nGate created.\n");
  }
}


struct Logic_Module create_logic_module(unsigned int * reg,
                                        unsigned int reg_tap){

  struct Logic_Module lm;
  lm.counter = 0;
  lm.reg = reg;
  lm.reg_inlet_value = reg_tap;
  lm.final_value = 0;

  return lm;
}

void compute_gate(struct Gate * a_gate){
  unsigned int x = get_bit(*a_gate->element1, a_gate->tap1);
  unsigned int y = get_bit(*a_gate->element2, a_gate->tap2);
  unsigned int z = operate(x, y, a_gate->type);
  a_gate->out =  z;
}

unsigned int compute_gate_array(struct Gate * a_gate, unsigned int array_length){
  unsigned int temp_output;
  for (int i=0; i<array_length; i++){
    compute_gate(&a_gate[i]);
    temp_output = a_gate[i].out;
  }
  return temp_output;
}

void compute_logic_module(struct Logic_Module * LM){
  LM->final_value = compute_gate_array(LM->array_of_gates, LM->counter);
  *LM->reg = *LM->reg | LM->final_value<<LM->reg_inlet_value;
}

void print_int_as_binary(unsigned int * reg){
  for(int i=31; i>-1; i--){
    unsigned int temp = *reg>>i;
    unsigned int temp2 = temp & 1;
    printf("%i", temp2);
  }
}

void print_reg_and_gates(struct Logic_Module * LM, unsigned int * reg){
  printf("\n");
  print_int_as_binary(reg);
  printf("      ");
  for(int i=0; i<LM->counter; i++){
    printf("%i", i);
  }

  printf("\n");

  for(int i=0; i<LM->counter; i++){
    if (LM->array_of_gates[i].species == 'r'){
      print_reg_taps(LM->array_of_gates[i].tap1, LM->array_of_gates[i].tap2);
      printf("      ");
      print_gate_tap_and_type(LM->counter, 11, 11, i, LM->array_of_gates[i].type);
    }
    else if (LM->array_of_gates[i].species == 'b'){
      print_reg_taps(LM->array_of_gates[i].tap1, LM->array_of_gates[i].tap1);
      printf("      ");
      print_gate_tap_and_type(LM->counter, LM->array_of_gates[i].gate1, LM->array_of_gates[i].gate1, i, LM->array_of_gates[i].type);
    }
    else if (LM->array_of_gates[i].species == 'g'){
      print_zero_reg();
      printf("      ");
      print_gate_tap_and_type(LM->counter,
                              LM->array_of_gates[i].gate1,
                              LM->array_of_gates[i].gate2,
                              i,
                              LM->array_of_gates[i].type);
    }
    printf("\n");
  }
  print_inlet(LM->reg_inlet_value);
  printf("\n");
}


void print_reg_indices(void){
  for (int i=31; i>-1;i--){
    printf("%i", i);
  }
}

void print_gate_indices(int counter){
  for (int i=0;i<counter; i++){
    printf("%i", i);
  }
}


void print_inlet(int inlet){
  for (int i=31; i>-1; i--){
    if (inlet == i){
    printf("^");
    }
    else{
      printf(" ");
    }
  }
}

void print_zero_reg(void){
  for (int i=31; i>-1; i--){
    printf(" ");
  }
}

void print_reg_taps(int tap1, int tap2){
  for (int i=31; i>-1; i--){
    if ((i==tap1) | (i==tap2)){
      printf("|");
    }
    else{
      printf(" ");
    }
  }
}

void print_gate_tap_and_type(int counter, int gate1, int gate2, int gate_index, char type){
  for (int i=0; i<counter; i++){

    if ((i == gate1) | (i == gate2)){
      printf("|");
    }
    else if (i==gate_index){
      printf("%c", type);
    }
    else{
      printf(" ");
    }
  }
}
