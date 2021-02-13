#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// def seek_next_line_start(fp):
    // while fp.read(1)[0] != 0x0a:
        // pass

void gramsearch_seek_next_line_start(FILE* fp)
{
    while (!feof(fp) && fgetc(fp) != 0x0a);
}

// def seek_prev_line_start(fp):
void gramsearch_seek_prev_line_start(FILE* fp)
{
    // pos = fp.tell() - 1
    // fp.seek(pos, io.SEEK_SET)
    long pos = ftell(fp) - 1;
    fseek(fp, pos, SEEK_SET);
    
    // # print(' SLS at {}'.format(pos))
    
    // while fp.read(1)[0] != 0x0a:
    while (fgetc(fp) != 0x0a)
    {
        // pos -= 1
        // fp.seek(pos, io.SEEK_SET)
        // # print(' SLS at {}'.format(pos))
        pos -= 1;
        fseek(fp, pos, SEEK_SET);
    }
    
    // pos -= 1
    // fp.seek(pos, io.SEEK_SET)
    pos -= 1;
    fseek(fp, pos, SEEK_SET);
    
    // while fp.read(1)[0] != 0x0a:
    while (fgetc(fp) != 0x0a)
    {
        // pos -= 1
        // fp.seek(pos, io.SEEK_SET)
        // # print(' SLS at {}'.format(pos))
        pos -= 1;
        fseek(fp, pos, SEEK_SET);
    }
    
    // fp.seek(pos + 1, io.SEEK_SET)
    fseek(fp, pos + 1, SEEK_SET);
}

// def seek_line_start(fp):
void gramsearch_seek_line_start(FILE* fp)
{
    // pos = fp.tell() - 1
    // fp.seek(pos, io.SEEK_SET)
    long pos = ftell(fp) - 1;
    fseek(fp, pos, SEEK_SET);
    // # print(' SLS at {}'.format(pos))
    
    // while fp.read(1)[0] != 0x0a:
    while (fgetc(fp) != 0x0a)
    {
        // pos -= 1
        // fp.seek(pos, io.SEEK_SET)
        // # print(' SLS at {}'.format(pos))
        pos -= 1;
        fseek(fp, pos, SEEK_SET);
    }
    
    // fp.seek(pos + 1, io.SEEK_SET)
    fseek(fp, pos + 1, SEEK_SET);
}

// def get_word(fp):
char* gramsearch_get_word(FILE* fp)
{
    // start = fp.tell()
    long start = ftell(fp);
    
    // if fp.read(1)[0] in (0x0a, 0x20):
        // start += 1
        // fp.seek(start, io.SEEK_SET)
    
    unsigned char c = fgetc(fp);
    
    if (c == 0x0a || c == 0x20)
    {
        start += 1;
        fseek(fp, start, SEEK_SET);
    }
    
    while (1)
    {
        c = fgetc(fp);
        
        if (c == 0x0a || c == 0x20)
        {
            break;
        }
    }
    // while fp.read(1)[0] not in (0x0a, 0x20):
        // pass
    
    // end = fp.tell() - 1
    long end = ftell(fp) - 1;
    
    // fp.seek(start, io.SEEK_SET)
    fseek(fp, start, SEEK_SET);
    
    
    unsigned char* buf = malloc(end - start + 1);
    buf[end - start] = 0;
    
    fread(buf, 1, end - start, fp);
    // return fp.read(end - start)
    
    return buf;
}

// def get_line(fp):
unsigned char* gramsearch_get_line(FILE* fp)
{
    // start = fp.tell()
    long start = ftell(fp);
    
    while (fgetc(fp) != 0x0a);
    // while fp.read(1)[0] != 0x0a:
        // pass
    
    // end = fp.tell() - 1
    long end = ftell(fp) - 1;
    
    // fp.seek(start, io.SEEK_SET)
    fseek(fp, start, SEEK_SET);
    
    unsigned char* buf = malloc(end - start + 1);
    buf[end - start] = 0;
    
    fread(buf, 1, end - start, fp);
    // return fp.read(end - start)
    
    return buf;
}

