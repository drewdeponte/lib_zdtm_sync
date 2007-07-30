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

#ifndef _ZDTM_ADW_MSG_H_
#define _ZDTM_ADW_MSG_H_ 1

#include "zdtm_common.h"

struct zdtm_adw_msg_content {
    unsigned char uk[4];
    uint16_t num_sync_ids;
    uint32_t sync_id;
};

extern const char *ADW_MSG_TYPE;
#define IS_ADW(x) (memcmp(x->body.type, ADW_MSG_TYPE, MSG_TYPE_SIZE) == 0)

int zdtm_parse_raw_adw_msg(void *buf, struct zdtm_adw_msg_content *adw);

#endif /* _ZDTM_ADW_MSG_H_ */
