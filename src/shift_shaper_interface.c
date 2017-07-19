#include "shift_shaper.h"

int prompt_user(struct data_for_interface * interface_data){
  char input_string[10];
  printf("\n start, stop, add, less, more, speed, quit?");
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
    *interface_data->shift_speed_mod = 5;
  }
  else if (strcmp(input_string, "rr")==0){
    printf("calling reg_reg...\n");
    reg_reg(interface_data);
  }
  else if (strcmp(input_string, "rg")==0){
    printf("calling reg_gate...\n");
    reg_gate(interface_data);
  }
  else if (strcmp(input_string, "gg")==0){
    printf("calling gate_gate...\n");
    gate_gate(interface_data);
  }
  else if (strcmp(input_string, "show")==0){
    for(int i=0; i<interface_data->LM->counter; i++){
      printf("%i", i);
    }
  }
}

void start_the_stream(struct data_for_interface * interface_data){
  if (*interface_data->stream_in_progress == 1){
    printf("Error: Already playing!");
  }
  else{
    *interface_data->err = Pa_StartStream(interface_data->stream1);
    printf("Return/Error number: %i\n", *interface_data->err);
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


void reg_reg(struct data_for_interface * interface_data){
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
              gate_type);
}

void reg_gate(struct data_for_interface * interface_data){
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
              gate_type);
}


void gate_gate(struct data_for_interface * interface_data){
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
              gate_type);
}
