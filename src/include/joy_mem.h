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
 * \joy_mem.h
 *
 * \brief joy memory functions for debugging purposes.
 *
 */

#ifndef JOY_MEM_H
#define JOY_MEM_H

#include <stdlib.h>
#include <stdio.h>

#ifdef JOY_DEBUG_MEM

/* joy debug memory routines */
void *joy_dbg_malloc(char *f, unsigned int l, size_t size);
void *joy_dbg_calloc(char *f, unsigned int l, size_t items,size_t size);
void *joy_dbg_realloc(char *f, unsigned int l, void *ptr, size_t size);
void joy_dbg_free(char *f, unsigned int l, void *ptr);

#define joy_malloc(a) joy_dbg_malloc((char*)__FUNCTION__,__LINE__,a)
#define joy_calloc(a,b) joy_dbg_calloc((char*)__FUNCTION__,__LINE__,a,b)
#define joy_realloc(a,b) joy_dbg_realloc((char*)__FUNCTION__,__LINE__,a,b)
#define joy_free(a) joy_dbg_free((char*)__FUNCTION__,__LINE__,a)

#else

/* use stdib routines */
#define joy_malloc malloc
#define joy_calloc calloc
#define joy_realloc realloc
#define joy_free free

#endif

#endif /* JOY_MEM_H */
