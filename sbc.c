//*neode.command* setgo ./sbc.sh

#include <libxml/xmlreader.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define XML_NODETYPE_ELEMENT_START 1
#define XML_NODETYPE_ELEMENT_TEXT 3
#define XML_NODETYPE_ELEMENT_END 15

#define WORD_MAX_LENGTH 30

int causes_rejected_sentence(gchar* w)
{
    gchar* p = w;
    int len = g_utf8_strlen(w, -1);
    int pos = 0;
    
    while (1)
    {
        guint32 ch = g_utf8_get_char(p);
        
        if (ch == 0x3c || ch == 0x3e || ch == 0x2b || ch == 0x2c || ch == 0x2e) // <>+,.
        {
            return 1;
        }
        
        p = g_utf8_next_char(p);
        ++pos;
        
        if (pos >= len)
        {
            return 0;
        }
    }
}

int is_word(gchar* w)
{
    gchar* p = w;
    int len = g_utf8_strlen(w, -1);
    int pos = 0;
    
    while (1)
    {
        guint32 ch = g_utf8_get_char(p);
        
        if ((ch >= 0x61 && ch <= 0x7a) || (ch >= 0x30 && ch <= 0x39)) // a-z, 0-9
        {
            return 1;
        }
        else if (ch == 0xe4 || ch == 0xe5 || ch == 0xf6) // äåö
        {
            return 1;
        }
        
        p = g_utf8_next_char(p);
        ++pos;
        
        if (pos >= len)
        {
            return 0;
        }
    }
}

int is_number(gchar* w)
{
    gchar* p = w;
    int len = g_utf8_strlen(w, -1);
    int pos = 0;
    
    while (1)
    {
        guint32 ch = g_utf8_get_char(p);
        
        if (!((ch >= 0x30 && ch <= 0x39) || ch == 0x2c || ch == 0x2e)) // 0-9, komma, punkt
        {
            return 0;
        }
        
        p = g_utf8_next_char(p);
        ++pos;
        
        if (pos >= len)
        {
            return 1;
        }
    }
}

struct processState
{
    int in_sentence;
    int in_word;
    long num_sentences;
    long num_skipped;
    gchar** current_sentence;
    int current_length;
    int skip;
    
    FILE* out_words;
    FILE* out_bistarts;
    FILE* out_tristarts;
    FILE* out_bimids;
    FILE* out_trimids;
    FILE* out_biends;
    FILE* out_triends;
};

typedef struct processState processState_t;

