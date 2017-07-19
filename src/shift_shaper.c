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
  unsigned int reg_inlet_value = 31;

  struct Logic_Module LM;
  LM = create_logic_module(&the_reg, reg_inlet_value);

  int running = 1;
  int stream_in_progress = 0;
  int shift_speed_mod = 1;

  PaError err;
  PaStreamParameters outputParameters;
  PaStream *stream1;

  paTestData2 data1 = {.reg_ptr = &the_reg,
                       .gate_array_ptr = LM.array_of_gates,
                       .array_length = LM.counter,
                       .shift_speed_mod = &shift_speed_mod,
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
          &data1 );

  struct data_for_interface the_data;
  the_data.err = &err;
  the_data.stream_in_progress = &stream_in_progress;
  the_data.stream1 = stream1;
  the_data.running = &running;
  the_data.reg = &the_reg;
  the_data.array_gates = LM.array_of_gates;
  the_data.shift_speed_mod = &shift_speed_mod;
  the_data.gate_counter = &LM.counter;


  while(running == 1){
    prompt_user(&the_data);
  }
}

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

			float final_bit = get_bit(*data->reg_ptr, 0);
			if (final_bit == 0) {
				final_bit = -1; // use full amplitude range
			}


				//float whole_int = (((float) *data->reg_ptr/4294967295) - 0.5) * 2.0;

				/* Stereo - two channels. */
				*out++ = final_bit;
				*out++ = final_bit;
        // printf("\r%f", out);

				framecount ++;
				if (framecount >= 44100){
					framecount = 0;
				}
				if ((framecount % *data->shift_speed_mod) == 0){
				shift_reg(data->reg_ptr, 1);
			}
		}
		return 0;
}
