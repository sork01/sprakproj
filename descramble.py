#!/usr/bin/python3
# -*- coding: utf-8 -*-

# *neode.command* setgo python3 descramble.py

# detta är alltså felix (första) metod med lite ökad vikt på trigram och
# sentence starters/enders

import sys
import itertools
import random
import re

from gramsearch import ngramscore, NGRAM_START, NGRAM_MID, NGRAM_END

showscores = False

if len(sys.argv) > 1 and sys.argv[1] == '--eval':
    sys.argv.append(sys.stdin.read().strip())
    sentence = sys.argv[2].split(' ')
    scrambled = sys.argv[2].split(' ')
    showscores = True
elif len(sys.argv) > 2:
    sentence = sys.argv[1].split(' ')
    scrambled = sys.argv[2].split(' ')
else:
    sentence = sys.argv[1].split(' ')
    scrambled = sys.argv[1].split(' ')

sentence = list(map(lambda x: re.sub(r'[\d.,]+', 'NUMBER', x), sentence))
scrambled = list(map(lambda x: re.sub(r'[\d.,]+', 'NUMBER', x), sentence))

lista = itertools.permutations(scrambled)

t = dict()

least_zeroes = len(sentence)*10
candidates = []

for perm in lista:
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
    if t['^' + trigrams[0]] == 0:
        zeroes += 1
    if t[bigrams[-1] + '$'] == 0:
        zeroes += 1
    if t[trigrams[-1] + '$'] == 0:
        zeroes += 1
    
    if zeroes > least_zeroes:
        continue
    
    for pair in bigrams[1:-1]:
        if not pair in t:
            t[pair] = ngramscore(pair, NGRAM_MID)
        
        if t[pair] == 0:
            zeroes += 1
    
    if zeroes > least_zeroes:
        continue
    
    for threes in trigrams[1:-1]:
        if not threes in t:
            t[threes] = ngramscore(threes, NGRAM_MID)
        
        if t[threes] == 0:
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
    
    if score >= best:
        # print('new best {} @ {}'.format(str(perm), score))
        best = score
        ans = list(perm)


def calcscore(sentence, descrambled):
    strsent = ' '.join(sentence)
    score = 0
    total = 0
    
    for i in range(2, len(sentence) + 1):
        for j in range(0, len(sentence) - i + 1):
            total += 1
            strans = ' '.join(descrambled[j:j+i])
            
            # print(strans)
            
            if strans in strsent:
                score += 1
    
    return float(score) / float(total)

randomdescramble = list(sentence)
random.shuffle(randomdescramble)

if showscores:
    print('ours: {} | random: {}'.format(' '.join(ans), ' '.join(randomdescramble)))
    print('{:.2f} | {:.2f}'.format(calcscore(sentence, ans), calcscore(sentence, randomdescramble)))
else:
    print(' '.join(ans))
