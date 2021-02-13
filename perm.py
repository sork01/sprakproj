#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
import itertools

def find_bigrams(input_list):
  return zip(input_list, input_list[1:])

s = ("somnade han och bröt lårbenshalsen")
s = s.split()
lista = list(itertools.permutations(s))

t = [('bröt', 'lårbenshalsen', 5),('han', 'bröt', 20),('lårbenshalsen', 'och', 2),('och', 'somnade', 5),('somnade', 'och', 3),('lårbenshalsen', 'bröt', 2)]


scorelist = []
permlist = []
for perm in lista:
    score = 0
    for bigrams in t:
        permtest = find_bigrams(perm)
        for shit in permtest:
            if shit[0] == bigrams[0] and shit[1] == bigrams[1]:
                score += bigrams[2]   

    scorelist += [(score)]
    permlist += [(perm)]

index = scorelist.index(max(scorelist))  
answer = permlist[index]
for ord in answer:
    sys.stdout.write(ord + " ")
print ""
    # print find_bigrams(perm)
    # find_bigram_in_text(perm)
    





   