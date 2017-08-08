#include "shift_shaper.h"
#include <math.h>
#include <time.h>
#include <stdlib.h>

int prompt_user(struct data_for_interface * interface_data){

  print_reg_and_gates(interface_data->LM, interface_data->reg);

  char input_string[10];
  printf("\n start, stop, r/b/g, show, move, speed, jump, quit?  ");
  fgets(input_string, 10, stdin);
  input_string[strcspn(input_string, "\n")] = '\0';

  if (strcmp(input_string, "start")==0){
    start_the_stream(interface_data);
    }
  else if (strcmp(input_string, "stop")==0){
    stop_the_stream(interface_data);
  }
  else if (strcmp(input_string, "quit")==0){
    stop_the_stream(interface_data);
    quit(interface_data);
  }
  else if (strcmp(input_string, "speed")==0){
    shift_speed(interface_data->shift_speed_mod);
  }
  else if (strcmp(input_string, "r")==0){
    reg_reg(interface_data, input_string[0]);
  }
  else if (strcmp(input_string, "b")==0){
    reg_gate(interface_data, input_string[0]);
  }
  else if (strcmp(input_string, "g")==0){
    gate_gate(interface_data, input_string[0]);
  }
  else if (strcmp(input_string, "show")==0){
    printf("\n");
    }
  else if (strcmp(input_string, "move")==0){
    move_inlet(interface_data->LM);
  }
  else if (strcmp(input_string, "jump")==0){
    jump(interface_data->reg);
  }
}

void shift_speed(float * shift_speed_mod){
  char temp_amount[10];
  printf("Amount: ");
  fgets(temp_amount, 10, stdin);
  float temp_float = atof(temp_amount);
  *shift_speed_mod = temp_float;
}

void jump(unsigned int * reg){
  srand(time(NULL));
  *reg = rand();
}

void move_inlet(struct Logic_Module * LM){
  char temp_amount[10];
  printf("Index: ");
  fgets(temp_amount, 10, stdin);
  int temp_float = atoi(temp_amount);
  LM->reg_inlet_value = temp_float % 32;
}

void start_the_stream(struct data_for_interface * interface_data){
  if (*interface_data->stream_in_progress == 1){
    printf("Error: Already playing!");
  }
  else{
    *interface_data->err = Pa_StartStream(interface_data->stream1);
    //printf("Return/Error number: %i\n", *interface_data->err);
    *interface_data->stream_in_progress = 1;
  }
}

void stop_the_stream(struct data_for_interface * interface_data){
  if (*interface_data->stream_in_progress == 1) {
  *interface_data->err = Pa_StopStream(interface_data->stream1);
  *interface_data->stream_in_progress = 0;
 }
}

void quit(struct data_for_interface * interface_data){
  Pa_CloseStream(interface_data->stream1);
  Pa_Terminate();
  *interface_data->running = 0;
}


void reg_reg(struct data_for_interface * interface_data, char species){
  char tap_one_char[10], tap_two_char[10];
  unsigned int tap_one, tap_two;
  char gate_type[10];

  printf("\nFirst register tap: ");
  fgets(tap_one_char, 10, stdin);

  printf("\nSecond register tap: ");
  fgets(tap_two_char, 10, stdin);

  printf("\nType of gate: ");
  fgets(gate_type, 10, stdin); //needs to be more than 1 because of newline issue.

  tap_one = atoi(tap_one_char);
  tap_two = atoi(tap_two_char);

  create_gate(interface_data->LM->array_of_gates,
              &interface_data->LM->counter,
              interface_data->reg,
              tap_one,
              interface_data->reg,
              tap_two,
              gate_type[0],
              species,
              0,
              0);
}

void reg_gate(struct data_for_interface * interface_data, char species){
  char tap_one_char[10], tap_two_char[10];
  unsigned int tap_one, tap_two;
  char gate_type[1];

  printf("\nRegister tap: ");
  fgets(tap_one_char, 10, stdin);
  printf("\nGate number: ");
  fgets(tap_two_char, 10, stdin);
  printf("\nType of gate: ");
  fgets(gate_type, 1, stdin);

  create_gate(interface_data->LM->array_of_gates,
              &interface_data->LM->counter,
              interface_data->reg,
              tap_one,
              &interface_data->LM->array_of_gates[tap_two].out,
              0,
              gate_type[0],
              species,
              tap_two,
              0);
}


void gate_gate(struct data_for_interface * interface_data, char species){
  char tap_one_char[10], tap_two_char[10];
  unsigned int tap_one, tap_two;
  char gate_type[1];

  printf("\nFirst gate number: ");
  fgets(tap_one_char, 10, stdin);
  printf("\nSecond gate number: ");
  fgets(tap_two_char, 10, stdin);
  printf("\nType of gate: ");
  fgets(gate_type, 1, stdin);

  create_gate(interface_data->LM->array_of_gates,
              &interface_data->LM->counter,
              &interface_data->LM->array_of_gates[tap_one].out,
              0,
              &interface_data->LM->array_of_gates[tap_two].out,
              0,
              gate_type[0],
              species,
              tap_one,
              tap_two);
}
