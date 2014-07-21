#!/bin/bash

program=../noin
generator=gen_samples
fft=kiss_fft
rms_test=rms_test


test_num=0
# param 1 - number of samples
# param 2 - 0 for forward fft, 1 for inverse
# param 3 - type of test.
# 	0 - real sin wave
# 	1 - real + im sin wave
# 	2 - real step function
# 	3 - real + im step function
# 	4 - real rect function
#	5 - real + im rect function
function run_test {
	printf "Test $test_num: "
	# forward
	if [ "$2" == "0" ]; then
		./$generator $1 $3 1 > "$test_num"_samples.test
		./$fft $1 0 		< "$test_num"_samples.test > "$test_num"_kiss_forward.test
		./$program $1 0	< "$test_num"_samples.test > "$test_num"_program_forward.test

		./$rms_test $1 "$test_num"_kiss_forward.test "$test_num"_program_forward.test
	else
	# inverse
		./$generator $1 $3 1 > "$test_num"_samples.test
		./$fft $1 0 		< "$test_num"_samples.test > "$test_num"_kiss_forward.test
		./$program $1 1 	< "$test_num"_kiss_forward.test > "$test_num"_program_inverse.test

		./$rms_test $1 "$test_num"_samples.test "$test_num"_program_inverse.test
	fi

	(( test_num++ ))
}


run_test 4 0 0
run_test 8 0 0
run_test 16 0 0
run_test 32 0 0
run_test 64 0 0
run_test 128 0 0
run_test 256 0 0
run_test 512 0 0
run_test 1024 0 0

run_test 4 0 1
run_test 8 0 1
run_test 16 0 1
run_test 32 0 1
run_test 64 0 1
run_test 128 0 1
run_test 256 0 1
run_test 512 0 1
run_test 1024 0 1
