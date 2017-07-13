#include "shift_shaper.h"

int prompt_user(struct data_for_interface * the_data, int (* start_stream)(PaStream *) ){
  char input_string[10];
  printf("\n start, stop, add, less, more, speed, quit?");
  scanf("%s", input_string);

  // DEFINITELY DECOUPLE THE BELOW INTO FUNCTIONS
  // MAYBE MAKE THIS A SWITCH STATEMENT?

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
  else if (strcmp(input_string, "add")){
    add_gate(the_data);
  }
  else if (strcmp(input_string, "less")==0){
    if (*the_data->array_wires[0].tap_destination <=0) {
      printf("Beginning of array reached!\n");
    }
    else {
    *the_data->array_wires[0].tap_destination -= 1;
    printf("Done.\n");
    }
  }
  else if (strcmp(input_string, "more")==0){
    if (*the_data->array_wires[0].tap_destination >= 31) {
      printf("End of array reached!\n");
    }
    else {
    *the_data->array_wires[0].tap_destination += 1;
    printf("Done.\n");
    }
    print_gate(&the_data->array_gates[0], the_data->array_regs, the_data->array_gates);
  }
  else if (strcmp(input_string, "speed")==0){
    *the_data->shift_speed_mod = 5;
  }
  else if (strcmp(input_string, "show")==0){
    print_reg_and_gates(the_data->array_regs[0], &the_data->array_gates[0], MAX_NUM_GATES);
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

void add_gate(struct data_for_interface * the_data){
  int temp_inlet1;
  int temp_inlet2;
  char temp_type;
  printf("Inlet1: ");
  scanf("%i", &temp_inlet1);
  printf("\nInlet2: ");
  scanf("%i", &temp_inlet2);
  printf("\nType: ");
  scanf(" %c", &temp_type);

  create_gate(the_data->array_gates,
              the_data->gate_counter,
              &the_data->array_regs[0].state,
              temp_inlet1,
              &the_data->array_regs[0].state,
              temp_inlet2,
              temp_type);
}
