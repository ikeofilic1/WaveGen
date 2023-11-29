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
#include "wavegen_regs.h"     // register offsets in QE IP

//-----------------------------------------------------------------------------
// Kernel module information
//-----------------------------------------------------------------------------

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jason Losh");
MODULE_DESCRIPTION("Wavegen IP Driver");

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
// Kernel Objects
//-----------------------------------------------------------------------------

// Mode
static uint32_t mode = 0;
module_param(mode, int, S_IRUGO);
MODULE_PARM_DESC(mode, "Waveform generator mode");

static ssize_t modeStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &mode);
    if (result == 0)
        setMode(mode);
    return count;
}

static ssize_t modeShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    mode = getMode();
    return sprintf(buffer, "%u\n", mode);
}

static struct kobj_attribute modeAttr = __ATTR(mode, 0664, modeShow, modeStore);

// Frequency A
static uint32_t freqA = 0;
module_param(freqA, uint, S_IRUGO);
MODULE_PARM_DESC(freqA, "Frequency A");

static ssize_t freqAStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &freqA);
    if (result == 0)
        setFreqA(freqA);
    return count;
}

static ssize_t freqAShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    freqA = getFreqA();
    return sprintf(buffer, "%u\n", freqA);
}

static struct kobj_attribute freqAAttr = __ATTR(freqA, 0664, freqAShow, freqAStore);

// Frequency B
static uint32_t freqB = 0;
module_param(freqB, uint, S_IRUGO);
MODULE_PARM_DESC(freqB, "Frequency B setting");

static ssize_t freqBStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &freqB);
    if (result == 0)
        setFreqB(freqB);
    return count;
}

static ssize_t freqBShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    freqB = getFreqB();
    return sprintf(buffer, "%u\n", freqB);
}

static struct kobj_attribute freqBAttr = __ATTR(freqB, 0664, freqBShow, freqBStore);

// Amplitude
static uint32_t amplitude = 0;
module_param(amplitude, uint, S_IRUGO);
MODULE_PARM_DESC(amplitude, "Amplitude setting");

static ssize_t amplitudeStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &amplitude);
    if (result == 0)
        setAmplitude(amplitude);
    return count;
}

static ssize_t amplitudeShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    amplitude = getAmplitude();
    return sprintf(buffer, "%u\n", amplitude);
}

static struct kobj_attribute amplitudeAttr = __ATTR(amplitude, 0664, amplitudeShow, amplitudeStore);

// Duty Cycle
static uint32_t dutyCycle = 0;
module_param(dutyCycle, uint, S_IRUGO);
MODULE_PARM_DESC(dutyCycle, "Duty cycle setting");

static ssize_t dutyCycleStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &dutyCycle);
    if (result == 0)
        setDutyCycle(dutyCycle);
    return count;
}

static ssize_t dutyCycleShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    dutyCycle = getDutyCycle();
    return sprintf(buffer, "%u\n", dutyCycle);
}

static struct kobj_attribute dutyCycleAttr = __ATTR(dutyCycle, 0664, dutyCycleShow, dutyCycleStore);

// Cycle
static uint32_t cycle = 0;
module_param(cycle, uint, S_IRUGO);
MODULE_PARM_DESC(cycle, "Cycle setting");

static ssize_t cycleStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &cycle);
    if (result == 0)
        setCycle(cycle);
    return count;
}

static ssize_t cycleShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    cycle = getCycle();
    return sprintf(buffer, "%u\n", cycle);
}

static struct kobj_attribute cycleAttr = __ATTR(cycle, 0664, cycleShow, cycleStore);

// Phase Offset
static uint32_t phaseOffset = 0;
module_param(phaseOffset, uint, S_IRUGO);
MODULE_PARM_DESC(phaseOffset, "Phase offset setting");

static ssize_t phaseOffsetStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &phaseOffset);
    if (result == 0)
        setPhaseOffset(phaseOffset);
    return count;
}

static ssize_t phaseOffsetShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    phaseOffset = getPhaseOffset();
    return sprintf(buffer, "%u\n", phaseOffset);
}

static struct kobj_attribute phaseOffsetAttr = __ATTR(phaseOffset, 0664, phaseOffsetShow, phaseOffsetStore);


// Attributes
static struct attribute *wavegenAttrs[] = {&modeAttr.attr, &freqAAttr.attr, &freqBAttr.attr, &amplitudeAttr.attr, &dutyCycleAttr.attr, &cycleAttr.attr, &phaseOffsetAttr.attr, NULL};

static struct attribute_group wavegenGroup =
{
    .name = "wavegen",
    .attrs = wavegenAttrs
};

static struct kobject *kobj;

//-----------------------------------------------------------------------------
// Initialization and Exit
//-----------------------------------------------------------------------------

static int __init initialize_module(void)
{
    int result;

    printk(KERN_INFO "Wavegen driver: starting\n");

    // Create Wavegen directory under /sys/kernel
    kobj = kobject_create_and_add("Wavegen", NULL); //kernel_kobj);
    if (!kobj)
    {
        printk(KERN_ALERT "Wavegen driver: failed to create and add kobj\n");
        return -ENOENT;
    }

    // Create Wavegen groups
    result = sysfs_create_group(kobj, &wavegenGroup);
    if (result !=0)
    {
        printk(KERN_ALERT "Wavegen driver: failed to create sysfs group\n");
        kobject_put(kobj);
        return result;
    }

    // Physical to virtual memory map to access gpio registers
    base = (unsigned int*)ioremap(AXI4_LITE_BASE + QE_BASE_OFFSET,
                                          WAVEGEN_SPAN_IN_BYTES);
    if (base == NULL)
        return -ENODEV;

    printk(KERN_INFO "Wavegen driver: initialized\n");

    return 0;
}

static void __exit exit_module(void)
{
    kobject_put(kobj);
    printk(KERN_INFO "Wavegen driver: exit\n");
}

module_init(initialize_module);
module_exit(exit_module);

