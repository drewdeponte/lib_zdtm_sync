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

#include "zdtm_rdr_msg.h"
#include "zdtm_common.h"

const char *RDR_MSG_TYPE = "RDR";

int zdtm_rdr_length(struct zdtm_rdr_msg_content *rdr){
    /* Because there is a char early in the struct, it is being 
     * null by the compiler to get the other data types memory
     * aligned, therefore, we are going to size the individual 
     * components, rather than using gcc-isms to get the data
     * structure packed.
     * The alternative is to add a __attribute__((__packed__))
     * between the closing brace and the semicolon on the 
     * structure declaration.
     * TODO Do we need to do this on the other structures
     *      for cross-platform issues?
     */
    
    return sizeof(rdr->sync_type) +
           sizeof(rdr->num_sync_ids) +
           sizeof(rdr->sync_id);
}

void *zdtm_rdr_write(void *buf, struct zdtm_rdr_msg_content *rdr){
    *((unsigned char*)buf++) = rdr->sync_type;

#ifdef WORDS_BIGENDIAN
    *((uint16_t*)buf) = zdtm_liltobigs(rdr->num_sync_ids);
    buf += sizeof(uint16_t);

    *((uint32_t*)buf) = zdtm_liltobigs(rdr->sync_id);
    buf += sizeof(uint32_t);

#else
    *((uint16_t*)buf) = rdr->num_sync_ids;
    buf += sizeof(uint16_t);

    *((uint32_t*)buf) = rdr->sync_id;
    buf += sizeof(uint32_t);

#endif
    return buf;
}

