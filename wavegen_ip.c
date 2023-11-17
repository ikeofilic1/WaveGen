#include <stdint.h>          // C99 integer types -- uint32_t
#include <stdio.h>
#include <stdbool.h>         // bool
#include <fcntl.h>           // open
#include <sys/mman.h>        // mmap
#include <unistd.h>          // close
#include <strings.h>
#include "../address_map.h"  // address map
#include "wavegen_ip.h"         // gpio
#include "wavegen_regs.h"       // registers

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

uint32_t *base = NULL;

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

bool wavegenOpen()
{
    // Open /dev/mem
    int file = open("/dev/mem", O_RDWR | O_SYNC);
    bool bOK = (file >= 0);
    if (bOK)
    {
        // Create a map from the physical memory location of
        // /dev/mem at an offset to LW avalon interface
        // with an aperature of SPAN_IN_BYTES bytes
        // to any location in the virtual 32-bit memory space of the process
        base = mmap(NULL, SPAN_IN_BYTES, PROT_READ | PROT_WRITE, MAP_SHARED,
                    file, AXI4_LITE_BASE + WAVEGEN_BASE_OFFSET);
        bOK = (base != MAP_FAILED);

        // Close /dev/mem
        close(file);
    }
    return bOK;
}

void configureDC(char *channel, int16_t offset) 
{
    int isChannelA = strcasecmp(channel, "a") == 0;
    int modeShift = isChannelA ? 0 : 3;
    int valueShift = isChannelA ? 0 : 16;

    // clear previous settings
    *(base + OFS_MODE) &= ~(0x7 << modeShift);
    *(base + OFS_OFFSET) &= ~(0xFFFF << valueShift);

    // set new configuration
    *(base + OFS_MODE) |= (MODE_DC << modeShift);
    *(base + OFS_OFFSET) |= ((uint16_t)offset << valueShift);

    printf("Setting channel %s as DC %.2fV\n", isChannelA ? "A" : "B", (float)offset/10000);
}

void configureWaveform(char *channel, int mode, uint32_t frequency, uint16_t amplitude, int16_t offset, uint16_t dutyCycle, int16_t phase_offs) 
{
    int isChannelA = strcasecmp(channel, "a") == 0;
    int OFS_FREQ = isChannelA ? OFS_FREQ_A : OFS_FREQ_B;
    int modeShift = isChannelA ? 0 : 3;
    int valueShift = isChannelA ? 0 : 16;

    // clear previous settings
    *(base + OFS_MODE) &= ~(0x7 << modeShift);
    *(base + OFS_FREQ) = 0;
    *(base + OFS_OFFSET) &= ~(0xFFFF << valueShift);
    *(base + OFS_AMPLITUDE) &= ~(0xFFFF << valueShift);
    *(base + OFS_DTYCYC) &= ~(0xFFFF << valueShift);
    *(base + OFS_PHASE_OFFS) &= ~(0xFFFF << valueShift);

    // set new configuration
    *(base + OFS_MODE) |= (mode << modeShift);
    *(base + OFS_FREQ) = frequency;
    *(base + OFS_OFFSET) |= ((uint16_t)offset << valueShift);
    *(base + OFS_AMPLITUDE) |= (amplitude << valueShift);
    *(base + OFS_DTYCYC) |= (dutyCycle << valueShift);
    *(base + OFS_PHASE_OFFS) |= ((uint16_t)phase_offs << valueShift);

    char *wave;
    switch (mode) 
    {
        case MODE_SINE:     wave = "sine"; break;
        case MODE_SAWTOOTH: wave = "sawtooth"; break;
        case MODE_TRIANGLE: wave = "triangle"; break;
        case MODE_SQUARE:   wave = "square"; break; 
        case MODE_ARB:      wave = "arbitrary"; break;
        default:            wave = "error";
    }

    bool kHz = frequency >= 1000; 

    if (mode == MODE_SQUARE)
        printf("Setting channel %s as a %'.3f%s square wave, with amplitude %'.2fV, offset %'.2fV, %'.2f%% duty cycle, and %'.2f degrees out of phase\n",
            isChannelA ? "A" : "B", 
            kHz ? frequency*1.0/1000 : frequency*1.0, 
            kHz ? "kHz" : "Hz", amplitude*1.0/10000, 
            offset*1.0/10000, dutyCycle*100.0/(1<<16), phase_offs*1.0/100);
    else 
        printf("Setting channel %s as a %'.3f%s %s wave, with amplitude %'.2fV, offset %'.2fV, and %'.2f degrees out of phase\n",
            isChannelA ? "A" : "B", 
            kHz ? frequency*1.0/1000 : frequency*1.0, 
            kHz ? "kHz" : "Hz", 
            wave, amplitude*1.0/10000, 
            offset*1.0/10000, phase_offs*1.0/100);
}

void setCycles(char *channel, uint16_t cycles) 
{
    int isChannelA = strcasecmp(channel, "a") == 0;
    int valueShift = isChannelA ? 0 : 16;

    *(base + OFS_CYCLES) &= ~(0xFFFF << valueShift);
    *(base + OFS_CYCLES) |= (cycles << valueShift);

    channel = isChannelA ? "A" : "B";
    if (cycles)
        printf("Limiting channel %s to %d cycles", channel, cycles);
    else
        printf("Setting channel %s to run forever", channel);
}

void configureRun() 
{
    *(base + OFS_RUN) = RUN_A | RUN_B;
}

void configureStop() 
{
    *(base + OFS_RUN) = ~(RUN_A | RUN_B);
}