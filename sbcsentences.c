//*neode.command* setgo ./sbcsentences.sh

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
    
    FILE* out_sentences;
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
                    
                    for (int i = 0; i < procstate->current_length - 1; ++i)
                    {
                        fwrite(procstate->current_sentence[i], strlen(procstate->current_sentence[i]), 1, procstate->out_sentences);
                        fwrite(" ", 1, 1, procstate->out_sentences);
                    }
                    
                    fwrite(procstate->current_sentence[len - 1], strlen(procstate->current_sentence[len - 1]), 1, procstate->out_sentences);
                    fwrite("\n", 1, 1, procstate->out_sentences);
                    
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
    
    procstate.out_sentences = fopen("sbc_sentences.txt", "a+");
    
    reader = xmlReaderForFile(argv[1], "utf-8", XML_PARSE_RECOVER);
    
    ret = xmlTextReaderRead(reader);
    
    while (ret == 1)
    {
        processNode(reader, &procstate);
        ret = xmlTextReaderRead(reader);
    }
    
    xmlFreeTextReader(reader);
    
    fclose(procstate.out_sentences);
}
