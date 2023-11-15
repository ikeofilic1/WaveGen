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

#include <stdint.h>
#include <stdbool.h>

// Define the maximum length of the arbitrary waveform
#define MAX_ARBITRARY_WAVEFORM_LENGTH 1000

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

#define OFS_MODE         0
#define OFS_RUN          4
#define OFS_FREQ_A       8
#define OFS_FREQ_B      12
#define OFS_OFFSET      16
#define OFS_AMPLITUDE   20
#define OFS_DTYCYC      24

#define OFS_CYCLES      28

#define SPAN_IN_BYTES 32

bool wavegenOpen();
void configureDC(char *channel, double offset, int dutyCycle);
void configureWaveform(char *channel, int mode, double frequency, double amplitude, double offset, int dutyCycle);
void configureRun();
void configureStop();

#endif // WAVEGEN_IP_H