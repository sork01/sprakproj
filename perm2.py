#!/usr/bin/python3
# -*- coding: utf-8 -*-

# *neode.command* setgo python3 perm2.py

import sys
import itertools
import random

from gramsearch import ngramscore, NGRAM_START, NGRAM_MID, NGRAM_END

def find_bigrams(input_list):
  return zip(input_list, input_list[1:])

s = ('biltillverkaren hade fuskat med proven')
print(s)
# s = ('var visste det det ingen som om')
s = s.split()
random.shuffle(s)
print(s)
# quit()

lista = itertools.permutations(s)

# t = [('bröt', 'lårbenshalsen', 5),('han', 'bröt', 20),('lårbenshalsen', 'och', 2),('och', 'somnade', 5),('somnade', 'och', 3),('lårbenshalsen', 'bröt', 2)]
t = dict()
# t['bröt lårbenshalsen'] = 5
# t['han bröt'] = 20
# t['lårbenshalsen och'] = 2
# t['och somnade'] = 5
# t['somnade och'] = 3
# t['lårbenshalsen bröt'] = 2

best = 0
ans = []

num = 0

for perm in lista:
    num += 1
    bigrams = list(map(lambda x: ' '.join(x), zip(perm, perm[1:])))
    trigrams = list(map(lambda x: ' '.join(x), zip(perm, perm[1:], perm[2:])))
    score = 0
    
    if not '^' + bigrams[0] in t:
        t['^' + bigrams[0]] = ngramscore(bigrams[0], NGRAM_START)
    
    if not bigrams[-1] + '$' in t:
        t[bigrams[-1] + '$'] = ngramscore(bigrams[-1], NGRAM_END)
    
    if not '^' + trigrams[0] in t:
        t['^' + trigrams[0]] = ngramscore(trigrams[0], NGRAM_START)
    
    if not trigrams[-1] + '$' in t:
        t[trigrams[-1] + '$'] = ngramscore(trigrams[-1], NGRAM_END)

    score += t['^' + bigrams[0]] * 2000 # * vikt för startar-mening
    score += t[bigrams[-1] + '$'] * 2000 # * vikt för avslutar-mening
    
    score += t['^' + trigrams[0]] * 5000 # * vikt för startar-mening
    score += t[trigrams[-1] + '$'] * 5000 # * vikt för avslutar-mening
    
    for pair in bigrams[1:-1]:
        if not pair in t:
            t[pair] = ngramscore(pair, NGRAM_MID)
        
        score += t[pair]
    
    for threes in trigrams[1:-1]:
        if not threes in t:
            t[threes] = ngramscore(threes, NGRAM_MID)
        
        score += t[threes]
        
    if score > best:
        # print('new best {} @ {}'.format(str(perm), score))
        best = score
        ans = list(perm)

print(ans)

# bigrams = list(map(lambda x: ' '.join(x), zip(ans, ans[1:])))
# trigrams = list(map(lambda x: ' '.join(x), zip(ans, ans[1:], ans[2:])))

# bigrams = ['^' + bigrams[0]] + bigrams + [bigrams[-1] + '$']
# trigrams = ['^' + trigrams[0]] + trigrams + [trigrams[-1] + '$']

# for x in bigrams:
    # print('"{}" has score {}'.format(x, t[x]))

# print(len(t.keys()))
# print(t['är det'])
