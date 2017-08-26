#!/bin/bash

NMIN=$1
shift

NMAX=$1
shift

T1MIN=$1
shift

T1MAX=$1
shift

T2MIN=$1
shift

T2MAX=$1
shift

T3MIN=$1
shift

T3MAX=$1
shift

for t1 in `seq ${T1MIN} ${T1MAX}`; do
	for t2 in `seq ${T2MIN} ${T2MAX}`; do
		for t3 in `seq ${T3MIN} ${T3MAX}`; do
			for n in `seq ${NMIN} ${NMAX}`; do
				for img in $@; do
					./d3d.sh ${img} ${n} ${t1} ${t2} ${t3}
				done
			done
		done
	done
done

rm -f .files

