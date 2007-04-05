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

#ifndef _ZDTM_RDI_MSG_H_
#define _ZDTM_RDI_MSG_H_ 1

#include "zdtm_common.h"
/**
 * Desktop RDI message content.
 *
 * The zdtm_rdi_msg_content represents an RDI Desktop to Zaurus message
 * solicits ADI message.
 *      - sync_type
 *          - todo 0x06
 *          - calendar 0x01
 *          - address book 0x07
 *      - uk is typically 0x07
 */

struct zdtm_rdi_msg_content {
    unsigned char sync_type;
    unsigned char uk;
};

extern const char *RDI_MSG_TYPE;
#define IS_RDI(x) (memcmp(x->body.type, RDI_MSG_TYPE, MSG_TYPE_SIZE) == 0)

inline int zdtm_rdi_length(struct zdtm_rdi_msg_content *rdi);
inline void *zdtm_rdi_write(void *buf, struct zdtm_rdi_msg_content *rdi);


#endif
