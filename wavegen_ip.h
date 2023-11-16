// WAVEGEN IP Example
// GPIO IP Library (gpio_ip.h)
// Jason Losh

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: Xilinx XUP Blackboard

// Hardware configuration:
//
// AXI4-Lite interface:
//   Mapped to offset of 0
//
// IRQ interface:
//   IRQ_F2P[2] is used as the interrupt interface to the PS
//
// GPIO interface:
//   GPIO[31-0] is used as a general purpose GPIO port

//-----------------------------------------------------------------------------

#ifndef WAVEGEN_IP_H
#define WAVEGEN_IP_H

#include <stdbool.h>
#include <stdint.h>

// Define the maximum length of the arbitrary waveform
#define MAX_ARBITRARY_WAVEFORM_LENGTH 1000

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

#define MODE_DC         0
#define MODE_SINE       1
#define MODE_SAWTOOTH   2
#define MODE_TRIANGLE   3
#define MODE_SQUARE     4
#define MODE_ARB        5

bool wavegenOpen();
void configureDC(char *channel, int16_t offset);
void configureWaveform(char *channel, int mode, uint32_t frequency, uint16_t amplitude, int16_t offset, uint16_t dutyCycle, uint16_t phase_offs);
void configureRun();
void configureStop();
void setCycles(char *channel, uint16_t cycles);

#endif // WAVEGEN_IP_H