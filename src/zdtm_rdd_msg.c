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

#include "zdtm_rdd_msg.h"

const char *RDD_MSG_TYPE = "RDD";

int zdtm_rdd_length(struct zdtm_rdd_msg_content *rdd){
    return sizeof(rdd->sync_type) +
           sizeof(rdd->num_sync_ids) +
           sizeof(rdd->sync_id);
}

void *zdtm_rdd_write(void *buf, struct zdtm_rdd_msg_content *rdd){
    *((unsigned char*)buf++) = rdd->sync_type;

#ifdef WORDS_BIGENDIAN
    *((uint16_t*)buf) = zdtm_liltobigs(rdd->num_sync_ids);
    buf += sizeof(uint16_t);

    *((uint32_t*)buf) = zdtm_liltobigs(rdd->sync_id);
    buf += sizeof(uint32_t);

#else
    *((uint16_t*)buf) = rdd->num_sync_ids;
    buf += sizeof(uint16_t);

    *((uint32_t*)buf) = rdd->sync_id;
    buf += sizeof(uint32_t);

#endif

    return buf;
}

