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
