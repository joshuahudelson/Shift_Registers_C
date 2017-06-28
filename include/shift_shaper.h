#include <stdio.h>
#include <string.h>
#include <math.h>
#include "portaudio.h"

#define OUTPUT_DEVICE       (Pa_GetDefaultOutputDeviceID())
#define SAMPLE_RATE_1       (44100)
#define FRAMES_PER_BUFFER   (256)

#define MAX_NUM_GATES       (10)
#define MAX_NUM_WIRES       (10)

struct Reg{
  unsigned int state;
  char species;
};

struct Gate{
  unsigned int * element1;
  unsigned int tap1;
  unsigned int * element2;
  unsigned int tap2;
  char type;
  unsigned int out;
  char species;
};

struct Wire{
  unsigned int * source;
  unsigned int tap_source;
  unsigned int * destination;
  unsigned int * tap_destination;
  char species;
};

typedef struct{
  struct Reg * reg_ptr;
  struct Gate * gate_array_ptr;
  unsigned int array_length;
  struct Wire * wire_ptr;
  int * shift_speed_mod;
} paTestData2;

// Constructors ----------------------------------------------------------------
unsigned int get_bit(unsigned int, int);
unsigned int operate(unsigned int, unsigned int, char);

static int patestCallback(const void *, void *,
						  unsigned long,
						  const PaStreamCallbackTimeInfo*,
						  PaStreamCallbackFlags,
						  void * );
