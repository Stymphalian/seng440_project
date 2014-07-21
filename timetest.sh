#!/bin/bash

for (( i=1, pow=2; i <= 18; i++ )) 
do
	(( pow *= 2 ))	
	./testbench/gen_samples $pow 1 1 > "$pow"_input.test
	./noin $pow 0 < "$pow"_input.test
done
