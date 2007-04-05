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

#ifndef _ZDTM_RDW_MSG_H_
#define _ZDTM_RDW_MSG_H_ 1

#include "zdtm_common.h"

/**
 * Desktop RDW message content.
 *
 * The zdtm_rdw_msg_content represents an RDW Desktop to Zaurus message
 * used for updating or adding items.
 *
 *    - variation -- added to help the preparation process.
 *
 */


struct zdtm_rdw_msg_content {
    unsigned char sync_type;
    uint16_t num_sync_ids;
    uint32_t sync_id;
    unsigned char variation;
    union zdtm_rdw_variations {
        struct {
            unsigned char padding[16];
        } one;

        struct {
            char attribute;
        } two;

        struct {
            char attribute;
            char card_created_date_time[5];
            char card_mod_date_time[5];
            uint32_t sync_id;
        } three;
    } vars;

    union {
        struct zdtm_todo todo;
    } cont;
};

extern const char *RDW_MSG_TYPE;
#define IS_RDW(x) (memcmp(x->body.type, RDW_MSG_TYPE, MSG_TYPE_SIZE) == 0)

inline int zdtm_rdw_length(struct zdtm_rdw_msg_content *rdw);
inline void *zdtm_rdw_write(void *buf, struct zdtm_rdw_msg_content *rdw);

#endif
