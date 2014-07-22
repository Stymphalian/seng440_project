#!/bin/bash

generator=gen_samples
for (( i = 1,pow=2; i <= 18; i++ ))
do
	((pow *= 2))
	echo "sample data $pow samples"
	./$generator $pow 0 1 > "$pow"_re_sin_samples
	./$generator $pow 1 1 > "$pow"_re_im_sin_samples
	./$generator $pow 2 1 > "$pow"_re_step_samples
	./$generator $pow 3 1 > "$pow"_re_im_step_samples
	./$generator $pow 4 1 > "$pow"_re_rect_samples
	./$generator $pow 5 1 > "$pow"_re_im_rect_samples

done

fft=kiss_fft
for (( i = 1,pow=2; i <= 18; i++ ))
do
	((pow *= 2))
	echo "forward FFT for $pow samples"
	./$fft $pow 0 < "$pow"_re_sin_samples 			> "$pow"_re_sin_forward
	./$fft $pow 0 < "$pow"_re_im_sin_samples 		> "$pow"_re_im_sin_forward
	./$fft $pow 0 < "$pow"_re_step_samples			> "$pow"_re_step_forward
	./$fft $pow 0 < "$pow"_re_im_step_samples 	> "$pow"_re_im_step_forward
	./$fft $pow 0 < "$pow"_re_rect_samples 		> "$pow"_re_rect_forward
	./$fft $pow 0 < "$pow"_re_im_rect_samples 	> "$pow"_re_im_rect_forward
done

mkdir sample_data
mkdir forward_data
mv *_re*_samples sample_data
mv *_re*_forward forward_data
