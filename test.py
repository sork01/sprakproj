#!/usr/bin/python3
# -*- coding: utf-8 -*-

import xml.etree.ElementTree
import re
import sys

MIN_WORDS_IN_SENTENCE = 3

in_sentence = False
current_sentence = list()

out_starts = open('starts.txt', 'w+')
out_ends = open('ends.txt', 'w+')
out_mids = open('mids.txt', 'w+')

num_sentences = 0

for event, e in xml.etree.ElementTree.iterparse(sys.argv[1], events=('start', 'end')):
    if not in_sentence and event == 'start' and e.tag == 'sentence':
        in_sentence = True
        del current_sentence[:]
    else:
        if event == 'end' and e.tag == 'sentence':
            in_sentence = False
            
            num_sentences += 1
            
            if num_sentences % 100 == 0:
                print(chr(13) + "{} sentences".format(num_sentences))
            
            # current_sentence = list(map(lambda x: x.lower(), current_sentence))
            current_sentence = list(filter(lambda x: re.search(r'[\wåäö]', x) != None, current_sentence))
            
            if len(current_sentence) < MIN_WORDS_IN_SENTENCE:
                continue
            
            out_starts.write(current_sentence[0])
            out_starts.write("\n")
            
            out_ends.write(current_sentence[-1])
            out_ends.write("\n")
            
            for i in range(0, len(current_sentence) - 1):
                out_mids.write("{} {}\n".format(current_sentence[i], current_sentence[i+1]))
            
            # quit()
        elif event == 'end' and e.tag == 'w':
            current_sentence.append(e.text.lower())
