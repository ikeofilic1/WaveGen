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

#define SPAN_IN_BYTES 32

#endif

