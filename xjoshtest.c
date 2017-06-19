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

int get_bit(int, int);
int operate(int, int, char);
void print_bin(int);

struct Link{
  unsigned int * element;
  int tap;
};

struct Gate{
  struct Link in1;
  struct Link in2;
  char type;
  struct Link out;
};

typedef struct{
  unsigned int * reg_ptr;
  struct Gate * gate_ptr;
} paTestData2;

int get_bit(int reg, int tap){
  return ((reg>>tap) & 1);
}

int operate(int left, int right, char type){
  if(type == 'X'){
    return (left ^ right);
  }
  else{
    return 0;
  }
}

void compute_gate(struct Gate a_gate){
  int x = get_bit(*a_gate.in1.element, a_gate.in1.tap);
  printf("Xvalue: %i  ", x);
  int y = get_bit(*a_gate.in2.element, a_gate.in2.tap);
  printf("Yvalue: %i  ", y);
  int z = operate(x, y, a_gate.type);
  printf("Zvalue: %i\n", z);
  z = z<<a_gate.out.tap;
  *a_gate.out.element = (*a_gate.out.element | z); //maybe this shouldn't be an OR gate...just equals?
                                                  //so right-most inlet is always dominant?
}

void shift_reg(unsigned int * reg, int number){
  *reg = *reg>>number;
}


void print_bin(int reg){
  for(int i=31; i>0; i--){
    int temp = reg>>i;
    int temp2 = temp & 1;
    printf("%i", temp2);
  }
  printf("\n");
}

void print_gate_state(struct Gate a_gate){
  printf("in1 element: %p\n", a_gate.in1.element);
  printf("in1 *element: %i\n", *a_gate.in1.element);
  printf("in1 tap: %i\n", a_gate.in1.tap);
  printf("in2 element: %p\n", a_gate.in2.element);
  printf("in2 *element: %i\n", *a_gate.in2.element);
  printf("in2 tap: %i\n", a_gate.in2.tap);
  printf("type: %c\n", a_gate.type);
  printf("out element: %p\n", a_gate.out.element);
  printf("out *element: %i\n", *a_gate.out.element);
  printf("out tap: %i\n", a_gate.out.tap);
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
        printf("%f\n", something);

        /* Stereo - two channels. */
        *out++ = value;
        *out++ = value;

        print_bin(*data->reg_ptr);
        shift_reg(data->reg_ptr, 1);
        compute_gate(*data->gate_ptr);

    }
    return 0;
}

/*******************************************************************/
int main(void)
{

    unsigned int Reg1 = 4;

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

    PaError err;
    PaStreamParameters outputParameters;
    PaStream *stream1;

    paTestData2 data1 = {.reg_ptr = &Reg1, .gate_ptr = &Gate1};

    printf("%i\n", *data1.reg_ptr);

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

    Pa_Sleep( 1000 );

    //Reg1 = 3847958;
    //printf("RESET!");

    Pa_Sleep(3000);

    err = Pa_StopStream( stream1 );
    if( err != paNoError ) goto error;

    Pa_CloseStream( stream1 );

    Pa_Terminate();

    printf("Reg is now: %i\n", Reg1);

    printf("Test finished.\n");
    return err;

error:
    Pa_Terminate();
    fprintf( stderr, "An error occured while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    return err;
}
