#!/bin/bash

program=noin

for (( i = 1,pow=2; i <= 18; i++ ))
do
	((pow *= 2))
	echo "Running $program forward fft on $pow sample_data"
	./$program $pow 0 < "sample_data/""$pow"_re_sin_samples 		> "$pow"_re_sin_forward
	./$program $pow 0 < "sample_data/""$pow"_re_im_sin_samples 	> "$pow"_re_im_sin_forward
	./$program $pow 0 < "sample_data/""$pow"_re_step_samples 	> "$pow"_re_step_forward
	./$program $pow 0 < "sample_data/""$pow"_re_im_step_samples > "$pow"_re_im_step_forward
	./$program $pow 0 < "sample_data/""$pow"_re_rect_samples 	> "$pow"_re_rect_forward
	./$program $pow 0 < "sample_data/""$pow"_re_im_rect_samples > "$pow"_re_im_rect_forward
done

for (( i = 1,pow=2; i <= 18; i++ ))
do
	((pow *= 2))
	echo "Running $program inverse fft on $pow forward_data"
	./$program $pow 1 < "$pow"_re_sin_forward 		> "$pow"_re_sin_inverse
	./$program $pow 1 < "$pow"_re_im_sin_forward 	> "$pow"_re_im_sin_inverse
	./$program $pow 1 < "$pow"_re_step_forward 		> "$pow"_re_step_inverse
	./$program $pow 1 < "$pow"_re_im_step_forward 	> "$pow"_re_im_step_inverse
	./$program $pow 1 < "$pow"_re_rect_forward 		> "$pow"_re_rect_inverse
	./$program $pow 1 < "$pow"_re_im_rect_forward 	> "$pow"_re_im_rect_inverse
done


comp=rms_test
for (( i = 1,pow=2; i <= 18; i++ ))
do
	((pow *= 2))
	echo "RMS forward fft on $pow forward_data"

	echo "re_sin"
	./$comp $pow "$pow"_re_sin_forward 		"forward_data/""$pow"_re_sin_forward

	echo "re_im_sin"
	./$comp $pow "$pow"_re_im_sin_forward 	"forward_data/""$pow"_re_im_sin_forward

	echo "re_step"
	./$comp $pow "$pow"_re_step_forward 		"forward_data/""$pow"_re_step_forward

	echo "re_im_step"
	./$comp $pow "$pow"_re_im_step_forward 	"forward_data/""$pow"_re_im_step_forward

	echo "re_rect"
	./$comp $pow "$pow"_re_rect_forward 		"forward_data/""$pow"_re_rect_forward

	echo "re_im_rect"
	./$comp $pow "$pow"_re_im_rect_forward 	"forward_data/""$pow"_re_im_rect_forward
done

for (( i = 1,pow=2; i <= 18; i++ ))
do
	((pow *= 2))
	echo "RMS inverse fft on $pow sample_data"

	echo "re_sin"
	./$comp $pow "$pow"_re_sin_inverse 		"sample_data/""$pow"_re_sin_samples

	echo "re_im_sin"
	./$comp $pow "$pow"_re_im_sin_inverse 	"sample_data/""$pow"_re_im_sin_samples

	echo "re_step"
	./$comp $pow "$pow"_re_step_inverse 		"sample_data/""$pow"_re_step_samples

	echo "re_im_step"
	./$comp $pow "$pow"_re_im_step_inverse 	"sample_data/""$pow"_re_im_step_samples

	echo "re_rect"
	./$comp $pow "$pow"_re_rect_inverse 		"sample_data/""$pow"_re_rect_samples

	echo "re_im_rect"
	./$comp $pow "$pow"_re_im_rect_inverse 	"sample_data/""$pow"_re_im_rect_samples
done


mkdir results
mv *_re*_forward results
mv *_re*_inverse results
