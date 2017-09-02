#!/bin/bash

IMAGEN=$1
NMAX=$2
T1=$3
T2=$4
T3=$5
RESET=$6

./a.out ${IMAGEN} ${NMAX} ${T1} ${T2} ${T3} ${RESET}> /dev/null

ls -l ${IMAGEN}_coded_Nmax_${NMAX}_1 | awk '{ print $5 }'

rm ${IMAGEN}_coded_Nmax_${NMAX}_1 
rm -f ${IMAGEN}_coded_Nmax_${NMAX}_1_decoded_.pgm 
