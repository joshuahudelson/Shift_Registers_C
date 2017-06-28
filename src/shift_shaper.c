//Borrowed code from:
/** @file patest_two_rates.c
 * Author: Phil Burk  http://www.softsynth.com
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "portaudio.h"
#include "shift_shaper.h"


static int patestCallback( const void *inputBuffer, void *outputBuffer,
						  unsigned long framesPerBuffer,
						  const PaStreamCallbackTimeInfo* timeInfo,
						  PaStreamCallbackFlags statusFlags,
						  void *userData )
{
    paTestData2 *data = (paTestData2*)userData;
    float *out = (float*)outputBuffer;
    int frameIndex;
    (void) timeInfo; /* Prevent unused variable warnings. */
    (void) inputBuffer;

    int framecount = 0;

// "Perform" function ----------------------------------------------------------

    for( frameIndex=0; frameIndex<(int)framesPerBuffer; frameIndex++ )
    {

      compute_gate_array(data->gate_array_ptr, data->array_length);
      compute_wire(data->wire_ptr);

      float final_bit = get_bit(data->reg_ptr->state, 0);
      if (final_bit == 0) {
        final_bit = -1; // use full amplitude range
      }


        //float whole_int = (((float) *data->reg_ptr/4294967295) - 0.5) * 2.0;

        /* Stereo - two channels. */
        *out++ = final_bit;
        *out++ = final_bit;

        framecount ++;
        if (framecount >= 44100){
          framecount = 0;
        }
        if ((framecount % *data->shift_speed_mod) == 0){
        shift_reg(&data->reg_ptr->state, 1);
      }
    }
    return 0;
}

/*******************************************************************/
int main(void)
{
    struct Reg array_regs[10];
    struct Gate array_gates[10];
    struct Wire array_wires[10];

    int reg_counter = 0;
    int gate_counter = 0;
    int wire_counter = 0;

    unsigned int Reg1 = 28590;
    unsigned int Reg1_inlet = 31;

    int running = 1;
    int stream_in_progress = 0;

    int shift_speed_mod = 1;

    create_reg(array_regs,
               28590,
               &reg_counter);

    create_gate(array_gates,
                &gate_counter,
                &array_regs[0].state,
                0,
                &array_regs[0].state,
                1,
                'X');

    print_reg_and_gates(array_regs[0], array_gates, MAX_NUM_GATES);
    print_gate(&array_gates[0], array_regs, array_gates);

    create_wire(array_wires,
                &wire_counter,
                &array_gates[0].out,
                0,
                &array_regs[0].state,
                &Reg1_inlet);


  PaError err;
  PaStreamParameters outputParameters;
  PaStream *stream1;

  paTestData2 data1 = {.reg_ptr = array_regs,
                       .gate_array_ptr = array_gates,
                       .array_length = gate_counter,
                       .wire_ptr = &array_wires[0],
                       .shift_speed_mod = &shift_speed_mod,
                     };

  err = Pa_Initialize();
  if( err != paNoError ) goto error;

  outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
  if (outputParameters.device == paNoDevice) {
  fprintf(stderr,"Error: No default output device.\n");
  goto error;
  }
  outputParameters.channelCount = 2;       /* stereo output */
  outputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
  outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
  outputParameters.hostApiSpecificStreamInfo = NULL;

  err = Pa_OpenStream(
          &stream1,
          NULL, /* no input */
          &outputParameters,
          SAMPLE_RATE_1,
          FRAMES_PER_BUFFER,
          paClipOff,      /* we won't output out of range samples so don't bother clipping them */
          patestCallback,
          &data1 );
if( err != paNoError ) goto error;


// Run Loop --------------------------------------------------------------------

  while(running == 1){

    char str1[10];
    printf("\nstart, stop, add, less, more, quit?");
    scanf("%s", str1);

  if (strcmp(str1, "start")==0){  //  ---------------- start
    if (stream_in_progress == 1){
      printf("Already playing!");
    }
    else{
      err = Pa_StartStream( stream1 );
      if( err != paNoError ){
        goto error;
      }
      else{
        stream_in_progress = 1;
        }
      }
    }

  else if (strcmp(str1, "stop")==0){  //  ------------- stop

    err = Pa_StopStream( stream1 );
    if( err != paNoError ) goto error;
    stream_in_progress = 0;
  }
  else if (strcmp(str1, "quit")==0){  // -------------- quit

    err = Pa_StopStream( stream1 );
    if( err != paNoError ) goto error;
    stream_in_progress = 0;

    Pa_CloseStream( stream1 );
    Pa_Terminate();
    running = 0;
    return err;
error:
    Pa_Terminate();
    fprintf( stderr, "An error occured while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    return err;
  }
else if (strcmp(str1, "less")==0){  // -------------- less
  if (*array_wires[0].tap_destination <=0) {
    printf("Beginning of array reached!\n");
  }
  else {
  *array_wires[0].tap_destination -= 1;
  printf("Done.\n");
}
}
else if (strcmp(str1, "more")==0){
  if (*array_wires[0].tap_destination >= 31) {
    printf("End of array reached!\n");
  }
  else {
  *array_wires[0].tap_destination += 1;
  printf("Done.\n");
    }
    print_gate(&array_gates[0], array_regs, array_gates);
  }
else if (strcmp(str1, "speed")==0){
  shift_speed_mod = 5;
}
else if (strcmp(str1, "show")==0){
  print_reg_and_gates(array_regs[0], &array_gates[0], MAX_NUM_GATES);
}
}
}
