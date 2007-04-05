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

#ifndef ZDTM_ASY_MSG_H
#define ZDTM_ASY_MSG_H

#include "zdtm_common.h"

/**
 * Zaurus ASY message content.
 *
 * The zdtm_asy_msg_content is a structure which represents an ASY
 * Zaurus message content after being parsed from the raw message
 * content.
 */
struct zdtm_asy_msg_content {
    unsigned char new_list_id;
    uint16_t num_new_sync_ids;  // Number of new sync ids.
    uint32_t *new_sync_ids;     // Array of new sync ids.
    unsigned char mod_list_id;
    uint16_t num_mod_sync_ids;  // Number of mod sync ids.
    uint32_t *mod_sync_ids;     // Array of mod sync ids.
    unsigned char del_list_id;
    uint16_t num_del_sync_ids;  // Number of del sync ids.
    uint32_t *del_sync_ids;     // Array of ndel sync ids.
};
extern const char *ASY_MSG_TYPE;
#define IS_ASY(x) (memcmp(x->body.type, ASY_MSG_TYPE, MSG_TYPE_SIZE) == 0)

int zdtm_parse_raw_asy_msg(void *buf, struct zdtm_asy_msg_content *asy);

#endif
