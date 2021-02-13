#!/usr/bin/python3
# -*- coding: utf-8 -*-

# *neode.command* setgo python3 descramble_robin.py

import sys
import itertools
import random

from gramsearch import ngramscore, NGRAM_START, NGRAM_MID, NGRAM_END

ourscore = 0
randscore = 0
missed = 0
for i in range(0,100):
    def find_bigrams(input_list):
      return zip(input_list, input_list[1:])

    count = 0
    with open('sent7.txt') as f:
        lines = f.readlines()
    for line in lines:
        count = count + 1
    randnr = random.randint(0,count)
    sentence = lines[randnr].strip().lower()
    print (sentence)
    sentence = sentence.split()
    # sentence = sys.argv[1].split(' ')
    scrambled = sentence

    lista = itertools.permutations(scrambled)

    t = dict()

    best = 0
    ans = []

    for perm in lista:
        bigrams = list(map(lambda x: ' '.join(x), zip(perm, perm[1:])))
        trigrams = list(map(lambda x: ' '.join(x), zip(perm, perm[1:], perm[2:])))
        
        if not '^' + bigrams[0] in t:
            t['^' + bigrams[0]] = ngramscore(bigrams[0], NGRAM_START)
        
        if not bigrams[-1] + '$' in t:
            t[bigrams[-1] + '$'] = ngramscore(bigrams[-1], NGRAM_END)
        
        if not '^' + trigrams[0] in t:
            t['^' + trigrams[0]] = ngramscore(trigrams[0], NGRAM_START)
        
        if not trigrams[-1] + '$' in t:
            t[trigrams[-1] + '$'] = ngramscore(trigrams[-1], NGRAM_END)
        
        score = 0
        
        if t['^' + bigrams[0]] == 0:
            score -= 1
        if t['^' + trigrams[0]] == 0:
            score -= 1
        if t[bigrams[-1] + '$'] == 0:
            score -= 1
        if t[trigrams[-1] + '$'] == 0:
            score -= 1
        
        # om nåt är noll så bidrar det ingenting här
        score += t['^' + bigrams[0]] * 2 # * vikt för startar-mening
        score += t[bigrams[-1] + '$'] * 2 # * vikt för avslutar-mening
        
        score += t['^' + trigrams[0]] * 2 # * vikt för startar-mening
        score += t[trigrams[-1] + '$'] * 2 # * vikt för avslutar-mening
        
        for pair in bigrams[1:-1]:
            if not pair in t:
                t[pair] = ngramscore(pair, NGRAM_MID)
            
            if t[pair] == 0:
                score -= 1
            else:
                score += t[pair]
        
        for threes in trigrams[1:-1]:
            if not threes in t:
                t[threes] = ngramscore(threes, NGRAM_MID)
            
            if t[threes] == 0:
                score -= 1
            else:
                score += t[threes] * 3
        
        if score > best:
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
    ourscore += calcscore(sentence, ans)
    randscore += calcscore(sentence, randomdescramble)
    if ' '.join(ans) == "":
        missed += 1
        print ("DEN HÄR MISSADE VI!!!!!!!!")
    print('ours: {} | random: {}'.format(' '.join(ans), ' '.join(randomdescramble)))
    print('{:.2f} | {:.2f}'.format(calcscore(sentence, ans), calcscore(sentence, randomdescramble)))

print ("WE MISSED: " + str(missed))
print ("ours: " + str((ourscore - missed) / (100 - missed)))
print ("random: " + str((randscore - missed)/ (100 - missed)))