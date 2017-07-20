#include <stdio.h>
#include <string.h>
#include <math.h>
#include "portaudio.h"

#define OUTPUT_DEVICE       (Pa_GetDefaultOutputDeviceID())
#define SAMPLE_RATE_1       (44100)
#define FRAMES_PER_BUFFER   (256)

#define MAX_NUM_GATES       (10)
#define MAX_NUM_WIRES       (10)

struct Gate{
  unsigned int * element1;
  int tap1;
  unsigned int * element2;
  int tap2;
  char type;
  int out;
  char species;
  int gate1;
  int gate2;
};

struct Logic_Module{
  struct Gate array_of_gates[10];
  int counter; // How many of the array locations are being used.
  unsigned int * reg;
  int reg_inlet_value;
  int final_value;
};

typedef struct{
  unsigned int * reg_ptr;
  float * shift_speed_mod;
  struct Logic_Module * LM;
} paTestData2;

struct data_for_interface{
  // This data gets sent to interface function.
  PaError * err;
  int * stream_in_progress;
  PaStream * stream1;
  int * running;
  unsigned int * reg;
  float * shift_speed_mod;
  struct Logic_Module * LM;
};

// Function Declarations -------------------------------------------------------

unsigned int get_bit(unsigned int, int);
unsigned int operate(unsigned int, unsigned int, char);
void shift_reg(unsigned int *, int);
void print_gate_array(struct Gate *, int);
void create_gate(struct Gate *, int *, unsigned int *, int, unsigned int *, int, char, char, int, int);
void compute_gate(struct Gate *);
unsigned int compute_gate_array(struct Gate *, unsigned int);
struct Logic_Module create_logic_module(unsigned int *, unsigned int);

static int patestCallback(const void *,
                          void *,
						              unsigned long,
						              const PaStreamCallbackTimeInfo*,
						              PaStreamCallbackFlags,
						              void * );

void compute_logic_module(struct Logic_Module *);
void print_int_as_binary(unsigned int * reg);
void print_reg_and_gates(struct Logic_Module * LM, unsigned int * reg);
void print_reg_indices(void);
void print_gate_indices(int counter);
void print_inlet(int inlet);
void print_zero_reg(void);
void print_reg_taps(int tap1, int tap2);
void print_gate_tap_and_type(int counter, int gate1, int gate2, int gate_index, char type);

int prompt_user(struct data_for_interface *);
void start_the_stream(struct data_for_interface * the_data);
void stop_the_stream(struct data_for_interface * the_data);
void quit(struct data_for_interface * the_data);
void reg_reg(struct data_for_interface *, char);
void reg_gate(struct data_for_interface *, char);
void gate_gate(struct data_for_interface *, char);
void shift_speed(float * shift_speed_mod);
void jump(unsigned int * reg);
void move_inlet(struct Logic_Module * LM);
