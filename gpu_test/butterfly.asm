#   Perform 2-point butterfly on the qpu
#
#   Inputs:             Output
#   A(0) -------------- X(0) = A(0) + W * B(0)
#             X 
#   B(0) -------------- X(1) = A(0) - W * B(0)
#
#   Twiddle -> W_N = e^(-j*2*M_PI/2)=W_N/2 ; N = 2
#
#   Uniforms:
#	A_re	- 32bit,signed
#	A_im	- 32bit,signed
#	B_re	- 32bit,signed
#	B_im	- 32bit,signed
#	Result	- address to X0 location

#----------------------Load/Store VPM Registers--------------------------------
# VPM_BLOCK_WRITE_SETUP (rb49)
# ~~~~~~~~~~~~~~~~~~~~~
# Sets up things so writes go into the small VPM data cache.
# Once the data's been written (by outputting repeatedly to the VPM_WRITE_FIFO
# register rb48), you then call VPM_DMA_WRITE_SETUP to configure the main
# memory destination and writing pattern.
#  STRIDE: 0-64 - How much to increment the ADDR after each write.
#  HORIZ: 0 or 1 - Whether the layout is horizontal (1) or vertical (0).
#  LANED: 0 or 1 - Whether the layout is laned (1) or packed (0).
#  SIZE: 0, 1, 2 - The data unit size, 8-bit (0), 16-bit(1), or 32-bit (2).
#  ADDR: 0-255 - Packed address, meaning depends on exact unit size and mode.
# See http://www.broadcom.com/docs/support/videocore/VideoCoreIV-AG100-R.pdf page 57

# VPM_BLOCK_READ_SETUP (rb49)
# ~~~~~~~~~~~~~~~~~~~~
# Controls how values are read from the VPM data cache into the QPU.
# Arguments:
#  NUM: 0-16 - How many elements to read at a time.
#  STRIDE: 0-64 - The amount to increment the address by after each read.
#  HORIZ: 0 or 1 - Whether the layour is horizontal (1) or vertical (0).
#  LANED: 0 or 1 - Whether the layout is laned (1) or packed (0).
#  SIZE: 0, 1, 2 - The data unit size, 8-bit (0), 16-bit(1), or 32-bit (2).
#  ADDR: 0-255 - Packed address, meaning depends on exact unit size and mode.
# See http://www.broadcom.com/docs/support/videocore/VideoCoreIV-AG100-R.pdf page 58

# VPM_DMA_STORE_START (rb50)
# ~~~~~~~~~~~~~~~~~~~
# Kicks off the transfer of data from the local VPM data cache to main memory.
# It will use the settings from VPM_DMA_STORE_SETUP to control the copy process.
# Arguments:
#  address: A register name that holds the address in main memory to write to.

# VPM_DMA_LOAD_SETUP (ra49)
# ~~~~~~~~~~~~~~~~~~
# Initializes the settings for transfering data from main memory into the VPM cache.
# Arguments:
#  MODEW: 0-7 : 0 is 32-bit, 2-3 is 16-bit with offset, 4-7 is 8-bit with offset.
#  MPITCH: 0-15: The amount to increment the memory pointer between rows, calculated as 8*2^MPITCH bytes.
#  ROWLEN: 0-15: The number of elements in each row in main memory.
#  NROWS: 0-15: How many rows to read from memory.
#  VPITCH: 0-15: How much to increment the VPM address by after each row is loaded.
#  VERT: 0 or 1 - Whether the layout is vertical (1) or horizontal (0). Be careful, this is inverted compared to normal.
#  ADDRY: 0-64 - The Y coordinate of the address in the VPM space to start loading into.
#  ADDRX: 0-16 - The X coordinate of the address in the VPM space to start loading into.

# VPM_DMA_STORE_WAIT_FOR_COMPLETION
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Pause until the previous DMA store operation has finished.
define(`VPM_DMA_STORE_WAIT_FOR_COMPLETION', `or rb39, rb50, rb50;       nop')

# END_PROGRAM
# ~~~~~~~~~~~
# Triggers a host interrupt to transfer control back to the main CPU.
define(`END_PROGRAM_HARD', `
or rb38, r0, 1;       nop
nop.tend ra39, ra39, ra39;       nop rb39, rb39, rb39
nop ra39, ra39, ra39;       nop rb39, rb39, rb39
nop ra39, ra39, ra39;       nop rb39, rb39, rb39')

#-------------------Defines-------------------------------------------------
define(`NOP', `nop ra39, ra39, ra39;       nop rb39, rb39, rb39')

# Hardwired IO registers
define(`rVpmWriteFifo', `rb48')
define(`rVpmReadFifo', `ra48')
define(`raReadUniform', `ra32')
define(`rbReadUniform', `rb32')

# Registers used to hold uniforms
define(`rAr', ra0)
define(`rAi', ra1)
define(`rBr', ra2)
define(`rBi', ra3)
define(`rResult', ra4)

# Registers used to hold working values
define(`W', ra5)
define(`Y', ra6)
define(`Z', ra7)

# The special accumulator registers
define(`rAccum0', r0)
define(`rAccum1', r1)
define(`rAccum2', r2)
define(`rTotal', r3)

# Load uniform arguments
or rAr, raReadUniform, 0; nop
or rAi, raReadUniform, 0; nop
or rBr, raReadUniform, 0; nop
or rBi, raReadUniform, 0; nop
or rResult, raReadUniform, 0; nop

# Configure the VPM for writing
#  ID 31=0: generic block write setup
#  STRIDE: 17:12=010 0000: Increment the addr by 32 after each write.
#  HORIZ: 11=1: layout is horizontal
#  LANED: 10=0: layout is packed
#  SIZE: 9:8=10: 32-bit data unit size.
#  ADDRXY: 7:0 - Packed address, meaning depends on exact unit size and mode.
ldi rb49, 0xa00 

# For Testing, add 1 to the current value and stick
# it in the VPM data cache we set up in the previous
# step - this would be replaced with the radix 2 eqns
ldi ra8, 0x1111
add rVpmWriteFifo, ra8, 0x0000;  nop
add rVpmWriteFifo, ra8, 0x0001;  nop
add rVpmWriteFifo, ra8, 0x0002;  nop
add rVpmWriteFifo, ra8, 0x0003;  nop

## move 16 words (1 vector) back to the host (DMA)
#  ID: 31=1: VDR basic setup 
#  MODEW: 30:28=000:  width=32-bit
#  MPITCH: 27:24=1000:  memory ptr += 8*2^8=2048 bytes every row.
#  ROWLEN: 23:20=0000: 16 elements per row 
#  NROWS: 19:16=0100: 4 rows to read 
#  VPITCH: 15:12=1000: increment the VPM address by 32 after each row is loaded.
#  VERT: 11=0: Layout is horizontal (0). 
#  ADDRXY: 10:0: Start at (0,0) 
ldi rb49, 0x88048000

## Sends data from VPM data cache (rb48) to DMA
or rb50, rResult, 0;          nop

# Wait for the DMA to complete
VPM_DMA_STORE_WAIT_FOR_COMPLETION

END_PROGRAM_HARD



