# Shift_Registers_C
Linear Feedback Shift Register simulation for sound synthesis.

Linear Feedback Shift Registers are a mainstay of cryptography.  Certain feedback arrays (based on primitive polynomials) can generate the full range of states for an n-bit register.  More interesting--for musical purposes--are arrangements that produce repetitive patters.

Shiftshaper lets the user build the feedback array from the command line and adjust several parameters.  After each command, the register is printed as a binary sequece, followed by a list of gates, with connections printed below.

Commands:

"r": adds a gate linking two taps on the register.

"b": adds a gate linking a tap on the register with a gate.

"g": adds a gate linking two gates.

"move": moves the inlet on the register that receives the output of the feedback array (which is always the last gate in the array).  Initial value is 31, the leftmost bit of the register.

"jumpstart": resets the register to a random integer.

"speed": scales the speed by a value between 0.0 (off) and 1.0 (full speed).

"start": starts the DSP engine.  The output signal value is always the smallest (rightmost) digit of the register--1 if it's 1, and -1 if it's 0.

"stop": stops the DSP engine.

"quit": stops the DSP engine and exits the program.
