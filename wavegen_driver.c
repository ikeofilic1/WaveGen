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
MODULE_AUTHOR("Girl and Boy together forever");
MODULE_DESCRIPTION("Wavegen IP Driver");

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

#define MODE_DC         0
#define MODE_SINE       1
#define MODE_SAWTOOTH   2
#define MODE_TRIANGLE   3
#define MODE_SQUARE     4
#define MODE_ARB        5

static uint32_t *base = NULL;

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

// Mode
void setMode(uint8_t channel, uint8_t mode)
{
    uint32_t val = ioread32(base + OFS_MODE) & ~(MMODE_MASK << channel*3);
    val |= (mode << channel*3);

    iowrite32(val, base + OFS_MODE);
}

uint8_t getMode(uint8_t channel)
{
    return (ioread32(base + OFS_MODE) >> channel*3) ;
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
void setfreqA(uint32_t freqA)
{
    iowrite32(freqA, base + OFS_FREQ_A);
}

int32_t getfreqA(void)
{
    return ioread32(base + OFS_FREQ_A);
}

// Freq B
void setfreqB(uint32_t freqB)
{
    iowrite32(freqB, base + OFS_FREQ_B);
}

int32_t getfreqB(void)
{
    return ioread32(base + OFS_FREQ_B);
}

// Offset A
void setOffsetA(uint8_t channel, int32_t offsetA)
{
    int shift = channel ? 16 : 0;

    uint32_t val = ioread32(base + OFS_OFFSET) & ~(OFFSET_MASK << shift);
    val |= (offsetA << shift);

    printk(KERN_INFO "Writing %d to %p (offset)", val, base + OFS_OFFSET);
    iowrite32(val, base + OFS_OFFSET);
}

uint16_t getOffsetA(uint8_t channel)
{
    return (ioread32(base + OFS_OFFSET) >> channel*16);
}

// Amplitude A
void setAmplitude(uint8_t channel, uint32_t amplitude)
{
    int shift = channel ? 16 : 0;
    uint32_t val = ioread32(base + OFS_AMPLITUDE) & ~(AMPLITUDE_MASK << shift);
    val |= (amplitude << shift);

    printk(KERN_INFO "Writing %d to %p (amplitude)", val, base + OFS_AMPLITUDE);
    iowrite32(val, base + OFS_AMPLITUDE);
}

uint16_t getAmplitude(uint8_t channel)
{
    return (ioread32(base + OFS_AMPLITUDE) >> channel*16);
}

// Duty Cycle A
void setDutyCycle(uint8_t channel, uint32_t dutyCycle)
{
    int shift = channel ? 16 : 0;
    uint32_t val = ioread32(base + OFS_DTYCYC) & ~(DTYCYC_MASK << shift);
    val |= (dutyCycle << shift);
    iowrite32(val, base + OFS_DTYCYC);
}

uint16_t getDutyCycle(uint8_t channel)
{
    return ioread32(base + OFS_DTYCYC) >> channel*16;
}

// Cycle A
void setCycle(uint8_t channel, uint16_t cycle)
{
    int shift = channel ? 16 : 0;
    uint32_t val = ioread32(base + OFS_CYCLES) & ~(DTYCYC_MASK << shift);
    val |= (cycle << shift);
    iowrite32(val, base + OFS_CYCLES);
}

uint16_t getCycle(uint8_t channel)
{
    return ioread32(base + OFS_CYCLES) >> channel*16;
}

// Phase Offset A
void setPhaseOffset(uint8_t channel, int16_t phaseOffset)
{
    int shift = channel ? 16 : 0;
    uint32_t val = ioread32(base + OFS_PHASE_OFFS) & ~(DTYCYC_MASK << shift);
    val |= (phaseOffset << shift);

    iowrite32(val, base + OFS_PHASE_OFFS);
}

int16_t getPhaseOffset(uint8_t channel)
{
    return (ioread32(base + OFS_PHASE_OFFS) >> channel*16);
}


//-----------------------------------------------------------------------------
// Kernel Objects
//-----------------------------------------------------------------------------

const char *mode_map[] = {
    [MODE_DC] = "dc",
    [MODE_SINE] = "sine",
    [MODE_SAWTOOTH] = "sawtooth",
    [MODE_TRIANGLE] = "triangle",
    [MODE_SQUARE] = "triangle",
    [MODE_ARB] = "arb"
};

#define MAP_SIZE (sizeof(mode_map)/sizeof(mode_map[0]))

// Mode
static uint32_t modeA = 0;
module_param(modeA, uint, S_IRUGO);
MODULE_PARM_DESC(modeA, "Channel A mode");

static ssize_t modeAStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int i = 0;
    for (; i < MAP_SIZE; ++i)
    {
        if (strncmp(buffer, mode_map[i], strlen(mode_map[i])) == 0) {
            modeA = i;
            break;
        }
    }

    if (i < MAP_SIZE)
        setMode(0, modeA);
    return count;
}

