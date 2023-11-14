#include <stdint.h>          // C99 integer types -- uint32_t
#include <stdbool.h>         // bool
#include <fcntl.h>           // open
#include <sys/mman.h>        // mmap
#include <unistd.h>          // close
#include "../address_map.h"  // address map
#include "gpio_ip.h"         // gpio
#include "gpio_regs.h"       // registers

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

void configureWaveform(char *channel, int mode, int frequency, int amplitude, int offset, int dutyCycle) 
{
    int isChannelA = strcasecmp(channel, "a") == 0;
    int baseOffset = isChannelA ? OFS_FREQ_A : OFS_FREQ_B;
    int modeShift = isChannelA ? 0 : 3;
    int valueShift = isChannelA ? 0 : 16;

    // Clear previous settings
    for (int i = 0; i < (isChannelA ? 16 : 32); i++) {
        *(base + baseOffset) = 0;
        *(base + OFS_AMPLITUDE) &= ~(1 << i);
        *(base + OFS_OFFSET) &= ~(1 << i);
        *(base + OFS_DTYCYC) &= ~(1 << i);
    }

    for (int i = 0; i < 3; i++) {
        *(base + OFS_MODE) &= ~(1 << i);
    }

    // Set new configuration
    *(base + baseOffset) = frequency;
    *(base + OFS_AMPLITUDE) |= (amplitude << valueShift);
    *(base + OFS_OFFSET) |= (offset << valueShift);
    *(base + OFS_DTYCYC) |= (dutyCycle << valueShift);
    *(base + OFS_MODE) |= (mode << modeShift);
}
