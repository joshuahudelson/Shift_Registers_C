# Shiftshaper
Linear Feedback Shift Register simulation for sound synthesis.

Linear Feedback Shift Registers are a mainstay of cryptography.  Certain feedback arrays (based on primitive polynomials) generate the full range of states for an n-bit register.  More interesting--for musical purposes--are arrangements that produce shorter repetitive patterns.

Shiftshaper lets the user build a feedback array from the command line and adjust several parameters of the system.  After each command, the register is printed as a binary sequece, followed by a list of gates, with connections printed below.

Commands:

"start": starts the DSP engine.  The output signal value at any given time is the rightmost digit of the register: 1.0 if it's 1, and -1.0 if it's 0.

"stop": stops the DSP engine.

"r": adds a gate linking two taps on the register.

"b": adds a gate linking a tap on the register with a gate.

"g": adds a gate linking two gates.

"move": moves the inlet on the register that receives the output of the feedback array (always the value of last gate in the array).  The inlet's initial location is 31, the leftmost bit of the register.

"jump": resets the register to a random integer.

"speed": scales the shift rate by a value between 0.0 (off) and 1.0 (once per sample at a sampling rate of 44100 Hz).



"quit": stops the DSP engine and exits the program.
