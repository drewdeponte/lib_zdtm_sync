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

#ifndef _ZDTM_RIG_MSG_H_
#define _ZDTM_RIG_MSG_H_ 1

#include "zdtm_common.h"

/**
 * Desktop RIG message content.
 *
 * The zdtm_rig_msg_content represents an RIG Desktop to Zaurus message
 * to request an AIG information packet.
 */
struct zdtm_rig_msg_content {
};

extern const char *RIG_MSG_TYPE;
#define IS_RIG(x) (memcmp(x->body.type, RIG_MSG_TYPE, MSG_TYPE_SIZE) == 0)


#endif 
