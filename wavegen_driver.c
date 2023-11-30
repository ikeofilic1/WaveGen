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

#include <linux/init.h>     // __init
#include <linux/kernel.h>   // kstrtouint
#include <linux/kobject.h>  // kobject, kobject_atribute,
#include <linux/module.h>   // MODULE_ macros
                            // kobject_create_and_add, kobject_put
#include "../address_map.h" // overall memory map
#include "wavegen_regs.h"   // register offsets in QE IP
#include <asm/io.h>         // iowrite, ioread, ioremap_nocache (platform specific)

//-----------------------------------------------------------------------------
// Kernel module information
//-----------------------------------------------------------------------------

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jason Losh");
MODULE_DESCRIPTION("Wavegen IP Driver");

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

static unsigned uint32_t *base = NULL;

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

// Mode
void setMode(uint8_t mode)
{
    iowrite32(mode, base + OFS_MODE);
}

int32_t getMode(uint8_t channel)
{
    return ioread32(base + OFS_MODE);
}

// Run
void setRunning(uint8_t channel, bool running)
{
    uint32_t val = ioread32(base + OFS_RUN);
    uint8_t a = channel == 0 ? (val & RUN_A) : running;
    uint8_t b = channel == 1 ? (val & RUN_B) : running << 1;

    iowrite32(a | b, base + OFS_RUN);
}

bool isChannelRunning(uint8_t channel)
{
    return (ioread32(base + OFS_RUN) >> channel) & RUN_MASK;
}

// Freq A
void setfreqA(void)
{
    iowrite32(freqA, base + OFS_FREQ_A);
}

int32_t getfreqA(void)
{
    return ioread32(base + OFS_FREQ_A);
}

// Freq B
void setfreqB(void)
{
    iowrite32(freqB, base + OFS_FREQ_B);
}

int32_t getfreqB(void)
{
    return ioread32(base + OFS_FREQ_B);
}

// Offset A
void setOffsetA(uint16_t offsetA)
{
    iowrite32(offsetA, base + OFS_OFFSET);
}

uint32_t getOffsetA(void)
{
    return ioread32(base + OFS_OFFSET);
}

// Offset B
void setOffsetB(uint16_t offsetB)
{
    iowrite32(offsetB, base + OFS_OFFSET);
}

uint32_t getOffsetB(void)
{
    return ioread32(base + OFS_OFFSET);
}

// Amplitude A
void setAmplitudeA(uint32_t amplitudeA)
{
    iowrite32(amplitudeA, base + OFS_AMPLITUDE);
}

uint32_t getAmplitudeA(void)
{
    return ioread32(base + OFS_AMPLITUDE);
}

// Amplitude B
void setAmplitudeB(uint32_t amplitudeB)
{
    iowrite32(amplitudeB, base + OFS_AMPLITUDE);
}

uint32_t getAmplitudeB(void)
{
    return ioread32(base + OFS_AMPLITUDE);
}

// Duty Cycle A
void setDutyCycleA(uint32_t dutyCycleA)
{
    iowrite32(dutyCycleA, base + OFS_DUTYCYCLE_A);
}

uint32_t getDutyCycleA(void)
{
    return ioread32(base + OFS_DUTYCYCLE_A);
}

// Duty Cycle B
void setDutyCycleB(uint32_t dutyCycleB)
{
    iowrite32(dutyCycleB, base + OFS_DUTYCYCLE);
}

uint32_t getDutyCycleB(void)
{
    return ioread32(base + OFS_DUTYCYCLE);
}

// Cycle A
void setCycleA(uint32_t cycleA)
{
    iowrite32(cycleA, base + OFS_CYCLE_A);
}

uint32_t getCycleA(void)
{
    return ioread32(base + OFS_CYCLE_A);
}

// Cycle B
void setCycleB(uint32_t cycleB)
{
    iowrite32(cycleB, base + OFS_CYCLE);
}

uint32_t getCycleB(void)
{
    return ioread32(base + OFS_CYCLE);
}

// Phase Offset A
void setPhaseOffsetA(uint32_t phaseOffsetA)
{
    iowrite32(phaseOffsetA, base + OFS_PHASEOFFSET_A);
}

uint32_t getPhaseOffsetA(void)
{
    return ioread32(base + OFS_PHASEOFFSET_A);
}

