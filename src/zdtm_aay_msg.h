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

#ifndef ZDTM_AAY_MSG_H
#define ZDTM_AAY_MSG_H

#include "zdtm_common.h"

/**
 * Zaurus AAY message content.
 *
 * The zdtm_aay_msg_content is a structure which represents an AAY
 * Zaurus message content after being parsed from the raw message
 * content.
 */
struct zdtm_aay_msg_content {
    unsigned char uk_data_0[3]; // general unknown data
};
extern const char *AAY_MSG_TYPE;
#define IS_AAY(x) (memcmp(x->body.type, AAY_MSG_TYPE, MSG_TYPE_SIZE) == 0)

int zdtm_parse_raw_aay_msg(void *buf, struct zdtm_aay_msg_content *aay);

#endif
