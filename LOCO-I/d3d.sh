#!/bin/bash

IMAGEN=$1
NMAX=$2
T1=$3
T2=$4
T3=$5

MAX=80
MSG="> T1=${T1} - T2=${T2} - T3=${T3} - Nmax=${NMAX} Imagen=${IMAGEN}"

if [ `cat /Users/juan/Desktop/Proyecto/codigo/D3D/LOCO-I/.nval` != T1${T1}T2${T2}T3${T3} ]; then
	g++ -std=c++11 -g *.cpp -DT1=${T1} -DT2=${T2} -DT3=${T3}
	echo "T1${T1}T2${T2}T3${T3}">.nval
fi

./a.out ${IMAGEN} ${NMAX} > /dev/null

ESPACIOS=
for i in `seq ${#MSG} ${MAX}`; do ESPACIOS=" ${ESPACIOS}"; done

echo -n "${MSG}${ESPACIOS} - Tam="
ls -l ${IMAGEN}_coded_Nmax_${NMAX}_1 | awk '{ print $5 }'

rm ${IMAGEN}_coded_Nmax_${NMAX}_1 
rm -f ${IMAGEN}_coded_Nmax_${NMAX}_1_decoded_.pgm 
