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

#ifndef _ZDTM_RSS_MSG_H_
#define _ZDTM_RSS_MSG_H_ 1

#include "zdtm_common.h"

/**
 * Desktop RSS message content.
 *
 * The zdtm_rss_msg_content represents an RSS Desktop to Zaurus message
 * in the process of doing a full synchronization.
 * Not really implemented at the moment.
 *
 *  [ UK (1 Byte) ] [ SYNC TYPE (1 Byte) ] [ UK (1 Byte) ]
 *  1. UK (1 Byte) - The meaning of this one byte is unknown. It has consistently been seen with a value of 0x01 in hex, despite variables such as synchronization type, etc.
 *  2. SYNC TYPE (1 Byte) - This byte represents the type of synchronization that is occurring. The known values in hex are as follows.
 *        1. To-Do 0x06.
 *        2. Calendar 0x01.
 *        3. Address Book 0x07.
 *  3. UK (1 Byte) - The meaning of this one byte is unknown. It has consistently been seen with a value of 0x01 in hex, despite variables such as synchronization type, etc.
 */

struct zdtm_rss_msg_content {
    unsigned char uk_1;
    unsigned char sync_type;
    unsigned char uk_2;
};

extern const char *RSS_MSG_TYPE;
#define IS_RSS(x) (memcmp(x->body.type, RSS_MSG_TYPE, MSG_TYPE_SIZE) == 0)

inline int zdtm_rss_length(struct zdtm_rss_msg_content *rss);
inline void *zdtm_rss_write(void *buf, struct zdtm_rss_msg_content *rss);


#endif
