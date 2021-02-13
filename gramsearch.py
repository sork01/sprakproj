#!/usr/bin/python3
# -*- coding: utf-8 -*-

# *neode.command* setgo python3 gramsearch.py

import io

def seek_next_line_start(fp):
    while fp.read(1)[0] != 0x0a:
        pass

def seek_prev_line_start(fp):
    pos = fp.tell() - 1
    fp.seek(pos, io.SEEK_SET)
    
    # print(' SLS at {}'.format(pos))
    
    while fp.read(1)[0] != 0x0a:
        pos -= 1
        fp.seek(pos, io.SEEK_SET)
        # print(' SLS at {}'.format(pos))
    
    pos -= 1
    fp.seek(pos, io.SEEK_SET)
    
    while fp.read(1)[0] != 0x0a:
        pos -= 1
        fp.seek(pos, io.SEEK_SET)
    
    fp.seek(pos + 1, io.SEEK_SET)

def seek_line_start(fp):
    pos = fp.tell() - 1
    fp.seek(pos, io.SEEK_SET)
    
    # print(' SLS at {}'.format(pos))
    
    while fp.read(1)[0] != 0x0a:
        pos -= 1
        fp.seek(pos, io.SEEK_SET)
        # print(' SLS at {}'.format(pos))
    
    fp.seek(pos + 1, io.SEEK_SET)

def get_word(fp):
    start = fp.tell()
    
    if fp.read(1)[0] in (0x0a, 0x20):
        start += 1
        fp.seek(start, io.SEEK_SET)
    
    while fp.read(1)[0] not in (0x0a, 0x20):
        pass
    
    end = fp.tell() - 1
    
    fp.seek(start, io.SEEK_SET)
    return fp.read(end - start)

def get_line(fp):
    start = fp.tell()
    
    while fp.read(1)[0] != 0x0a:
        pass
    
    end = fp.tell() - 1
    
    fp.seek(start, io.SEEK_SET)
    return fp.read(end - start)

def binsearch(fp, start, end, text):
    textlen = len(text)
    middle = (start + end) // 2
    
    while end - start > 10:
        fp.seek(middle, io.SEEK_SET)
        seek_line_start(fp)
        
        # print('at {}, interval is {}-({})-{} [{} chars]'.format(fp.tell(), start, middle, end, end - start))
        
        found = True
        
        for i in range(0, textlen):
            f_ch = fp.read(1)[0]
            ch = text[i]
            
            if ch < f_ch:
                # print('char {}: {} ({}) < {} ({}), backward'.format(i, ch, chr(ch), f_ch, chr(f_ch)))
                end = middle
                middle = (start + end) // 2
                found = False
                break
            elif ch > f_ch:
                # print('char {}: {} ({}) > {} ({}), forward'.format(i, ch, chr(ch), f_ch, chr(f_ch)))
                start = middle
                middle = (start + end) // 2
                found = False
                break
            else:
                # print('character {} matches'.format(ch))
                
                if i == textlen - 1 and fp.read(1)[0] != 0x20:
                    # print('hoola')
                    # print('{} < {} ({}), backward(2)'.format(ch, f_ch, chr(f_ch)))
                    end = middle
                    middle = (start + end) // 2
                    found = False
                    break
        
        if found:
            return (start, end)
    
    # print('binsearch reached bottom of function without finding anything')
    # quit()
    return (-1,-1)


NGRAM_START = 'starts'
NGRAM_MID = 'mids'
NGRAM_END = 'ends'

def ngramscore(text, pos):
    n = text.count(' ') + 1
    
    if n == 3:
        f = open('sbc-txt/sbc_tri' + pos + '.txt_proc.txt', 'rb')
        f.seek(0, io.SEEK_END)
        fsize = f.tell()
        f.seek(0, io.SEEK_SET)
        
        start = 0
        end = fsize
        
        start, end = binsearch(f, start, end, bytes(text, encoding='utf-8'))
        
        if start != -1:
            seek_line_start(f)
            fst, snd, thd = get_word(f), get_word(f), get_word(f)
            
            return int(str(get_word(f), encoding='utf-8'))
        else:
            return 0
    elif n == 2:
        f = open('sbc-txt/sbc_bi' + pos + '.txt_proc.txt', 'rb')
        f.seek(0, io.SEEK_END)
        fsize = f.tell()
        f.seek(0, io.SEEK_SET)
        
        start = 0
        end = fsize
        
        start, end = binsearch(f, start, end, bytes(text, encoding='utf-8'))
        
        if start != -1:
            seek_line_start(f)
            fst, snd = get_word(f), get_word(f)
            
            return int(str(get_word(f), encoding='utf-8'))
        else:
            return 0

if __name__ == '__main__':
    # print(ngramscore('åka till stockholm', NGRAM_MID))
    print(ngramscore('just nu', NGRAM_MID))
    print(ngramscore('nu är', NGRAM_MID))
    print(ngramscore('är det', NGRAM_MID))
    print(ngramscore('det totalstopp', NGRAM_MID))
    print(ngramscore('totalstopp på', NGRAM_MID))
    print(ngramscore('på platsen', NGRAM_MID))
