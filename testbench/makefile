CC=gcc
CFLAGS=-c -Wall
INFLAGS=-Iinclude
LDFLAGS=-L. -lm

all: kiss_fft gen_samples rms_test
		
kiss_fft: base_fft.c kiss_fft.c kiss_fft.h _kiss_fft_guts.h
	$(CC) $(CFLAGS) $(INFLAGS) base_fft.c kiss_fft.c
	$(CC) -o kiss_fft base_fft.o kiss_fft.o $(LDFLAGS)

gen_samples:
	$(CC) $(CFLAGS) $(INFLAGS) gen_samples.c
	$(CC) -o gen_samples gen_samples.o $(LDFLAGS)

rms_test: rms_test.c
	$(CC) $(CFLAGS) $(INFLAGS) rms_test.c
	$(CC) -o rms_test rms_test.o $(LDFLAGS)

clean:
	rm -rf *.o kiss_fft gen_samples rms_test
	rm  *.exe