void processNode(xmlTextReaderPtr reader, processState_t* procstate)
{
    xmlChar* tagname;
    xmlChar* text;
    
    switch (xmlTextReaderNodeType(reader))
    {
        case XML_NODETYPE_ELEMENT_START:
            tagname = xmlTextReaderName(reader);
            
            if (procstate->in_sentence == 0 && xmlStrEqual(tagname, "sentence"))
            {
                procstate->in_sentence = 1;
            }
            
            if (procstate->in_sentence == 1 && procstate->in_word == 0 && xmlStrEqual(tagname, "w"))
            {
                procstate->in_word = 1;
            }
            
            xmlFree(tagname);
            break;
        
        case XML_NODETYPE_ELEMENT_END:
            tagname = xmlTextReaderName(reader);
            
            if (procstate->in_sentence == 1 && xmlStrEqual(tagname, "sentence"))
            {
                procstate->in_sentence = 0;
                
                if (procstate->skip)
                {
                    procstate->skip = 0;
                    xmlFree(tagname);
                    return;
                }
                
                if (procstate->current_length > 2)
                {
                    int len = procstate->current_length;
                    
                    // todo: gör detta generellt och använd commandline args för att välja vad som
                    //       ska genereras
                    
                    fwrite(procstate->current_sentence[0],
                        strlen(procstate->current_sentence[0]), 1, procstate->out_bistarts);
                    fwrite(" ", 1, 1, procstate->out_bistarts);
                    fwrite(procstate->current_sentence[1],
                        strlen(procstate->current_sentence[1]), 1, procstate->out_bistarts);
                    fwrite("\n", 1, 1, procstate->out_bistarts);
                    
                    fwrite(procstate->current_sentence[0],
                        strlen(procstate->current_sentence[0]), 1, procstate->out_tristarts);
                    fwrite(" ", 1, 1, procstate->out_tristarts);
                    fwrite(procstate->current_sentence[1],
                        strlen(procstate->current_sentence[1]), 1, procstate->out_tristarts);
                    fwrite(" ", 1, 1, procstate->out_tristarts);
                    fwrite(procstate->current_sentence[2],
                        strlen(procstate->current_sentence[2]), 1, procstate->out_tristarts);
                    fwrite("\n", 1, 1, procstate->out_tristarts);
                    
                    fwrite(procstate->current_sentence[len - 2],
                        strlen(procstate->current_sentence[len - 2]), 1, procstate->out_biends);
                    fwrite(" ", 1, 1, procstate->out_biends);
                    fwrite(procstate->current_sentence[len - 1],
                        strlen(procstate->current_sentence[len - 1]), 1, procstate->out_biends);
                    fwrite("\n", 1, 1, procstate->out_biends);
                    
                    fwrite(procstate->current_sentence[len - 3],
                        strlen(procstate->current_sentence[len - 3]), 1, procstate->out_triends);
                    fwrite(" ", 1, 1, procstate->out_triends);
                    fwrite(procstate->current_sentence[len - 2],
                        strlen(procstate->current_sentence[len - 2]), 1, procstate->out_triends);
                    fwrite(" ", 1, 1, procstate->out_triends);
                    fwrite(procstate->current_sentence[len - 1],
                        strlen(procstate->current_sentence[len - 1]), 1, procstate->out_triends);
                    fwrite("\n", 1, 1, procstate->out_triends);
                    
                    for (int i = 0; i < procstate->current_length; ++i)
                    {
                        fwrite(procstate->current_sentence[i],
                            strlen(procstate->current_sentence[i]), 1,
                            procstate->out_words);
                        fwrite("\n", 1, 1, procstate->out_words);
                    }
                    
                    for (int i = 0; i < procstate->current_length - 1; ++i)
                    {
                        fwrite(procstate->current_sentence[i],
                            strlen(procstate->current_sentence[i]), 1,
                            procstate->out_bimids);
                        fwrite(" ", 1, 1, procstate->out_bimids);
                        
                        fwrite(procstate->current_sentence[i + 1],
                            strlen(procstate->current_sentence[i + 1]), 1,
                            procstate->out_bimids);
                        fwrite("\n", 1, 1, procstate->out_bimids);
                    }
                    
                    for (int i = 0; i < procstate->current_length - 2; ++i)
                    {
                        fwrite(procstate->current_sentence[i],
                            strlen(procstate->current_sentence[i]), 1,
                            procstate->out_trimids);
                        fwrite(" ", 1, 1, procstate->out_trimids);
                        fwrite(procstate->current_sentence[i + 1],
                            strlen(procstate->current_sentence[i + 1]), 1,
                            procstate->out_trimids);
                        fwrite(" ", 1, 1, procstate->out_trimids);
                        fwrite(procstate->current_sentence[i + 2],
                            strlen(procstate->current_sentence[i + 2]), 1,
                            procstate->out_trimids);
                        fwrite("\n", 1, 1, procstate->out_trimids);
                    }
                    
                    ++procstate->num_sentences;
                    
                    if ((procstate->num_sentences % 1000) == 0)
                    {
                        printf("%d sentences (%d skipped)\n", procstate->num_sentences, procstate->num_skipped);
                    }
                }
                
                procstate->current_length = 0;
            }
            
            if (procstate->in_sentence == 1 && procstate->in_word == 1 && xmlStrEqual(tagname, "w"))
            {
                procstate->in_word = 0;
            }
            
            xmlFree(tagname);
            break;
        
        case XML_NODETYPE_ELEMENT_TEXT:
            if (procstate->in_word == 1 && !procstate->skip)
            {
                text = xmlTextReaderValue(reader);
                
                if (g_utf8_strlen(text, -1) > WORD_MAX_LENGTH)
                {
                    // printf("skip %s\n", text);
                    procstate->skip = 1;
                    procstate->num_skipped += 1;
                    xmlFree(text);
                    return;
                }
                
                if (text != NULL)
                {
                    gchar* w = g_utf8_strdown(text, -1);
                    
                    if (is_word(w))
                    {
                        if (is_number(w))
                        {
                            gchar* numstr = malloc(sizeof(gchar) * 7);
                            numstr[6] = 0;
                            
                            strcpy(numstr, "NUMBER");
                            
                            procstate->current_sentence[procstate->current_length] = numstr;
                        }
                        else
                        {
                            if (causes_rejected_sentence(w))
                            {
                                // printf("skip %s\n", text);
                                procstate->skip = 1;
                                procstate->num_skipped += 1;
                                g_free(w);
                                xmlFree(text);
                                return;
                            }
                            
                            procstate->current_sentence[procstate->current_length] = w;
                        }
                        
                        ++procstate->current_length;
                    }
                    else
                    {
                        g_free(w);
                    }
                }
                
                xmlFree(text);
            }
            
            break;
        
        default:
            break;
    }
}

int main(int argc, char** argv)
{
    processState_t procstate;
    xmlTextReaderPtr reader;
    int ret;
    
    procstate.in_sentence = 0;
    procstate.in_word = 0;
    procstate.num_sentences = 0;
    procstate.current_sentence = malloc(sizeof(gchar*) * 1000);
    procstate.current_length = 0;
    procstate.skip = 0;
    procstate.num_skipped = 0;
    
    for (int i = 0; i < 1000; ++i)
    {
        procstate.current_sentence[i] = 0;
    }
    
    procstate.out_words = fopen("sbc_words.txt", "w+");
    procstate.out_bistarts = fopen("sbc_bistarts.txt", "w+");
    procstate.out_tristarts = fopen("sbc_tristarts.txt", "w+");
    procstate.out_bimids = fopen("sbc_bimids.txt", "w+");
    procstate.out_trimids = fopen("sbc_trimids.txt", "w+");
    procstate.out_biends = fopen("sbc_biends.txt", "w+");
    procstate.out_triends = fopen("sbc_triends.txt", "w+");
    
    reader = xmlReaderForFile(argv[1], "utf-8", XML_PARSE_RECOVER);
    
    ret = xmlTextReaderRead(reader);
    
    while (ret == 1)
    {
        processNode(reader, &procstate);
        ret = xmlTextReaderRead(reader);
    }
    
    xmlFreeTextReader(reader);
    
    fclose(procstate.out_words);
    fclose(procstate.out_bistarts);
    fclose(procstate.out_tristarts);
    fclose(procstate.out_bimids);
    fclose(procstate.out_trimids);
    fclose(procstate.out_biends);
    fclose(procstate.out_triends);
    
    printf("%d sentences (%d skipped)\n", procstate.num_sentences, procstate.num_skipped);
}
