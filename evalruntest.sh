#!/bin/bash

numrounds=$1

for n in {3..8}; do
	rm -f evaltest$n*.txt

	for round in $(seq 1 $numrounds); do
		OLDIFS=$IFS
		IFS=$'\n'

		for s in $(cat eval$n.txt); do
			echo "$s" | ./descramble.py --eval | tee -a evaltest$n.$round.txt
		done

		IFS=$OLDIFS
	done
done
