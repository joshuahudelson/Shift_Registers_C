#include "shift_shaper.h"

int prompt_user(struct data_for_interface * the_data){
  char input_string[10];
  printf("\n start, stop, add, less, more, speed, quit?");
  scanf("%s", input_string);

  if (strcmp(input_string, "start")==0){
    start_the_stream(the_data);
    }
  else if (strcmp(input_string, "stop")==0){
    stop_the_stream(the_data);
  }
  else if (strcmp(input_string, "quit")==0){
    stop_the_stream(the_data);
    quit(the_data);
  }
  else if (strcmp(input_string, "speed")==0){
    *the_data->shift_speed_mod = 5;
  }
  else if (strcmp(input_string, "rr")==0){
    reg_reg(the_data);
  }
  else if (strcmp(input_string, "rg")==0){
    reg_gate(the_data);
  }
  else if (strcmp(input_string, "gg")==0){
    gate_gate(the_data);
  }
}

void start_the_stream(struct data_for_interface * the_data){
  if (*the_data->stream_in_progress == 1){
    printf("Error: Already playing!");
  }
  else{
    *the_data->err = Pa_StartStream(the_data->stream1);
    printf("Return/Error number: %i\n", *the_data->err);
    *the_data->stream_in_progress = 1;
  }
}

void stop_the_stream(struct data_for_interface * the_data){
  if (*the_data->stream_in_progress == 1) {
  *the_data->err = Pa_StopStream(the_data->stream1);
  *the_data->stream_in_progress = 0;
 }
}

void quit(struct data_for_interface * the_data){
  Pa_CloseStream(the_data->stream1);
  Pa_Terminate();
  *the_data->running = 0;
}

void reg_reg(struct data_for_interface * the_data){
  unsigned int tap_one, tap_two;
  char gate_type;

  printf("\nFirst register tap: ");
  scanf("%i", &tap_one);
  printf("\nSecond register tap: ");
  scanf("%i", &tap_two);
  printf("\nType of gate: ");
  scanf(" %c", &gate_type);

  create_gate(the_data->array_gates,
              the_data->gate_counter,
              the_data->reg,
              tap_one,
              the_data->reg,
              tap_two,
              gate_type);

  *the_data->gate_counter++; // Make an increment function that checks overflow.
}

void reg_gate(struct data_for_interface * the_data){
  unsigned int tap_one, tap_two;
  char gate_type;
  printf("Why is this getting called?");

  printf("\nRegister tap: ");
  scanf("%i", &tap_one);
  printf("\nGate tap: ");
  scanf("%i", &tap_two);
  printf("\nType of gate: ");
  scanf("%c", &gate_type);

  create_gate(the_data->array_gates,
              the_data->gate_counter,
              the_data->reg,
              tap_one,
              &(the_data->array_gates[tap_two]),
              0,
              gate_type);

  *the_data->gate_counter++; // Make an increment function that checks overflow.
}

void gate_gate(struct data_for_interface * the_data){
  unsigned int tap_one, tap_two;
  char gate_type;

  printf("\nFirst gate tap: ");
  scanf("%i", &tap_one);
  printf("\nSecond gate tap: ");
  scanf("%i", &tap_two);
  printf("\nType of gate: ");
  scanf("%c", &gate_type);

  create_gate(the_data->array_gates,
              the_data->gate_counter,
              the_data->array_gates + tap_one,
              0,
              the_data->array_gates + tap_two,
              0,
              gate_type);

  *the_data->gate_counter++; // Make an increment function that checks overflow.
}
