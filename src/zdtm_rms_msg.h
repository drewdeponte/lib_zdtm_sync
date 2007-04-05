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

#ifndef _ZDTM_RMS_MSG_H_
#define _ZDTM_RMS_MSG_H_ 1

#include "zdtm_common.h"

/**
 * Desktop RMS message content.
 *
 * The zdtm_rms_msg_content represents an RMS Desktop to Zaurus message
 * in the process of doing a full synchronization.
 * Not really implemented at the moment.
 * First two bytes are size, the remaining 38 are log message.
 * The message is padded with 0x00 if it is not the full size.
 */

#define RMS_LOG_SIZE 38
 
struct zdtm_rms_msg_content {
    uint16_t log_size;
    unsigned char log[RMS_LOG_SIZE];
};

extern const char *RMS_MSG_TYPE;
#define IS_RMS(x) (memcmp(x->body.type, RMS_MSG_TYPE, MSG_TYPE_SIZE) == 0)

inline int zdtm_rms_length(struct zdtm_rms_msg_content *rms);
inline void *zdtm_rms_write(void *buf, struct zdtm_rms_msg_content *rms);


#endif
