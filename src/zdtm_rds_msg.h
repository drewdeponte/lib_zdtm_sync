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

#ifndef _ZDTM_RDS_MSG_H_
#define _ZDTM_RDS_MSG_H_ 1

#include "zdtm_common.h"

/**
 * Desktop RDS message content.
 *
 * The zdtm_rds_msg_content represents an RDS Desktop to Zaurus message
 * indicates that synchronization was successful. 
 *
 * status is always 0x7.
 */

struct zdtm_rds_msg_content {
    unsigned char sync_type;
    char status;
    char null_bytes[2];
};

extern const char *RDS_MSG_TYPE;
#define IS_RDS(x) (memcmp(x->body.type, RDS_MSG_TYPE, MSG_TYPE_SIZE) == 0)

inline int zdtm_rds_length(struct zdtm_rds_msg_content *rds);
inline void *zdtm_rds_write(void *buf, struct zdtm_rds_msg_content *rds);


#endif
