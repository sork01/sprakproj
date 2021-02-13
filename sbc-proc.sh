#!/bin/bash

rm -rf sbc-txt
./sbc "$1"
mkdir sbc-txt
mv sbc*.txt sbc-txt
cd sbc-txt

for f in $(ls *.txt); do
	cat "$f" | LC_ALL=C sort | uniq -c | awk '{ print $2 " " $3 " " $4 " " $1}' | tr -s ' ' > "$f"_proc.txt
done
