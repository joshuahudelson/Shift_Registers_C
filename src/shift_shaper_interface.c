#include "shift_shaper.h"

int prompt_user(struct data_for_interface * the_data, int (* start_stream)(PaStream *) ){
  char input_string[10];
  printf("\n start, stop, add, less, more, speed, quit?");
  scanf("%s", input_string);

  // DEFINITELY DECOUPLE THE BELOW INTO FUNCTIONS
  // MAYBE MAKE THIS A SWITCH STATEMENT?

  if (strcmp(input_string, "start")==0){
    if (*the_data->stream_in_progress == 1){
      printf("Already playing!");
    }
    else{
      *the_data->err = start_stream(the_data->stream1);
      printf("Error number: %i\n", *the_data->err);
      *the_data->stream_in_progress = 1;
      }
    }
  else if (strcmp(input_string, "stop")==0){
      *the_data->err = Pa_StopStream(the_data->stream1); // we'll see if I need to pass a function pointer
      *the_data->stream_in_progress = 0;
    }
  else if (strcmp(input_string, "quit")==0){
      *the_data->err = Pa_StopStream(the_data->stream1);
      *the_data->stream_in_progress = 0;

      Pa_CloseStream(the_data->stream1);
      Pa_Terminate();
      *the_data->running = 0;
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
