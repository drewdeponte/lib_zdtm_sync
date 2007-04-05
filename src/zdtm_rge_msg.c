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

#include "zdtm_rge_msg.h"

const char *RGE_MSG_TYPE = "RGE";

int zdtm_rge_length(struct zdtm_rge_msg_content *rge){
    return sizeof(rge->path_len) + rge->path_len;
}

void *zdtm_rge_write(void *buf, struct zdtm_rge_msg_content *rge){ 
    *((uint16_t*)buf) = rge->path_len;
    buf += sizeof(rge->path_len);

    memcpy(buf, rge->path, rge->path_len);
    buf += rge->path_len;

    return buf;
}

