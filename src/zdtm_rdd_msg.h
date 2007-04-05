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

#ifndef _ZDTM_RDD_MSG_H_
#define _ZDTM_RDD_MSG_H_ 1

#include "zdtm_common.h"


/**
 * Desktop RDD message content.
 *
 * The zdtm_rdd_msg_content represents an RDD Desktop to Zaurus message
 * indicates that an item is to be deleted during synchronizaion.
 *
 *      - num_sync_ids is always 1, like a happy vestigial organ.
 */

struct zdtm_rdd_msg_content {
    unsigned char sync_type;
    uint16_t num_sync_ids;
    uint32_t sync_id;
};

extern const char *RDD_MSG_TYPE;
#define IS_RDD(x) (memcmp(x->body.type, RDD_MSG_TYPE, MSG_TYPE_SIZE) == 0)

inline int zdtm_rdd_length(struct zdtm_rdd_msg_content *rdd);
inline void *zdtm_rdd_write(void *buf, struct zdtm_rdd_msg_content *rdd);


#endif
