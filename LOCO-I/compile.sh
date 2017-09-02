#!/bin/bash

for T1 in `seq 2 4`; do
	for T2 in `seq 5 9`; do
		for T3 in `seq 15 25`; do
			for RESET in 8 16 32 64 128 256 512 1024; do
				echo -n ">> Generando ejecutable para T1=${T1} T2=${T2} T3=${T3} RESET=${RESET}: "
				g++ -std=c++11 -g *.cpp -DT1=${T1} -DT2=${T2} -DT3=${T3} -DRESET=${RESET}
				mv a.out dicom_${T1}_${T2}_${T3}_${RESET}
				echo "dicom_${T1}_${T2}_${T3}_${RESET}"
			done
		done
	done
done

