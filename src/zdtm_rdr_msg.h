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

#ifndef _ZDTM_RDR_MSG_H_
#define _ZDTM_RDR_MSG_H_ 1

#include "zdtm_common.h"

/**
 * Desktop RDR message content.
 *
 * The zdtm_rdr_msg_content represents an RDR Desktop to Zaurus message
 * solicits ADR messages, one message for each specific item.
 *      - sync_type
 *          - todo 0x06
 *          - calendar 0x01
 *          - address book 0x07
 *      - num_sync_ids Usually 1
 *      - sync_id
 */

struct zdtm_rdr_msg_content {
    unsigned char sync_type;
    uint16_t num_sync_ids;
    uint32_t sync_id;
};

extern const char *RDR_MSG_TYPE;
#define IS_RDR(x) (memcmp(x->body.type, RDR_MSG_TYPE, MSG_TYPE_SIZE) == 0)

inline int zdtm_rdr_length(struct zdtm_rdr_msg_content *rdr);
inline void *zdtm_rdr_write(void *buf, struct zdtm_rdr_msg_content *rdr);


#endif
