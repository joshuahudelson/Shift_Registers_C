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

// This is the data sent to PortAudio's DSP function.
typedef struct{
  struct Reg * reg_ptr;
  struct Gate * gate_array_ptr;
  unsigned int array_length;
  struct Wire * wire_ptr;
  int * shift_speed_mod;
} paTestData2;

struct data_for_interface{
  PaError * err;
  int * stream_in_progress;
  PaStream * stream1;
  int * running;
  struct Wire * array_wires;
  struct Reg * array_regs;
  struct Gate * array_gates;
  int * shift_speed_mod;
};

// Function Declarations -------------------------------------------------------

unsigned int get_bit(unsigned int, int);
unsigned int operate(unsigned int, unsigned int, char);
void shift_reg(unsigned int *, int);
void print_bin(struct Reg);
void print_gate_array(struct Gate *, int);
void print_reg_and_gates(struct Reg, struct Gate *, int);
void create_reg(struct Reg *, int, int *);
void create_gate(struct Gate *, int *, unsigned int *, int, unsigned int *, int, char);
void create_wire(struct Wire *, int *, unsigned int *, int, unsigned int *, int *);
void compute_gate(struct Gate *);
void compute_wire(struct Wire *);
void compute_gate_array(struct Gate *, unsigned int);
void print_gate(struct Gate *, struct Reg *, struct Gate *);

static int patestCallback(const void *,
                          void *,
						              unsigned long,
						              const PaStreamCallbackTimeInfo*,
						              PaStreamCallbackFlags,
						              void * );
