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

#include "zdtm_aay_msg.h"

const char *AAY_MSG_TYPE = "AAY";

/**
 * Parse a raw AAY message.
 *
 * The zdtm_parse_raw_aay_msg function takes a raw AAY message and
 * parses it into it's appropriate components and fills in the aay
 * content fields so that the data can be easily obtained at a later
 * point in time.
 * @param buf Pointer to AAY message raw content.
 * @param aay Pointer to struct to store parsed AAY message content in.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully parsed the aay message.
 */
int zdtm_parse_raw_aay_msg(void *buf, struct zdtm_aay_msg_content *aay) {
    memcpy((void *)aay->uk_data_0, buf, 3);
    return 0;
}
