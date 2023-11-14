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
    gpioOpen();
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
        else if ((strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0))
        {
            printf("Command not understood or incorrect number of arguments.\n");
        }
    }

    if (argc == 4)
    {
        //wavegen dutyCycle OUT OFS
        if (strcmp(argv[1], "dutyCycle") == 0)
        {
            double voltage = atof(argv[3]);
            configureDC();
        }

        //wavegen cycles OUT N
        else if (strcmp(argv[1], "cycles") == 0)
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
    }

    if (argc >= 6)
    {
        // wavegen sine OUT, FREQ, AMP, [OFS]
        if (strcmp(argv[1], "sine") == 0)
        {
            mode = 1;
            frequency = atof(argv[3]);
            amplitude = atof(argv[4]);
            if (argc == 6) 
            {
                offset = atof(argv[5]);
            } 
            else 
            {
                offset = 0;
            }
            configureWaveform(channel, mode, frequency, amplitude, offset, dutyCycle)
        }
        // wavegen sawtooth OUT, FREQ, AMP, [OFS]
        else if (strcmp(argv[1], "sawtooth") == 0)
        {
            mode = 2;
            frequency = atof(argv[3]);
            amplitude = atof(argv[4]);
            if (argc == 6) 
            {
                offset = atof(argv[5]);
            } 
            else 
            {
                offset = 0;
            }
            configureWaveform(channel, mode, frequency, amplitude, offset, dutyCycle)
        }

        // wavegen triangle OUT, FREQ, AMP, [OFS]
        else if (strcmp(argv[1], "triangle") == 0)
        {
            mode = 3;
            frequency = atof(argv[3]);
            amplitude = atof(argv[4]);
            if (argc == 6) 
            {
                offset = atof(argv[5]);
            } 
            else 
            {
                offset = 0;
            }
            configureWaveform(channel, mode, frequency, amplitude, offset, dutyCycle)
        }

        //wavegen square OUT, FREQ, AMP, [OFS, [DC]]
        else if (strcmp(argv[1], "square") == 0)
        {
            mode = 4;
            frequency = atof(argv[3]);
            amplitude = atof(argv[4]);

            if (argc >= 6) 
            {
                offset = atof(argv[5]);
            } 
            else 
            {
                offset = 0.0;
            }

            // Set DC
            if (argc >= 7) 
            {
                dutyCycle = atof(argv[6]);
            } 
            else 
            {
                dutyCycle = 50.0;
            }

            // Ensure duty cycle is within valid range (0-100%)
            if (dutyCycle < 0.0) 
            {
                dutyCycle = 0.0;
            } 
            else if (dutyCycle > 100.0) 
            {
                dutyCycle = 100.0;
            }
            configureWaveform(channel, mode, frequency, amplitude, offset, dutyCycle)
        }
    }
    return EXIT_SUCCESS;
}









