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

#include "zdtm_rqt_msg.h"

const char *RQT_MSG_TYPE = "RQT";

int zdtm_rqt_length(struct zdtm_rqt_msg_content *rqt){
    return sizeof(rqt->null_bytes);
}

void *zdtm_rqt_write(void *buf, struct zdtm_rqt_msg_content *rqt){
    memcpy(buf, rqt->null_bytes, sizeof(rqt->null_bytes));
    buf += sizeof(rqt->null_bytes);
    return buf;
}
