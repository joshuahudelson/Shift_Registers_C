/* Portaudio code built from:
 * patest_two_rates.c
 * Author: Phil Burk  http://www.softsynth.com
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include "shift_shaper.h"

int main(void)
{
  srand(time(NULL));
  unsigned int the_reg = rand();
  int reg_inlet_value = 31;

  struct Logic_Module LM;
  LM = create_logic_module(&the_reg, reg_inlet_value);

/*
  create_gate(LM.array_of_gates,
              &LM.counter,
              LM.reg,
              3,
              LM.reg,
              4,
              'X',
              'r',
               0,
               0);
*/

  int running = 1;
  int stream_in_progress = 0;
  float shift_speed_mod = 1.0;

  PaError err;
  PaStreamParameters outputParameters;
  PaStream *stream1;

  paTestData2 pa_data = {.reg_ptr = &the_reg,
                         .shift_speed_mod = &shift_speed_mod,
                         .LM = &LM
                        };

  err = Pa_Initialize();
  //if( err != paNoError ) goto error;

  outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
  if (outputParameters.device == paNoDevice) {
    fprintf(stderr,"Error: No default output device.\n");
    //goto error;
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
          &pa_data);

  // get rid of array_gates and gate_counter and instead send just the LM.
  struct data_for_interface interface_data;
  interface_data.err = &err;
  interface_data.stream_in_progress = &stream_in_progress;
  interface_data.stream1 = stream1;
  interface_data.running = &running;
  interface_data.reg = &the_reg;
  interface_data.shift_speed_mod = &shift_speed_mod;
  interface_data.LM = &LM;


  while(running == 1){
    prompt_user(&interface_data);
  }
}

//----------------CALLBACK FUNCTION---------------------------------------------

static int patestCallback( const void *inputBuffer, void *outputBuffer,
							unsigned long framesPerBuffer,
							const PaStreamCallbackTimeInfo* timeInfo,
							PaStreamCallbackFlags statusFlags,
							void *userData )
{
		paTestData2 * pa_data = (paTestData2*)userData;
		float *out = (float*)outputBuffer;
		int frameIndex;
		(void) timeInfo; /* Prevent unused variable warnings. */
		(void) inputBuffer;

		float framecount = 0.0;

// "Perform" function ----------------------------------------------------------

		for( frameIndex=0; frameIndex<(int)framesPerBuffer; frameIndex++ )
		{
      compute_logic_module(pa_data->LM);

			float final_bit = get_bit(*pa_data->reg_ptr, 0);
			if (final_bit == 0) {
				final_bit = -1; // use full amplitude range
			}

				/* Stereo - two channels. */
				*out++ = final_bit;
				*out++ = final_bit;
        // printf("\r%f", out);

				framecount += *pa_data->shift_speed_mod;
				if (framecount >= 1.0){
					framecount = 0.0;
          shift_reg(pa_data->reg_ptr, 1);
				}
		}
		return 0;
}