// def binsearch(fp, start, end, text):
int binsearch(FILE* fp, long start, long end, const unsigned char* text)
{
    // textlen = len(text)
    int textlen = strlen(text);
    
    // middle = (start + end) // 2
    long middle = (start + end) / 2;
    
    // while end - start > 10:
    while (end - start > 10)
    {
        // fp.seek(middle, io.SEEK_SET)
        // seek_line_start(fp)
        fseek(fp, middle, SEEK_SET);
        gramsearch_seek_line_start(fp);
        
        // # print('at {}, interval is {}-({})-{} [{} chars]'.format(fp.tell(), start, middle, end, end - start))
        // printf("at %d, interval is %d-(%d)-%d\n [%d chars]\n", ftell(fp), start, middle, end, end - start);
        
        // found = True
        int found = 1;
        
        // for i in range(0, textlen):
        for (int i = 0; i < textlen; ++i)
        {
            // f_ch = fp.read(1)[0]
            unsigned char f_ch = fgetc(fp);
            // ch = text[i]
            unsigned char ch = text[i];
            
            // if ch < f_ch:
            if (ch < f_ch)
            {
                // # print('char {}: {} ({}) < {} ({}), backward'.format(i, ch, chr(ch), f_ch, chr(f_ch)))
                // end = middle
                // middle = (start + end) // 2
                // found = False
                // break
                
                end = middle;
                middle = (start + end) / 2;
                found = 0;
                break;
            }
            else if (ch > f_ch)
            {
            // elif ch > f_ch:
                // # print('char {}: {} ({}) > {} ({}), forward'.format(i, ch, chr(ch), f_ch, chr(f_ch)))
                // start = middle
                // middle = (start + end) // 2
                // found = False
                // break
                
                start = middle;
                middle = (start + end) / 2;
                found = 0;
                break;
            }
            else
            {
            // else:
                // # print('character {} matches'.format(ch))
                // printf("character %d matches\n", ch);
                
                // if i == textlen - 1 and fp.read(1)[0] != 0x20:
                if (i == textlen - 1 && fgetc(fp) != 0x20)
                {
                    // # print('hoola')
                    // printf("hoola\n");
                    // # print('{} < {} ({}), backward(2)'.format(ch, f_ch, chr(f_ch)))
                    // end = middle
                    // middle = (start + end) // 2
                    // found = False
                    // break
                    end = middle;
                    middle = (start + end) / 2;
                    found = 0;
                    break;
                }
            }
        }
        
        // if found:
        if (found)
        {
            // return (start, end)
            return 1;
        }
    }
    
    // # print('binsearch reached bottom of function without finding anything')
    // # quit()
    // return (-1,-1)
    return 0;
}


// NGRAM_START = 'starts'
// NGRAM_MID = 'mids'
// NGRAM_END = 'ends'

#define NGRAM_START 1
#define NGRAM_MID 2
#define NGRAM_END 3

