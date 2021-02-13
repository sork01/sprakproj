#!/usr/bin/python3
# -*- coding: utf-8 -*-

# *neode.command* setgo python3 sumscore.py

import sys

for i in range(3,9):
    lineno = 0
    ourscore = 0
    randscore = 0
    count = 0
    directory = str(sys.argv[1])
    with open(directory +  "/evaltest" + str(i) + ".1.txt") as f:
        lines = f.readlines()
        for line in lines:
            if lineno % 2 == 1:
                line = line.split("|")
                ourscore += float(line[0])
                randscore += float(line[1])
                count += 1
            lineno += 1            
    
    print("")
    print(str(i) + " ord långa meningar") 
    print("Descramble: " + str(float(ourscore) / float(count)))
    print("Slumpmässig ordning: " + str(float(randscore) / float(count)))
    print("")