// Phase Offset B
void setPhaseOffsetB(uint32_t phaseOffsetB)
{
    iowrite32(phaseOffsetB, base + OFS_PHASEOFFSET);
}

uint32_t getPhaseOffsetB(void)
{
    return ioread32(base + OFS_PHASEOFFSET);
}

//-----------------------------------------------------------------------------
// Kernel Objects
//-----------------------------------------------------------------------------

// Mode
static uint8_t modeA = 0;
module_param(modeA, uint8_t, S_IRUGO);
MODULE_PARM_DESC(modeA, "Channel A mode");

static ssize_t modeAStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &modeA);
    if (result == 0)
        setMode(1, modeA);
    return count;
}

static ssize_t modeAShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    modeA = getMode(0);
    return sprintf(buffer, "%u\n", modeA);
}

static struct kobj_attribute modeBAttr = __ATTR(modeB, 0664, modeBShow, modeBStore);

// Amplitude
static uint32_t amplitudeA = 0;
module_param(amplitudeA, uint, S_IRUGO);
MODULE_PARM_DESC(amplitudeA, "Amplitude setting");

static ssize_t amplitudeAStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &amplitudeA);
    if (result == 0)
        setAmplitudeA(amplitudeA);
    return count;
}

static ssize_t amplitudeAShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    amplitudeA = getAmplitudeA(0);
    return sprintf(buffer, "%u\n", amplitudeA);
}

static struct kobj_attribute amplitudeAttr = __ATTR(amplitudeA, 0664, amplitudeAShow, amplitudeAStore);

// Duty Cycle
static uint32_t dutyCycleA = 0;
module_param(dutyCycleA, uint, S_IRUGO);
MODULE_PARM_DESC(dutyCycleA, "Duty cycle setting");

static ssize_t dutyCycleAStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &dutyCycleA);
    if (result == 0)
        setDutyCycleA(dutyCycleA);
    return count;
}

static ssize_t dutyCycleAShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    dutyCycleA = getDutyCycleA(0);
    return sprintf(buffer, "%u\n", dutyCycleA);
}

static struct kobj_attribute dutyCycleAAttr = __ATTR(dutyCycleA, 0664, dutyCycleAShow, dutyCycleAStore);

// Cycle
static uint32_t cycleA = 0;
module_param(cycleA, uint, S_IRUGO);
MODULE_PARM_DESC(cycleA, "Cycle setting");

static ssize_t cycleAStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &cycleA);
    if (result == 0)
        setCycleA(cycleA);
    return count;
}

static ssize_t cycleAShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    cycleA = getCycleA(0);
    return sprintf(buffer, "%u\n", cycleA);
}

static struct kobj_attribute cycleAttr = __ATTR(cycleA, 0664, cycleAShow, cycleAStore);

// Phase Offset
static uint32_t phaseOffsetA = 0;
module_param(phaseOffsetA, uint, S_IRUGO);
MODULE_PARM_DESC(phaseOffsetA, "Phase offset setting");

static ssize_t phaseOffsetAStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &phaseOffsetA);
    if (result == 0)
        setPhaseOffsetA(phaseOffsetA);
    return count;
}

static ssize_t phaseOffsetAShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    phaseOffsetA = getPhaseOffsetA(0);
    return sprintf(buffer, "%u\n", phaseOffsetA);
}

static struct kobj_attribute phaseOffsetAttr = __ATTR(phaseOffsetA, 0664, phaseOffsetAShow, phaseOffsetAStore);

// Mode
static uint8_t modeB = 0;
module_param(modeB, uint8_t, S_IRUGO);
MODULE_PARM_DESC(modeB, "Channel B mode");

static ssize_t modeBStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &modeB);
    if (result == 0)
        setMode(1, modeB);
    return count;
}

static ssize_t modeBShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    modeB = getMode(1);
    return sprintf(buffer, "%u\n", modeB);
}

static struct kobj_attribute modeBAttr = __ATTR(modeB, 0664, modeBShow, modeBStore);

// Amplitude
static uint32_t amplitudeB = 0;
module_param(amplitudeB, uint, S_IRUGO);
MODULE_PARM_DESC(amplitudeB, "Amplitude setting");

static ssize_t amplitudeStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &amplitudeB);
    if (result == 0)
        setAmplitude(amplitudeB);
    return count;
}

