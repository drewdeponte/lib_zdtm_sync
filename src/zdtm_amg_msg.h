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

#ifndef ZDTM_AMG_MSG_H
#define ZDTM_AMG_MSG_H

#include "zdtm_common.h"

#define AMG_TODO_MASK 0x01
#define AMG_CAL_MASK 0x02
#define AMG_ADDR_MASK 0x04

/**
 * Zaurus AMG message content.
 *
 * The zdtm_amg_msg_content is a structure which represents an AMG
 * Zaurus message content after being parsed from the raw message
 * content.
 */
struct zdtm_amg_msg_content {
    /**
     * SL Buffer
     * 
     * The sl buffer holds the two ascii characters 'S' and 'L' that are
     * found as the first two bytes of the the AMG message's raw
     * content.
     */
    unsigned char sl[2];

    /**
     * Full Sync Flags
     *
     * The fullsync_flags is byte which is designed to be used with
     * bit-masking to determin what types of synchronizations require a
     * full (slow) synchronization. Hence, if the associated byt is set
     * (1) then it  is has been synced since the last manual State
     * Clearing. Hence, a full sync is NOT necessary if the bit is set
     * (1). The 1s bit is the flag for Todo data, the 2s bit is the flag
     * for the Calendar data, and the 4s bit is the flag for the Address
     * Book data.
     */
    unsigned char fullsync_flags;

    /**
     * Unknown Bytes
     *
     * This is a buffer which is simply here to hold the 46 unknown
     * bytes that follow in the raw AMG mesasge content.
     */
    unsigned char uk[46];
};
extern const char *AMG_MSG_TYPE;
#define IS_AMG(x) (memcmp(x->body.type, AMG_MSG_TYPE, MSG_TYPE_SIZE) == 0)

int zdtm_parse_raw_amg_msg(void *p_cont, struct zdtm_amg_msg_content *amg);

#endif
