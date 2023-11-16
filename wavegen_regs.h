// WAVEGEN IP Library Registers

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: DE1-SoC Board
// Target uC:       -
// System Clock:    -

// Hardware configuration:
// GPIO IP core connected to light-weight Avalon bus

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#ifndef WAVEGEN_REGS_H_
#define WAVEGEN_REGS_H_

#define OFS_MODE         0
#define OFS_RUN          4
#define OFS_FREQ_A       8
#define OFS_FREQ_B      12
#define OFS_OFFSET      16
#define OFS_AMPLITUDE   20
#define OFS_DTYCYC      24
#define OFS_CYCLES      28
#define OFS_PHASE_OFFS  32

// #define MODE_MASK        
// #define RUN_MASK          4
// #define OFFSET_MASK      16
// #define AMPLITUDE_MASK   20
// #define DTYCYC_MASK      24
// #define CYCLES_MASK      28
// #define PHASE_OFFS_MASK  32

#define RUN_A  (0x1 << 0)
#define RUN_B  (0x1 << 1)


#define SPAN_IN_BYTES 36

#endif

