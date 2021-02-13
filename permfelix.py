#!/usr/bin/python3
# -*- coding: utf-8 -*-

# *neode.command* setgo python3 permfelix.py

import sys
import itertools
import random

from gramsearch import ngramscore, NGRAM_START, NGRAM_MID, NGRAM_END

def find_bigrams(input_list):
  return zip(input_list, input_list[1:])

s = ('flera olika sorters bönor har skal')
print(s)
# s = ('var visste det det ingen som om')
s = s.split()
random.shuffle(s)
print(s)
# quit()

lista = itertools.permutations(s)

t = dict()

least_zeroes = len(s)
candidates = []

for perm in lista:
    z = []
    bigrams = list(map(lambda x: ' '.join(x), zip(perm, perm[1:])))
    trigrams = list(map(lambda x: ' '.join(x), zip(perm, perm[1:], perm[2:])))
    zeroes = 0
    
    if not '^' + bigrams[0] in t:
        t['^' + bigrams[0]] = ngramscore(bigrams[0], NGRAM_START)
    
    if not bigrams[-1] + '$' in t:
        t[bigrams[-1] + '$'] = ngramscore(bigrams[-1], NGRAM_END)
    
    if not '^' + trigrams[0] in t:
        t['^' + trigrams[0]] = ngramscore(trigrams[0], NGRAM_START)
    
    if not trigrams[-1] + '$' in t:
        t[trigrams[-1] + '$'] = ngramscore(trigrams[-1], NGRAM_END)
    
    if t['^' + bigrams[0]] == 0:
        zeroes += 1
        z.append('^' + bigrams[0])
    if t['^' + trigrams[0]] == 0:
        zeroes += 1
        z.append('^' + trigrams[0])
    if t[bigrams[-1] + '$'] == 0:
        zeroes += 1
        z.append(bigrams[-1] + '$')
    if t[trigrams[-1] + '$'] == 0:
        zeroes += 1
        z.append(trigrams[-1] + '$')
    
    if zeroes > least_zeroes:
        continue
    
    for pair in bigrams[1:-1]:
        if not pair in t:
            t[pair] = ngramscore(pair, NGRAM_MID)
        
        if t[pair] == 0:
            z.append(pair)
            zeroes += 1
    
    if zeroes > least_zeroes:
        continue
    
    for threes in trigrams[1:-1]:
        if not threes in t:
            t[threes] = ngramscore(threes, NGRAM_MID)
        
        if t[threes] == 0:
            z.append(threes)
            zeroes += 1
    
    if zeroes > least_zeroes:
        continue
    
    if zeroes < least_zeroes:
        least_zeroes = zeroes
        # print('new least zeroes: {}'.format(least_zeroes))
        # print(z)
        candidates = [perm]
    elif zeroes == least_zeroes:
        # print('same least')
        # print(z)
        candidates.append(perm)

# print(candidates)

best = 0
ans = []

for perm in candidates:
    bigrams = list(map(lambda x: ' '.join(x), zip(perm, perm[1:])))
    trigrams = list(map(lambda x: ' '.join(x), zip(perm, perm[1:], perm[2:])))
    score = 0
    
    score += t['^' + bigrams[0]] * 2 # * vikt för startar-mening
    score += t[bigrams[-1] + '$'] * 2 # * vikt för avslutar-mening
    
    score += t['^' + trigrams[0]] * 2 # * vikt för startar-mening
    score += t[trigrams[-1] + '$'] * 2 # * vikt för avslutar-mening
    
    for pair in bigrams[1:-1]:
        score += t[pair]
    
    for threes in trigrams[1:-1]:
        score += t[threes] * 3
    
    if score > best:
        # print('new best {} @ {}'.format(str(perm), score))
        best = score
        ans = list(perm)

print(ans)