static ssize_t modeAShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    modeA = getMode(0);
    strcpy(buffer, mode_map[modeA]); //use snprintf!!

    return strlen(buffer);
}

static struct kobj_attribute modeAAttr = __ATTR(modeA, 0664, modeAShow, modeAStore);

// Frequency A
static uint32_t runA = 0;
module_param(runA, uint, S_IRUGO);
MODULE_PARM_DESC(runA, "Frequency A");


static ssize_t runAStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &runA);
    if (result == 0)
        setfreqA(runA);
    return count;
}

static ssize_t runAShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    runA = getfreqA();
    return sprintf(buffer, "%u\n", runA);
}
static struct kobj_attribute Attr = __ATTR(run, 0664, runAShow, runAStore);


// Frequency A
static uint32_t freqA = 0;
module_param(freqA, uint, S_IRUGO);
MODULE_PARM_DESC(freqA, "Frequency A");


static ssize_t freqAStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &freqA);
    if (result == 0)
        setfreqA(freqA);
    return count;
}

static ssize_t freqAShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    freqA = getfreqA();
    return sprintf(buffer, "%u\n", freqA);
}

static struct kobj_attribute freqAAttr = __ATTR(freqA, 0664, freqAShow, freqAStore);

// Offset
static int32_t offsetA = 0;
module_param(offsetA, int, S_IRUGO);
MODULE_PARM_DESC(offsetA, "Amplitude setting");

static ssize_t offsetAStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtoint(buffer, 0, &offsetA);
    if (result == 0)
        setOffsetA(0, offsetA);
    return count;
}

static ssize_t offsetAShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    offsetA = getOffsetA(0);
    return sprintf(buffer, "%d\n", offsetA);
}

static struct kobj_attribute offsetAAttr = __ATTR(offsetA, 0664, offsetAShow, offsetAStore);

// Amplitude
static uint32_t amplitudeA = 0;
module_param(amplitudeA, uint, S_IRUGO);
MODULE_PARM_DESC(amplitudeA, "Amplitude setting");

static ssize_t amplitudeAStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &amplitudeA);
    if (result == 0)
        setAmplitude(0, amplitudeA);
    return count;
}

static ssize_t amplitudeAShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    amplitudeA = getAmplitude(0);
    return sprintf(buffer, "%u\n", amplitudeA);
}

static struct kobj_attribute amplitudeAAttr = __ATTR(amplitudeA, 0664, amplitudeAShow, amplitudeAStore);

// Duty Cycle
static uint32_t dutyCycleA = 0;
module_param(dutyCycleA, uint, S_IRUGO);
MODULE_PARM_DESC(dutyCycleA, "Duty cycle setting");

static ssize_t dutyCycleAStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &dutyCycleA);
    if (result == 0)
        setDutyCycle(0, (dutyCycleA*65535)/100);
    return count;
}

static ssize_t dutyCycleAShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    dutyCycleA = getDutyCycle(0);
    return sprintf(buffer, "%u\n", (dutyCycleA*100)/65535);
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
        setCycle(0, cycleA);
    return count;
}

static ssize_t cycleAShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    cycleA = getCycle(0);
    return sprintf(buffer, "%u\n", cycleA);
}

static struct kobj_attribute cycleAAttr = __ATTR(cycleA, 0664, cycleAShow, cycleAStore);

// Phase Offset
static uint32_t phaseOffsetA = 0;
module_param(phaseOffsetA, uint, S_IRUGO);
MODULE_PARM_DESC(phaseOffsetA, "Phase offset setting");

static ssize_t phaseOffsetAStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &phaseOffsetA);
    if (result == 0)
        setPhaseOffset(0, phaseOffsetA*100);
    return count;
}

static ssize_t phaseOffsetAShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    phaseOffsetA = getPhaseOffset(0);
    return sprintf(buffer, "%u degrees\n", phaseOffsetA/100);
}

static struct kobj_attribute phaseOffsetAAttr = __ATTR(phaseOffsetA, 0664, phaseOffsetAShow, phaseOffsetAStore);

// Mode
static uint32_t modeB = 0;
module_param(modeB, uint, S_IRUGO);
MODULE_PARM_DESC(modeB, "Channel B mode");

static ssize_t modeBStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int i = 0;
    for (; i < MAP_SIZE; ++i)
    {
        if (strncmp(buffer, mode_map[i], strlen(mode_map[i])) == 0) {
            modeB = i;
            break;
        }
    }

    if (i < MAP_SIZE)
        setMode(1, modeB);
    return count;
}

