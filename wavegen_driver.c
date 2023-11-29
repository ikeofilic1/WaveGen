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
#include "wavegen_regs.h"          // register offsets in QE IP
#include "wavegen_ip.h" 

//-----------------------------------------------------------------------------
// Kernel module information
//-----------------------------------------------------------------------------

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jason Losh");
MODULE_DESCRIPTION("Wave Generator Driver");

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

static unsigned int *base = NULL;

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

void configureDC(char *channel, int16_t offset) 
{
    int isChannelA = strcasecmp(channel, "a") == 0;
    int modeShift = isChannelA ? 0 : 3;
    int valueShift = isChannelA ? 0 : 16;

    //clear previous settings
    iowrite32(ioread32(base + OFS_MODE) & ~(0x7 << modeShift), base + OFS_MODE);
    iowrite32(ioread32(base + OFS_OFFSET) & ~(0xFFFF << valueShift), base + OFS_OFFSET);

    //set new configuration
    iowrite32(ioread32(base + OFS_MODE) | (MODE_DC << modeShift), base + OFS_MODE);
    iowrite32(ioread32(base + OFS_OFFSET) | ((uint16_t)offset << valueShift), base + OFS_OFFSET);
}

void configureWaveform(char *channel, int mode, uint32_t frequency, uint16_t amplitude, int16_t offset, uint16_t dutyCycle, int16_t phase_offs) 
{
    int isChannelA = strcasecmp(channel, "a") == 0;
    int OFS_FREQ = isChannelA ? OFS_FREQ_A : OFS_FREQ_B;
    int modeShift = isChannelA ? 0 : 3;
    int valueShift = isChannelA ? 0 : 16;

    //clear previous settings
    iowrite32(ioread32(base + OFS_MODE) & ~(0x7 << modeShift), base + OFS_MODE);
    iowrite32(0, base + OFS_FREQ);
    iowrite32(ioread32(base + OFS_OFFSET) & ~(0xFFFF << valueShift), base + OFS_OFFSET);
    iowrite32(ioread32(base + OFS_AMPLITUDE) & ~(0xFFFF << valueShift), base + OFS_AMPLITUDE);
    iowrite32(ioread32(base + OFS_DTYCYC) & ~(0xFFFF << valueShift), base + OFS_DTYCYC);
    iowrite32(ioread32(base + OFS_PHASE_OFFS) & ~(0xFFFF << valueShift), base + OFS_PHASE_OFFS);

    //set new configuration
    iowrite32(ioread32(base + OFS_MODE) | (mode << modeShift), base + OFS_MODE);
    iowrite32(frequency, base + OFS_FREQ);
    iowrite32(ioread32(base + OFS_OFFSET) | ((uint16_t)offset << valueShift), base + OFS_OFFSET);
    iowrite32(ioread32(base + OFS_AMPLITUDE) | (amplitude << valueShift), base + OFS_AMPLITUDE);
    iowrite32(ioread32(base + OFS_DTYCYC) | (dutyCycle << valueShift), base + OFS_DTYCYC);
    iowrite32(ioread32(base + OFS_PHASE_OFFS) | ((uint16_t)phase_offs << valueShift), base + OFS_PHASE_OFFS);
}


void setCycles(char *channel, uint16_t cycles) 
{
    int isChannelA = strcasecmp(channel, "a") == 0;
    int valueShift = isChannelA ? 0 : 16;

    iowrite32(ioread32(base + OFS_CYCLES) & ~(0xFFFF << valueShift), base + OFS_CYCLES);
    iowrite32(ioread32(base + OFS_CYCLES) | (cycles << valueShift), base + OFS_CYCLES);
}


void configureRun() 
{
    iowrite32(RUN_A | RUN_B, base + OFS_RUN);
}

void configureStop() 
{
    iowrite32(~(RUN_A | RUN_B), base + OF
}

//-----------------------------------------------------------------------------
// Kernel Objects
//-----------------------------------------------------------------------------

// Enable 0
static bool channelA = 0;
module_param(channelA, bool, S_IRUGO);
MODULE_PARM_DESC(channelA, " Enable wavegen channel A");

static ssize_t enablechannelA_Store(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    if (strncmp(buffer, "true", strlen("true")) == 0) 
    {
        configureRun(); 
        channelA = true;
    } 
    else 
        if (strncmp(buffer, "false", strlen("false")) == 0) 
        {
            configureStop(); 
            channelA = false;
        }
    return count;
}

static ssize_t enablechannelA_Show(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    return sprintf(buffer, "%s\n", channelA ? "true" : "false");
}
