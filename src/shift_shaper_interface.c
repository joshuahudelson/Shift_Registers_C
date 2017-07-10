#include "shift_shaper.h"

int prompt_user(struct data_for_interface the_data, int (* start_stream)(PaStream *) ){
  char input_string[10];
  printf("\nstart, stop, add, less, more, quit?");
  scanf("%s", input_string);

if (strcmp(input_string, "start")==0){
  if (*the_data.stream_in_progress == 1){
    printf("Already playing!");
  }
  else{
    *the_data.err = start_stream( the_data.stream1 );
    printf("Error number: %i\n", *the_data.err);
    if( *the_data.err != 0 ){
      goto error;
    }
    else{
      *the_data.stream_in_progress = 1;
      }
    }
  }
error:
  Pa_Terminate();
  fprintf( stderr, "An error occured while using the portaudio stream\n" );
  fprintf( stderr, "Error number: %d\n", *the_data.err );
  fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( *the_data.err ) );
  printf("This is in interface");
  return *the_data.err;
}
