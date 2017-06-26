/** @file patest_two_rates.c
 * Author: Phil Burk  http://www.softsynth.com
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "portaudio.h"

#define OUTPUT_DEVICE       (Pa_GetDefaultOutputDeviceID())
#define SAMPLE_RATE_1       (44100)
#define SAMPLE_RATE_2       (48000)
#define FRAMES_PER_BUFFER   (256)
#define FREQ_INCR           (0.1)

#ifndef M_PI
#define M_PI  (3.14159265)
#endif

//-------------------------------------------------

unsigned int get_bit(unsigned int, int);
unsigned int operate(unsigned int, unsigned int, char);
void print_bin(unsigned int);

struct Gate{
  unsigned int * element1;
  unsigned int tap1;
  unsigned int * element2;
  unsigned int tap2;
  char type;
  unsigned int out;
};

struct Wire{
  unsigned int * source;
  unsigned int tap_source;
  unsigned int * destination;
  unsigned int tap_destination;
};

typedef struct{
  unsigned int * reg_ptr;
  struct Gate * gate_array_ptr;
  unsigned int array_length;
  struct Wire * wire_ptr;
} paTestData2;

unsigned int get_bit(unsigned int reg, int tap){
  return ((reg>>tap) & 1);
}

unsigned int operate(unsigned int left, unsigned int right, char type){
  if(type == 'X'){
    return (left ^ right);
  }
  else{
    return 0;
  }
}

void shift_reg(unsigned int * reg, int number){
  *reg = *reg>>number;
}

void print_bin(unsigned int reg){
  for(int i=31; i>-1; i--){
    unsigned int temp = reg>>i;
    unsigned int temp2 = temp & 1;
    printf("%i", temp2);
  }
  printf("\n");
}

void create_gate(struct Gate * gate, //array of gates
                 int * counter,
                 unsigned int * element1,
                 int tap1,
                 unsigned int * element2,
                 int tap2,
                 char type){


    struct Gate Gate1;
    Gate1.element1 = element1;
    Gate1.tap1 = tap1;
    Gate1.element2 = element2;
    Gate1.tap2 = tap2;
    Gate1.type = type;

    gate[*counter] = Gate1;
    *counter += 1;
}

void create_wire(struct Wire * array_wires,
                 int * counter,
                 unsigned int * source,
                 int tap_source,
                 unsigned int * destination,
                 int tap_destination){

  struct Wire new_wire;

  new_wire.source = source;
  new_wire.tap_source = tap_source;
  new_wire.destination = destination;
  new_wire.tap_destination = tap_destination;

  array_wires[*counter] = new_wire;
  *counter += 1;
}

void compute_gate(struct Gate * a_gate){
  unsigned int x = get_bit(*a_gate->element1, a_gate->tap1);
  unsigned int y = get_bit(*a_gate->element2, a_gate->tap2);
  unsigned int z = operate(x, y, a_gate->type);
  a_gate->out =  z;
}

void compute_wire(struct Wire * a_wire){
  unsigned int x = get_bit(*a_wire->source, 0);
  x = x<<a_wire->tap_destination;
  *a_wire->destination = (*a_wire->destination | x);
}

void compute_gate_array(struct Gate * a_gate, unsigned int array_length){
  for (int i=0; i<array_length; i++){
    compute_gate(&a_gate[i]);
  }
}

//------------------------------------------------------------------------------

/* This routine will be called by the PortAudio engine when audio is needed.
 ** It may called at interrupt level on some machines so don't do anything
 ** that could mess up the system like calling malloc() or free().
 */
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

    for( frameIndex=0; frameIndex<(int)framesPerBuffer; frameIndex++ )
    {

      compute_gate_array(data->gate_array_ptr, data->array_length);
      compute_wire(data->wire_ptr);
      //print_bin(*data->reg_ptr);

      float value = get_bit(*data->reg_ptr, 0);
      if (value == 0) {
        value = -1;
      }

        float something = (((float) *data->reg_ptr/4294967295) - 0.5) * 2.0;
        //printf("%f\n", something);

        /* Stereo - two channels. */
        *out++ = value;
        *out++ = value;


        shift_reg(data->reg_ptr, 1);

    }
    return 0;
}

/*******************************************************************/
int main(void)
{

    struct Gate array_gates[10];
    struct Wire array_wires[10];

    int gate_counter = 0;
    int wire_counter = 0;

    unsigned int Reg1 = 28590;


    create_gate(array_gates,
                &gate_counter,
                &Reg1,
                0,
                &Reg1,
                1,
                'X');

    create_wire(array_wires,
                &wire_counter,
                &array_gates[0].out,
                0,
                &Reg1,
                31);


  PaError err;
  PaStreamParameters outputParameters;
  PaStream *stream1;

  paTestData2 data1 = {.reg_ptr = &Reg1,
                       .gate_array_ptr = array_gates,
                       .array_length = gate_counter,
                       .wire_ptr = &array_wires[0]};

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

  int question = 1;
  while(question ==1){

  char str1[10];
  printf("Play?");
  scanf("%s", str1);

  if (strcmp(str1, "play")==0){
    // need a flag so if already playing, no new stream.
    printf("Playing...\n");

    /* Start first stream. **********************/


    err = Pa_StartStream( stream1 );
    if( err != paNoError ) goto error;

    //Pa_Sleep(3000);
}

else if (strcmp(str1, "stop")==0){

    err = Pa_StopStream( stream1 );
    if( err != paNoError ) goto error;
    Pa_CloseStream( stream1 );

    Pa_Terminate();

    return err;

error:
    Pa_Terminate();
    fprintf( stderr, "An error occured while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    return err;
  }

else if (strcmp(str1, "less")==0){
  // Need to catch before it goes negative.
  array_wires[0].tap_destination -= 1;
  printf("Did it");
}
else if (strcmp(str1, "more")==0){
  array_wires[0].tap_destination += 1;
  printf("Did it");
}
  }
}