// def ngramscore(text, pos):
int ngramscore(const unsigned char* text, int pos)
{
    int n = 1;
    
    // n = text.count(' ') + 1
    for (int i = 0; i < strlen(text); ++i)
    {
        if (text[i] == ' ')
        {
            n += 1;
        }
    }
    
    // if n == 3:
    if (n == 3)
    {
        FILE* fp;
        
        // f = open('sbc-txt/sbc_tri' + pos + '.txt_proc.txt', 'rb')
        switch (pos)
        {
            case NGRAM_START:
                fp = fopen("sbc-txt/sbc_tristarts.txt_proc.txt", "rb");
                break;
            
            case NGRAM_MID:
                fp = fopen("sbc-txt/sbc_trimids.txt_proc.txt", "rb");
                break;
            
            case NGRAM_END:
                fp = fopen("sbc-txt/sbc_triends.txt_proc.txt", "rb");
                break;
            
            default:
                break;
        }
        
        // f.seek(0, io.SEEK_END)
        // fsize = f.tell()
        // f.seek(0, io.SEEK_SET)
        
        fseek(fp, 0, SEEK_END);
        long fsize = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        
        // start = 0
        // end = fsize
        long start = 0;
        long end = fsize;
        
        // start, end = binsearch(f, start, end, bytes(text, encoding='utf-8'))
        
        int result = binsearch(fp, start, end, text);
        
        // if start != -1:
        if (result)
        {
            // seek_line_start(f)
            gramsearch_seek_line_start(fp);
            
            // fst, snd, thd = get_word(f), get_word(f), get_word(f)
            unsigned char* fst = gramsearch_get_word(fp);
            unsigned char* snd = gramsearch_get_word(fp);
            unsigned char* thd = gramsearch_get_word(fp);
            
            // return int(str(get_word(f), encoding='utf-8'))
            unsigned char* scoretext = gramsearch_get_word(fp);
            int score = (int) strtol(scoretext, NULL, 10);
            
            free(fst);
            free(snd);
            free(thd);
            free(scoretext);
            
            fclose(fp);
            
            return score;
        }
        else
        {
        // else:
            // return 0
            return 0;
        }
    }
    else if (n == 2)
    {
    // elif n == 2:
        // f = open('sbc-txt/sbc_bi' + pos + '.txt_proc.txt', 'rb')
        // f.seek(0, io.SEEK_END)
        // fsize = f.tell()
        // f.seek(0, io.SEEK_SET)
        
        // start = 0
        // end = fsize
        
        // start, end = binsearch(f, start, end, bytes(text, encoding='utf-8'))
        
        // if start != -1:
            // seek_line_start(f)
            // fst, snd = get_word(f), get_word(f)
            
            // return int(str(get_word(f), encoding='utf-8'))
        // else:
            // return 0
        FILE* fp;
        
        switch (pos)
        {
            case NGRAM_START:
                fp = fopen("sbc-txt/sbc_bistarts.txt_proc.txt", "rb");
                break;
            
            case NGRAM_MID:
                fp = fopen("sbc-txt/sbc_bimids.txt_proc.txt", "rb");
                break;
            
            case NGRAM_END:
                fp = fopen("sbc-txt/sbc_biends.txt_proc.txt", "rb");
                break;
            
            default:
                break;
        }
        
        // f.seek(0, io.SEEK_END)
        // fsize = f.tell()
        // f.seek(0, io.SEEK_SET)
        
        fseek(fp, 0, SEEK_END);
        long fsize = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        
        // start = 0
        // end = fsize
        long start = 0;
        long end = fsize;
        
        // start, end = binsearch(f, start, end, bytes(text, encoding='utf-8'))
        
        int result = binsearch(fp, start, end, text);
        // printf("result=%d\n", result);
        // if start != -1:
        if (result)
        {
            // seek_line_start(f)
            gramsearch_seek_line_start(fp);
            
            // fst, snd, thd = get_word(f), get_word(f), get_word(f)
            unsigned char* fst = gramsearch_get_word(fp);
            unsigned char* snd = gramsearch_get_word(fp);
            
            // return int(str(get_word(f), encoding='utf-8'))
            unsigned char* scoretext = gramsearch_get_word(fp);
            int score = (int) strtol(scoretext, NULL, 10);
            
            free(fst);
            free(snd);
            free(scoretext);
            
            fclose(fp);
            
            return score;
        }
        else
        {
        // else:
            // return 0
            return 0;
        }
    }
}

// if __name__ == '__main__':
    // # print(ngramscore('åka till stockholm', NGRAM_MID))
    // print(ngramscore('just nu', NGRAM_MID))
    // print(ngramscore('nu är', NGRAM_MID))
    // print(ngramscore('är det', NGRAM_MID))
    // print(ngramscore('det totalstopp', NGRAM_MID))
    // print(ngramscore('totalstopp på', NGRAM_MID))
    // print(ngramscore('på platsen', NGRAM_MID))


int main(int argc, char** argv)
{
    for (int i = 1; i < 15000; ++i)
    {
        ngramscore("just nu", NGRAM_MID);
        ngramscore("nu är", NGRAM_MID);
        ngramscore("är det", NGRAM_MID);
    }
}
