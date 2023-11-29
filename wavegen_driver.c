// QE IP Example
// QE Driver (qe_driver.c)
// Jason Losh

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: Xilinx XUP Blackboard

// Hardware configuration:
// 
// AXI4-Lite interface
//   Mapped to offset of 0x10000
// 
// QE 0 and 1 interface:
//   GPIO[11-10] are used for QE 0 inputs
//   GPIO[9-8] are used for QE 1 inputs

// Load kernel module with insmod qe_driver.ko [param=___]

//-----------------------------------------------------------------------------

#include <linux/kernel.h>     // kstrtouint
#include <linux/module.h>     // MODULE_ macros
#include <linux/init.h>       // __init
#include <linux/kobject.h>    // kobject, kobject_atribute,
                              // kobject_create_and_add, kobject_put
#include <asm/io.h>           // iowrite, ioread, ioremap_nocache (platform specific)
#include "../address_map.h"   // overall memory map
#include "qe_regs.h"          // register offsets in QE IP

//-----------------------------------------------------------------------------
// Kernel module information
//-----------------------------------------------------------------------------

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jason Losh");
MODULE_DESCRIPTION("QE IP Driver");

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

static unsigned int *base = NULL;

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

// Mode
void setMode(uint8_t mode)
{
    iowrite32(mode, base + OFS_MODE);
}

int32_t getMode()
{
    return ioread32(base + OFS_MODE);
}

// Run

// Freq A
void setfreqA(unsigned int freqA)
{
    iowrite32(freqA, base + OFS_FREQ_A);
}

int32_t getfreqA(uint8_t freqA)
{
    return ioread32(base + (OFS_FREQ_A + (freqA * 2)));
}

// Freq B
void setfreqB(unsigned int freqB)
{
    iowrite32(freqB, base + OFS_FREQ_B);
}

int32_t getfreqB(uint8_t freqB)
{
    return ioread32(base + (OFS_FREQ_B + (freqB * 2)));
}

// Offset
void setOffset(unsigned int offset)
{
    iowrite32(offset, base + OFS_PERIOD);
}

int32_t getOffset(uint8_t offset)
{
    return ioread32(base + (OFS_OFFSET + (offset * 2)));
}

// Amplitude
void setAmplitude(uint32_t amplitude)
{
    iowrite32(amplitude, base + OFS_AMPLITUDE);
}

uint32_t getAmplitude()
{
    return ioread32(base + OFS_AMPLITUDE);
}

// Duty Cycle
void setDutyCycle(uint32_t dutyCycle)
{
    iowrite32(dutyCycle, base + OFS_DTYCYC);
}

uint32_t getDutyCycle()
{
    return ioread32(base + OFS_DTYCYC);
}

// Cycles
void setCycle(uint32_t cycle)
{
    iowrite32(cycle, base + OFS_CYCLES);
}

uint32_t getCycle()
{
    return ioread32(base + OFS_CYCLES);
}

// PHASE OFFS
void setPhaseOffset(uint32_t phaseOffset)
{
    iowrite32(phaseOffset, base + OFS_PHASE_OFFS);
}

uint32_t getPhaseOffset()
{
    return ioread32(base + OFS_PHASE_OFFS);
}

//-----------------------------------------------------------------------------
// Need to fix this - 
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Kernel Objects
//-----------------------------------------------------------------------------

static struct kobj_attribute swap0Attr = __ATTR(swap0, 0664, swap0Show, swap0Store);

// Position 0
static int position0 = 0;
module_param(position0, int, S_IRUGO);
