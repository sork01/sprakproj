#!/usr/bin/env python

import sys
import random

count = 0
with open('sent6.txt') as f:
    lines = f.readlines()
for line in lines:
    count = count + 1
randnr = random.randint(0,count)
chosenline = lines[randnr].strip()
print chosenline.lower()
words = chosenline.split()
wordcount = len(words)
s = list(range(wordcount))
random.shuffle(s)
sent = ""
for number in s:
    sent = sent + words[number] + " "
sent = sent.strip()
print sent.lower()