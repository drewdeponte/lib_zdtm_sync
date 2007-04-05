/*
 * Copyright 2005-2007 Andrew De Ponte
 * 
 * This file is part of lib_zdtm_sync.
 * 
 * lib_zdtm_sync is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or any
 * later version.
 * 
 * lib_zdtm_sync is distributed in the hopes that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with lib_zdtm_sync; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

#ifndef ZDTM_COMMON_H
#define ZDTM_COMMON_H

#include "zdtm_gentypes.h"
#include "zdtm_config.h"

// Return values
#define RET_NNULL_RAW     -7
#define RET_SIZE_MISMATCH -8
#define RET_MEM_CONTENT   -9
#define RET_UNK_TYPE      -10
#define RET_BAD_SIZE      -11
#define RET_UNK_VAR       -12
#define RET_MALLOC_FAIL   -13
#define RET_PARSE_RAW_FAIL -14

// Sync Types
#define SYNC_TYPE_CALENDAR  0x01
#define SYNC_TYPE_TODO      0x06
#define SYNC_TYPE_ADDRESS   0x07

// Data types
#define DATA_ID_TIME 0x04
#define DATA_ID_BIT 0x06
#define DATA_ID_WORD 0x08
#define DATA_ID_UCHAR 0x0b
#define DATA_ID_BARRAY 0x0c
#define DATA_ID_UTF8 0x11
#define DATA_ID_ULONG 0x12

uint16_t zdtm_liltobigs(uint16_t lilshort);
uint32_t zdtm_liltobigl(uint32_t lillong);
uint16_t zdtm_bigtolils(uint16_t bigshort);
uint32_t zdtm_bigtolill(uint32_t biglong);

/**
 * zdtm_todo contains the information for a todo message. 
 */ 
struct zdtm_todo{
    unsigned char attribute;
    char creation_date[5];
    char modification_date[5];
    uint32_t sync_id;
    uint32_t category_len;
    char *category;
    char start_date[5];
    char due_date[5];
    char completed_date[5];
    char progress;
    char priority;
    uint32_t description_len;
    char *description;
    uint32_t notes_len;
    char *notes;
};

inline int zdtm_todo_length(struct zdtm_todo * todo);
inline void *zdtm_todo_write(void *buf, struct zdtm_todo *todo);


#endif
