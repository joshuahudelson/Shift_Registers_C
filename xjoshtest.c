/** @file patest_two_rates.c
	@ingroup test_src
	@brief Play two streams at different rates to make sure they don't interfere.
	@author Phil Burk <philburk@softsynth.com>
*/
/*
 * $Id$
 *
 * Author: Phil Burk  http://www.softsynth.com
 *
 * This program uses the PortAudio Portable Audio Library.
 * For more information see: http://www.portaudio.com
 * Copyright (c) 1999-2000 Ross Bencina and Phil Burk
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * The text above constitutes the entire PortAudio license; however,
 * the PortAudio community also makes the following non-binding requests:
 *
 * Any person wishing to distribute modifications to the Software is
 * requested to send the modifications to the original developer so that
 * they can be incorporated into the canonical version. It is also
 * requested that these non-binding requests be included along with the
 * license above.
 */

#include <stdio.h>
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

// Links go inside gates.  They're really more like inlets.
struct Link{
  unsigned int * element;
  int tap;
};

struct Gate{
  struct Link in1;
  struct Link in2;
  char type;
  unsigned int out;
};


typedef struct{
  unsigned int * reg_ptr;
  struct Gate * gate_ptr;
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

void print_gate_state(struct Gate a_gate){
  printf("in1 *element: %i\n", *a_gate.in1.element);
  printf("in1 tap: %i\n", a_gate.in1.tap);
  printf("in2 *element: %i\n", *a_gate.in2.element);
  printf("in2 tap: %i\n", a_gate.in2.tap);
  printf("type: %c\n", a_gate.type);
  printf("out element: %i\n", a_gate.out);
}

struct Wire{
  unsigned int * source;
  unsigned int tap_source;
  unsigned int * destination;
  unsigned int tap_destination;
};

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

void compute_gate(struct Gate a_gate){
  unsigned int x = get_bit(*a_gate.in1.element, a_gate.in1.tap);
  printf("Xvalue: %i  ", x);
  unsigned int y = get_bit(*a_gate.in2.element, a_gate.in2.tap);
  printf("Yvalue: %i  ", y);
  unsigned int z = operate(x, y, a_gate.type);
  printf("Zvalue: %i\n", z);
  a_gate.out =  z;
}

void compute_wire(struct Wire * a_wire){
  unsigned int x = get_bit(a_wire[0].source, 0);
  printf("The wire's source is: %u\n", a_wire[0].source);
  printf("The wire's source value: %u\n", x);
  printf("The wire's destination value: %u\n", a_wire[0].destination);
  x = x<<a_wire[0].tap_destination;
  a_wire[0].destination = (a_wire[0].destination | x);
}

void create_gate(struct Gate * gate,
                 int * counter,
                 unsigned int * element1,
                 int tap1,
                 unsigned int * element2,
                 int tap2,
                 char type){

    struct Link link1;
    link1.element = element1;
    link1.tap = tap1;

    struct Link link2;
    link2.element = element2;
    link2.tap = tap2;

    struct Gate Gate1;
    Gate1.in1 = link1;
    Gate1.in2 = link2;
    Gate1.type = type;

    gate[*counter] = Gate1;
    *counter += 1;
}


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

        float value = get_bit(*data->reg_ptr, 0);
        if (value == 0) {
          value = -1;
        }

        float something = (((float) *data->reg_ptr/4294967295) - 0.5) * 2.0;

        /* Stereo - two channels. */
        *out++ = value;
        *out++ = value;

        compute_gate(*data->gate_ptr);
        compute_wire(*data->wire_ptr);
        print_bin(*data->reg_ptr);
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

    unsigned int Reg1 = 1;

/*
    create_wire(array_wires,
                &wire_counter,
                &Reg1,
                0,
                &Reg1,
                16);
*/

    create_gate(array_gates,
                &gate_counter,
                &Reg1,
                0,
                &Reg1,
                1,
                'X');

    printf("array_gates[0]: %i\n", array_gates[0].out);

    create_wire(array_wires,
                &wire_counter,
                array_gates[0].out,
                0,
                &Reg1,
                31);

/*
    struct Link link1;
    link1.element = &Reg1;
    link1.tap = 0;

    struct Link link2;
    link2.element = &Reg1;
    link2.tap = 1;

    struct Link link3;
    link3.element = &Reg1;
    link3.tap = 31;

    struct Gate Gate1;
    Gate1.in1 = link1;
    Gate1.in2 = link2;
    Gate1.type = 'X';
    Gate1.out = link3;
*/

    PaError err;
    PaStreamParameters outputParameters;
    PaStream *stream1;

    paTestData2 data1 = {.reg_ptr = &Reg1, .gate_ptr = &array_gates[0], .wire_ptr = &array_wires[0]};

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


    /* Start first stream. **********************/
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

    err = Pa_StartStream( stream1 );
    if( err != paNoError ) goto error;

    Pa_Sleep(1);

    err = Pa_StopStream( stream1 );
    if( err != paNoError ) goto error;

    Pa_CloseStream( stream1 );

    Pa_Terminate();

    printf("Gate counter is now: ");
    printf("%i\n", gate_counter);
    printf("Wire counter is now: ");
    printf("%i\n", wire_counter);

    printf("Reg is now: %i\n", Reg1);

    return err;

error:
    Pa_Terminate();
    fprintf( stderr, "An error occured while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    return err;
}
