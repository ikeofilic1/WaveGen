// WAVEGEN IP Example, gpio.c
// WAVEGEN Shell Command

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
    }

    if (argc >= 3)
    {
        //wavegen DC OUT OFS
        if (strcmp(argv[1], "DC") == 0)
        {
            double offset = atof(argv[3]);
            configureDC();
        }

        if (strcmp(argv[1], "cycles") == 0)
        {
            if (argc == 3 && strcmp(argv[2], "continuous") == 0)
            {
                configureDC();
            }
            else 
            {
                int cycles = atoi(argv[3]);
                configureDC();
            }
        }

        // wavegen sine OUT, FREQ, AMP, [OFS]
        if (strcmp(argv[1], "sine") == 0)
        {
            int mode = 1;
            double frequency = atof(argv[3]);
            double amplitude = atof(argv[4]);
            if (argc == 6) 
            {
                double offset = atof(argv[5]);
            } 
            else 
            {
                double offset = 0.0;
            }
            configureWaveform(channel, mode, frequency, amplitude, offset, dutyCycle)
        }

        // wavegen sawtooth OUT, FREQ, AMP, [OFS]
        else if (strcmp(argv[1], "sawtooth") == 0)
        {
            int mode = 2;
            double frequency = atof(argv[3]);
            double amplitude = atof(argv[4]);
            if (argc == 6) 
            {
                double offset = atof(argv[5]);
            } 
            else 
            {
                double offset = 0.0;
            }
            configureWaveform(channel, mode, frequency, amplitude, offset, dutyCycle)
        }

        // wavegen triangle OUT, FREQ, AMP, [OFS]
        else if (strcmp(argv[1], "triangle") == 0)
        {
            int mode = 3;
            double frequency = atof(argv[3]);
            double amplitude = atof(argv[4]);
            if (argc == 6) 
            {
                double offset = atof(argv[5]);
            } 
            else 
            {
                double offset = 0;
            }
            configureWaveform(channel, mode, frequency, amplitude, offset, dutyCycle)
        }

        //wavegen square OUT, FREQ, AMP, [OFS, [DC]]
        else if (strcmp(argv[1], "square") == 0)
        {
            int mode = 4;
            double frequency = atof(argv[3]);
            double amplitude = atof(argv[4]);

            if (argc >= 6) 
            {
                double offset = atof(argv[5]);
            } 
            else 
            {
                double offset = 0.0;
            }
            if (argc >= 7) 
            {
                double dutyCycle = atof(argv[6]);
            } 
            else 
            {
                double dutyCycle = 50.0;
            }

            if (dutyCycle < 0.0) 
            {
                double dutyCycle = 0.0;
            } 
            else if (dutyCycle > 100.0) 
            {
                double dutyCycle = 100.0;
            }
            configureWaveform(channel, mode, frequency, amplitude, offset, dutyCycle)
        }
        wavegenClose();
    }
    return EXIT_SUCCESS;
}










