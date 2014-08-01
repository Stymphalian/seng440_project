define(`NOP', `nop ra39, ra39, ra39;  nop rb39, rb39, rb39')

## Move the uniforms (arguments) into registers
or ra31, ra32, 0;           nop         # address of twiddle in ra31
or ra30, ra32, 0;           nop         # address of io in ra30
#or ra29, ra32, 0;           nop         # stride 

## Load some rotation constants that don't fit in small immediates
ldi rb2, 0x1E; //30

## VCD DMA setup for the output vectors to load
# Assumes 16*8 twiddles 
# ID MODEW MPITCH ROWL NROW VPITCH H Addr
# 1    000   0010 1000 0000   0001 0 000 0000 0000
ldi ra49, 0x82801000

## Move the output vectors into the VPM (0,0 in VPM)
or ra50, ra30, 0;           nop

## Wait for the DMA to complete
and rb39, ra50, ra50;           nop

## Configure the VPM for reading the output vectors
ldi ra49, 0x801200

# This would likely need to be overloaded
## Read the twiddle vectors into registers ra20..ra27 
## Also copy them into rb20..rb27 (we need the original values to write back)
or ra20, ra48, 0;           v8max rb20, ra48, ra48;
or ra21, ra48, 0;           v8max rb21, ra48, ra48;
or ra22, ra48, 0;           v8max rb22, ra48, ra48;
or ra23, ra48, 0;           v8max rb23, ra48, ra48;
or ra24, ra48, 0;           v8max rb24, ra48, ra48;
or ra25, ra48, 0;           v8max rb25, ra48, ra48;
or ra26, ra48, 0;           v8max rb26, ra48, ra48;
or ra27, ra48, 0;           v8max rb27, ra48, ra48;


## FFT - unimplemented
#num_levels = log_2(n)
#W=current twiddle
#Z_k=butterfly
ldi ra2, 0x00 #level
levels:
	#num_blocks = n >> level
	
	outer_loop:
	#out = output + (segment << level)
	
	    inner_loop:
	    #Y_k = out[i]
	    #Z_k = out[i + (block_size >> 1)]
	    #W = twiddle[(n >> level) * i]
	
	    #temp_re = (W.re*Z_k.re >> 30) - W.im*Z_k.im >> 30);
	    #temp_im = (W.im*Z_k.re >> 30) + W.re*Z_k.im >> 30);				
				
	    #out[i].re = (Y_k.re) + (temp_re); 
	    #out[i].im = (Y_k.im) + (temp_im); 
				
	    #out[i + (block_size >>1 )].re = (Y_k.re) - (temp_re); 
	    #out[i + (block_size >>1 )].im = (Y_k.im) - (temp_im); 
	#block_size  = block_size << 1;		
						
NOP
NOP
NOP

## Configure the VPM to write the H vectors back into place
ldi rb49, 0x1200

# Again, would need to be overloaded or looped or something
## Write output vectors back (+=)
add rb48, ra20, rb20;       nop
add rb48, ra21, rb21;       nop
add rb48, ra22, rb22;       nop
add rb48, ra23, rb23;       nop
add rb48, ra24, rb24;       nop
add rb48, ra25, rb25;       nop
add rb48, ra26, rb26;       nop
add rb48, ra27, rb27;       nop

## Configure the VCD for DMA back to the host
ldi rb49, 0x88084000

## Write the output address to store
or rb50, ra30, 0;           nop

## Wait for the DMA to complete
or rb39, rb50, ra39;        nop

## Trigger a host interrupt to finish the program
or rb38, ra39, rb39;        nop

nop.tend ra39, ra39, ra39;  nop rb39, rb39, rb39
NOP
NOP
