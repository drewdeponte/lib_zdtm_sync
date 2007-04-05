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

#include "zdtm_ang_msg.h"

const char *ANG_MSG_TYPE = "ANG";

/**
 * Parse a raw ANG message.
 *
 * The zdtm_parse_raw_ang_msg function takes a raw ANG message and
 * parses it into it's appropriate components and fills in the ang
 * content fields so that the data con be easily obtained at a later
 * point in time.
 * @param buf Pointer to ANG message raw content.
 * @param ang Pointer to struct to store parsed ANG message content in.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully parsed the ang message.
 */
int zdtm_parse_raw_ang_msg(void *buf, struct zdtm_ang_msg_content *ang) {
    ang->uk_data_0 = *(unsigned char *)buf;
    return 0;
}
