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

#define OFS_MODE        0
#define OFS_RUN         1
#define OFS_FREQ_A      2
#define OFS_FREQ_B      3
#define OFS_OFFSET      4
#define OFS_AMPLITUDE   5
#define OFS_DTYCYC      6
#define OFS_CYCLES      7
#define OFS_PHASE_OFFS  8


#define MMODE_MASK       0x7
#define RUN_MASK        0x1
#define RUN_A          (RUN_MASK << 0)
#define RUN_B          (RUN_MASK << 1)
#define OFFSET_MASK     0xFF
#define AMPLITUDE_MASK  0xFF
#define DTYCYC_MASK     0xFF
#define CYCLES_MASK     0xFF
#define PHASE_OFFS_MASK 0xFF
#define OFFSET_MASK     0xFF

#define SPAN_IN_BYTES 36

#endif