static ssize_t modeBShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    modeB = getMode(1);
    strcpy(buffer, mode_map[modeB]);

    return strlen(buffer);
}

static struct kobj_attribute modeBAttr = __ATTR(modeB, 0664, modeBShow, modeBStore);

// Frequency B
static uint32_t freqB = 0;
module_param(freqB, uint, S_IRUGO);
MODULE_PARM_DESC(freqB, "Frequency B setting");

static ssize_t freqBStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &freqB);
    if (result == 0)
        setfreqB(freqB);
    return count;
}

static ssize_t freqBShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    freqB = getfreqB();
    return sprintf(buffer, "%u\n", freqB);
}

static struct kobj_attribute freqBAttr = __ATTR(freqB, 0664, freqBShow, freqBStore);

// Offset
static int32_t offsetB = 0;
module_param(offsetB, int, S_IRUGO);
MODULE_PARM_DESC(offsetB, "Amplitude setting");

static ssize_t offsetBStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtoint(buffer, 0, &offsetB);
    if (result == 0)
        setOffsetA(1, offsetB);
    return count;
}

static ssize_t offsetBShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    offsetB = getOffsetA(1);
    return sprintf(buffer, "%d\n", offsetB);
}

static struct kobj_attribute offsetBAttr = __ATTR(offsetB, 0664, offsetBShow, offsetBStore);

// Amplitude
static uint32_t amplitudeB = 0;
module_param(amplitudeB, uint, S_IRUGO);
MODULE_PARM_DESC(amplitudeB, "Amplitude setting");

static ssize_t amplitudeBStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &amplitudeB);
    if (result == 0)
        setAmplitude(1, amplitudeB);
    return count;
}

static ssize_t amplitudeBShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    amplitudeB = getAmplitude(1);
    return sprintf(buffer, "%u\n", amplitudeB);
}

static struct kobj_attribute amplitudeBAttr = __ATTR(amplitudeB, 0664, amplitudeBShow, amplitudeBStore);

// Duty Cycle
static uint32_t dutyCycleB = 0;
module_param(dutyCycleB, uint, S_IRUGO);
MODULE_PARM_DESC(dutyCycleB, "Duty cycle setting");

static ssize_t dutyCycleBStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &dutyCycleB);
    if (result == 0)
        setDutyCycle(1, dutyCycleB*65535/100);
    return count;
}

static ssize_t dutyCycleBShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    dutyCycleB = getDutyCycle(1);
    return sprintf(buffer, "%u\n", dutyCycleB*100/65535);
}

static struct kobj_attribute dutyCycleBAttr = __ATTR(dutyCycleB, 0664, dutyCycleBShow, dutyCycleBStore);

// Cycle
static uint32_t cycleB = 0;
module_param(cycleB, uint, S_IRUGO);
MODULE_PARM_DESC(cycleB, "Cycle setting");

static ssize_t cycleBStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &cycleB);
    if (result == 0)
        setCycle(1, cycleB);
    return count;
}

static ssize_t cycleBShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    cycleB = getCycle(1);
    return sprintf(buffer, "%u\n", cycleB);
}

static struct kobj_attribute cycleBAttr = __ATTR(cycleB, 0664, cycleBShow, cycleBStore);

// Phase Offset
static uint32_t phaseOffsetB = 0;
module_param(phaseOffsetB, uint, S_IRUGO);
MODULE_PARM_DESC(phaseOffsetB, "Phase offset setting");

static ssize_t phaseOffsetBStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &phaseOffsetB);
    if (result == 0)
        setPhaseOffset(1, phaseOffsetB*100);
    return count;
}

static ssize_t phaseOffsetBShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    phaseOffsetB = getPhaseOffset(1);
    return sprintf(buffer, "%u\n", phaseOffsetB/100);
}

static struct kobj_attribute phaseOffsetBAttr = __ATTR(phaseOffsetB, 0664, phaseOffsetBShow, phaseOffsetBStore);

// Attributes
static struct attribute *wavegenAAttrs[] = {&modeAAttr.attr, &Attr.attr, &freqAAttr.attr, &offsetAAttr.attr, &amplitudeAAttr.attr, &dutyCycleAAttr.attr, &cycleAAttr.attr, &phaseOffsetAAttr.attr, NULL};
static struct attribute *wavegenBAttrs[] = {&modeBAttr.attr, &Attr.attr, &freqBAttr.attr, &offsetBAttr.attr, &amplitudeBAttr.attr, &dutyCycleBAttr.attr, &cycleBAttr.attr, &phaseOffsetBAttr.attr, NULL};

// clang-format off
static struct attribute_group channelA =
{
    .name = "a",
    .attrs = wavegenAAttrs
};

static struct attribute_group channelB =
{
    .name = "b",
    .attrs = wavegenBAttrs
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
