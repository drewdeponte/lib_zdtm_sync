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

#include "zdtm_rms_msg.h"
#include <string.h>

const char *RMS_MSG_TYPE = "RMS";

int zdtm_rms_length(struct zdtm_rms_msg_content *rms){
    return sizeof(struct zdtm_rms_msg_content);
}

void *zdtm_rms_write(void *buf, struct zdtm_rms_msg_content *rms){
        if(rms->log_size > sizeof(rms->log))
            return NULL;
        
#ifdef WORDS_BIGENDIAN            
        *((uint16_t*)buf) = zdtm_liltobigs(rms->log_size);
        buf += sizeof(uint16_t);
        
#else
        *((uint16_t*)buf) = rms->log_size;
        buf += sizeof(uint16_t);

#endif

        memcpy(buf, rms->log, rms->log_size);

        buf += rms->log_size;

        memset(buf, 0x00, (RMS_LOG_SIZE - rms->log_size));

        buf += (RMS_LOG_SIZE - rms->log_size);

        return buf;
}

