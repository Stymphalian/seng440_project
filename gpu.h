#ifndef GPU_H
#define GPU_H

#define NUM_QPUS        12
#define MAX_CODE_SIZE   8192

int fft_setup_qpu(complex_t* input, complex_t *output, complex_t* twiddles, unsigned N,
                   unsigned *shader_code, unsigned code_len);
int load_qpu_code(const char *fname, unsigned int* buffer, int len);
void fft_cleanup_qpu(int handle);
void fft_execute_qpu(unsigned nsamples);
void fft_fetch_result(complex_t* output, unsigned N);
//volatile uint32_t* getRegisterMap();

#endif      //GPU_H
