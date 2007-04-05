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

#include "zdtm_amg_msg.h"

const char *AMG_MSG_TYPE = "AMG";

int zdtm_parse_raw_amg_msg(void *p_cont, struct zdtm_amg_msg_content *amg) {
    memcpy((void *)amg->sl, p_cont, 2);
    p_cont = p_cont + 2;
    amg->fullsync_flags = *((unsigned char *)p_cont);
    p_cont = p_cont + 1;
    memcpy((void *)amg->uk, p_cont, 46);

    return 0;
}
