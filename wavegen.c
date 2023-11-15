// WAVEGEN IP Example, gpio.c
// WAVEGEN Shell Command

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: Xilinx XUP Blackboard

// Hardware configuration:
//
// AXI4-Lite interface:
//   Mapped to offset of 20000
//
// GPIO interface:
//   GPIO[31-0] is used as a general purpose GPIO port

//-----------------------------------------------------------------------------

#include <stdlib.h>          // EXIT_ codes
#include <stdio.h>           // printf
#include <string.h>          // strcmp
#include "wavegen_ip.h"         // IP library

int main(int argc, char* argv[])
{
    wavegenOpen();
    char channel = argv[2];

    if (argc == 2)
    {
        if (strcmp(argv[1], "run") == 0)
        {
            configureRun();
        }
        else if (strcmp(argv[1], "stop") == 0)
        {
            configureStop();
        }

        // TODO: add help command here
        // TODO: add arb command here
    }

    else if (argc == 3 || argc == 4)
    {
        //wavegen DC OUT OFS
        if (strcmp(argv[1], "DC") == 0)
        {
            int voltage = atoi(argv[3]);
            configureDC(channel, voltage);
        }

        else if (strcmp(argv[1], "cycles") == 0)
        {
            // wavegen cycles OUT, continuous
            if (strcmp(argv[3], "continuous") == 0)
            {
                setCycles(channel, 0);
            }
            //wavegen cycles OUT, N
            else
            {
                int cycles = atoi(argv[3]);
                setCycles(channel, cycles);
            }
        }
    }
    else if (argc >= 5) {

        uint8_t mode; // 0, 1, 2, 3, 4, or 5
        uint32_t frequency = atoi(argv[3]); // 0 to 500000000 (units of 100uHz)
        uint16_t amplitude = atoi(argv[4]); // -25000 to 25000
        int16_t offset = argc > 5 ? atoi(argv[5]) : 0; // -25000 to 25000
        uint16_t dutyCycle = argc > 6 ? atoi(argv[6]) : 0; // 0 to 65535
        int16_t phaseOffs = argc > 6 ? atoi(argv[6]) : 0; // -18000 to 18000

        // wavegen sine OUT, FREQ, AMP, [OFS]
        if (strcmp(argv[1], "sine") == 0)
        {
            mode = MODE_SINE;                        
        }
        // wavegen sawtooth OUT, FREQ, AMP, [OFS]
        else if (strcmp(argv[1], "sawtooth") == 0)
        {
            mode = MODE_SAWTOOTH;
        }
        // wavegen triangle OUT, FREQ, AMP, [OFS]
        else if (strcmp(argv[1], "triangle") == 0)
        {
            mode = MODE_TRIANGLE;
        }

        //wavegen square OUT, FREQ, AMP, [OFS, [DC]]
        else if (strcmp(argv[1], "square") == 0)
        {
            mode = MODE_SQUARE;
        }        

        configureWaveform(channel, mode, frequency, amplitude, offset, dutyCycle, phaseOffs);
    }

    return EXIT_SUCCESS;
}
