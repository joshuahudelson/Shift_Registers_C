#include "shift_shaper.h"
#include <math.h>
#include <time.h>

int prompt_user(struct data_for_interface * interface_data){
  char input_string[10];
  print_reg_and_gates(interface_data->LM, interface_data->reg);
  printf("\n start, stop, r/b/g, show, move, speed, jump, quit?  ");
  scanf("%s", input_string);

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
  float temp_amount;
  printf("Amount: ");
  scanf("%f", &temp_amount);
  *shift_speed_mod = temp_amount;
}

void jump(unsigned int * reg){
  srand(time(NULL));
  *reg = rand();
}

void move_inlet(struct Logic_Module * LM){
  int temp_amount;
  printf("Index: ");
  scanf("%i", &temp_amount);
  LM->reg_inlet_value = temp_amount % 32;
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
  unsigned int tap_one, tap_two;
  char gate_type;

  printf("\nFirst register tap: ");
  scanf("%i", &tap_one);
  printf("\nSecond register tap: ");
  scanf("%i", &tap_two);
  printf("\nType of gate: ");
  scanf(" %c", &gate_type);

  create_gate(interface_data->LM->array_of_gates,
              &interface_data->LM->counter,
              interface_data->reg,
              tap_one,
              interface_data->reg,
              tap_two,
              gate_type,
              species,
              0,
              0);
}

void reg_gate(struct data_for_interface * interface_data, char species){
  unsigned int tap_one, tap_two;
  char gate_type;

  printf("\nRegister tap: ");
  scanf("%i", &tap_one);
  printf("\nGate number: ");
  scanf("%i", &tap_two);
  printf("\nType of gate: ");
  scanf(" %c", &gate_type);

  create_gate(interface_data->LM->array_of_gates,
              &interface_data->LM->counter,
              interface_data->reg,
              tap_one,
              &interface_data->LM->array_of_gates[tap_two].out,
              0,
              gate_type,
              species,
              tap_two,
              0);
}


void gate_gate(struct data_for_interface * interface_data, char species){
  unsigned int tap_one, tap_two;
  char gate_type;

  printf("\nFirst gate number: ");
  scanf("%i", &tap_one);
  printf("\nSecond gate number: ");
  scanf("%i", &tap_two);
  printf("\nType of gate: ");
  scanf(" %c", &gate_type);

  create_gate(interface_data->LM->array_of_gates,
              &interface_data->LM->counter,
              &interface_data->LM->array_of_gates[tap_one].out,
              0,
              &interface_data->LM->array_of_gates[tap_two].out,
              0,
              gate_type,
              species,
              tap_one,
              tap_two);
}