static ssize_t amplitudeShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    amplitude = getAmplitude(1);
    return sprintf(buffer, "%u\n", amplitudeB);
}

static struct kobj_attribute amplitudeAttr = __ATTR(amplitudeB, 0664, amplitudeBShow, amplitudeBStore);

// Duty Cycle
static uint32_t dutyCycleB = 0;
module_param(dutyCycleB, uint, S_IRUGO);
MODULE_PARM_DESC(dutyCycleB, "Duty cycle setting");

static ssize_t dutyCycleBStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &dutyCycleB);
    if (result == 0)
        setDutyCycle(1, dutyCycleB);
    return count;
}

static ssize_t dutyCycleBShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    dutyCycle = getDutyCycle(1);
    return sprintf(buffer, "%u\n", dutyCycleB);
}

static struct kobj_attribute dutyCycleAttr = __ATTR(dutyCycleB, 0664, dutyCycleBShow, dutyCycleBStore);

// Cycle
static uint32_t cycleB = 0;
module_param(cycleB, uint, S_IRUGO);
MODULE_PARM_DESC(cycleB, "Cycle setting");

static ssize_t cycleBStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &cycle);
    if (result == 0)
        setCycle(cycle);
    return count;
}

static ssize_t cycleBShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    cycle = getCycle(1);
    return sprintf(buffer, "%u\n", cycle);
}

static struct kobj_attribute cycleAttr = __ATTR(cycleB, 0664, cycleBShow, cycleBStore);

// Phase Offset
static uint32_t phaseOffsetB = 0;
module_param(phaseOffsetB, uint, S_IRUGO);
MODULE_PARM_DESC(phaseOffsetB, "Phase offset setting");

static ssize_t phaseOffsetStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &phaseOffsetB);
    if (result == 0)
        setPhaseOffset(phaseOffsetB);
    return count;
}

static ssize_t phaseOffsetBShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    phaseOffset = getPhaseOffset();
    return sprintf(buffer, "%u\n", phaseOffset);
}

static struct kobj_attribute phaseOffsetAttr = __ATTR(phaseOffsetB, 0664, phaseOffsetBShow, phaseOffsetBStore);

// Attributes
static struct attribute *wavegenAAttrs[] = {&modeAAttr.attr, &freqAAttr.attr, &freqAAttr.attr, &amplitudeAAttr.attr, &dutyCycleAAttr.attr, &cycleAAttr.attr, &phaseOffsetAAttr.attr, NULL};
static struct attribute *wavegenBAttrs[] = {&modeAAttr.attr, &freqBAttr.attr, &freqBAttr.attr, &amplitudeBAttr.attr, &dutyCycleBAttr.attr, &cycleBAttr.attr, &phaseOffsetBAttr.attr, NULL};

// clang-format off
static struct attribute_group channelA =
{
    .name = "a",
    .attrs = wavegenAAttrs
};

static struct attribute_group channelB =
{
    .name = "b",
    .attrs = wavegenBAttrss
};
// clang-format on

static struct kobject *kobj;

//-----------------------------------------------------------------------------
// Initialization and Exit
//-----------------------------------------------------------------------------

static int __init initialize_module(void)
{
    int result;

    printk(KERN_INFO "Wavegen driver: starting\n");

    // Create Wavegen directory under /sys/kernel
    kobj = kobject_create_and_add("wavegen", NULL); // kernel_kobj);
    if (!kobj)
    {
        printk(KERN_ALERT "Wavegen driver: failed to create and add kobj\n");
        return -ENOENT;
    }

    // Create Wavegen groups
    result = sysfs_create_group(kobj, &channelA);
    if (result != 0)
    {
        printk(KERN_ALERT "Wavegen driver: failed to create sysfs group\n");
        kobject_put(kobj);
        return result;
    }

    result = sysfs_create_group(kobj, &channelB);
    if (result != 0)
    {
        printk(KERN_ALERT "Wavegen driver: failed to create sysfs group\n");
        kobject_put(kobj);
        return result;
    }

    // Physical to virtual memory map to access gpio registers
    base = (unsigned int *)ioremap(AXI4_LITE_BASE + WAVEGEN_BASE_OFFSET,
                                   SPAN_IN_BYTES);
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
