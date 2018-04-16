/*
 *
 * Copyright (c) 2018 Cisco Systems, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above
 *   copyright notice, this list of conditions and the following
 *   disclaimer in the documentation and/or other materials provided
 *   with the distribution.
 *
 *   Neither the name of the Cisco Systems, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/**
 * \file joy_mem.c
 * \brief joy memory functions for debugging purposes.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "joy_mem.h"

#ifdef JOY_DEBUG_MEM

unsigned long memory_used = 0;
unsigned long highest_usage = 0;

static void joy_check_paint(unsigned char *ptr, unsigned int size) {
    int i;
    unsigned int end = 0;

    end = size + 8;
    if ((*(ptr) != 0xDE) || (*(ptr+1) != 0xAD) || 
        (*(ptr+2) != 0xBE) || (*(ptr+3) != 0xEF)) {
        for (i=0; i < 16; ++i) {
            fprintf(stderr,"%.2X",*(ptr+i));
        }
        fprintf(stderr," - tracks in the header paint,%p,%d\n",ptr,size);
    }
    if ((*(ptr+end) != 0xDE) || (*(ptr+end+1) != 0xAD) || 
        (*(ptr+end+2) != 0xBE) || (*(ptr+end+3) != 0xEF)) {
        for (i=0; i < 4; ++i) {
            fprintf(stderr,"%.2X",*(ptr+end+i));
        }
        fprintf(stderr," - tracks in the trailer paint,%p,%d\n",ptr,size);
    }
}

void *joy_dbg_malloc(char *f, unsigned int l, size_t size) {
    size_t new_size = size + 12;
    unsigned char *ptr = NULL;
    unsigned int sz = (unsigned int)size;

    /* allocate an extra 12 bytes */
    ptr = malloc((new_size));
    if (ptr != NULL) {
        memcpy(ptr,"\xDE\xAD\xBE\xEF",4); /* paint first 4 bytes */
        /* store off the size allocated */
        *(ptr+4) = (sz&0xFF);
        *(ptr+5) = ((sz>>8)&0xFF);
        *(ptr+6) = ((sz>>16)&0xFF);
        *(ptr+7) = ((sz>>24)&0xFF);
        memcpy((ptr+new_size-4),"\xDE\xAD\xBE\xEF",4); /* paint the last 4 bytes */
        fprintf(stderr,"Func:%s Line:%d PTR:%p Size:%d - malloc\n",f,l,ptr,(unsigned int)size);
        memory_used += sz;
        return((ptr+8));
    }
    return NULL;
}

void *joy_dbg_calloc(char *f, unsigned int l, size_t items, size_t size) {
    size_t new_size = (items * size) + 12;
    unsigned char *ptr = NULL;
    unsigned int sz = (unsigned int)(items * size);

    ptr = malloc(new_size);
    if (ptr != NULL) {
        memcpy(ptr,"\xDE\xAD\xBE\xEF",4); /* paint first 4 bytes */
        /* store off the size allocated */
        *(ptr+4) = (sz&0xFF);
        *(ptr+5) = ((sz>>8)&0xFF);
        *(ptr+6) = ((sz>>16)&0xFF);
        *(ptr+7) = ((sz>>24)&0xFF);
        memset((ptr+8), 0x00, sz);
        memcpy((ptr+new_size-4),"\xDE\xAD\xBE\xEF",4); /* paint the last 4 bytes */
        fprintf(stderr,"Func:%s Line:%d PTR:%p Size:%d - calloc\n",f,l,ptr,(unsigned int)(items*size));
        memory_used += sz;
        return((ptr+8));
    }
    return NULL;
}

void *joy_dbg_realloc(char *f, unsigned int l, void *ptr, size_t size) {
    unsigned int old_size = 0;
    size_t new_size = size + 12;
    unsigned char *new_ptr = NULL;
    unsigned char *free_ptr = NULL;
    unsigned int sz = (unsigned int)size;

    free_ptr = (((unsigned char*)ptr) - 8);
    old_size = (unsigned int)*(free_ptr+4);
    old_size += (unsigned int)*(free_ptr+5) << 8;
    old_size += (unsigned int)*(free_ptr+6) << 16;
    old_size += (unsigned int)*(free_ptr+7) << 24;

    fprintf(stderr,"Func:%s Line:%d PTR:%p Size:%d - realloc free\n",f,l,free_ptr,old_size);
    joy_check_paint(free_ptr, old_size);

    new_ptr = realloc(free_ptr, new_size);
    if (new_ptr != NULL) {
        memcpy(new_ptr,"\xDE\xAD\xBE\xEF",4); /* paint first 4 bytes */
        /* store off the size allocated */
        *(new_ptr+4) = (sz&0xFF);
        *(new_ptr+5) = ((sz>>8)&0xFF);
        *(new_ptr+6) = ((sz>>16)&0xFF);
        *(new_ptr+7) = ((sz>>24)&0xFF);
        memcpy((new_ptr+new_size-4),"\xDE\xAD\xBE\xEF",4); /* paint the last 4 bytes */
        fprintf(stderr,"Func:%s Line:%d PTR:%p Size:%d - realloc malloc\n",f,l,new_ptr,(unsigned int)size);
        if (highest_usage < memory_used) highest_usage = memory_used;
        memory_used -= old_size;
        memory_used += sz;
        return((new_ptr+8));
    }
    return NULL;
}

void joy_dbg_free(char *f, unsigned int l, void *ptr) {
    unsigned int size = 0;
    unsigned char *free_ptr = NULL;

    if (ptr == NULL) return;

    free_ptr = (((unsigned char*)ptr) - 8);
    size = (unsigned int)*(free_ptr+4);
    size += (unsigned int)*(free_ptr+5) << 8;
    size += (unsigned int)*(free_ptr+6) << 16;
    size += (unsigned int)*(free_ptr+7) << 24;

    if (highest_usage < memory_used) highest_usage = memory_used;
    memory_used -= size;
    fprintf(stderr,"Func:%s Line:%d PTR:%p Size:%d TOTAL USAGE: %lu HIGHEST USAGE: %lu - free\n",f,l,free_ptr,size,memory_used,highest_usage);
    joy_check_paint(free_ptr,size);

    free(free_ptr);
}

#endif

