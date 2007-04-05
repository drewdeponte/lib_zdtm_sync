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

#include "zdtm_types.h"

/* This is a static message header to be used for messages that
 * originate from the Zaurus side of the synchronization. */
const unsigned char ZMSG_HDR[MSG_HDR_SIZE] =
{0x00, 0x00, 0x00, 0x00, 0x00, 0x96, 0x01, 0x01, 0x00, 0xff, 0xff, 0xff, 0xff};

/* This is a static message header to be used for messages that
 * originate from the Desktop side of the synchronization. The two 0xff
 * bytes are to be replaced by the message content size later. */
const unsigned char DMSG_HDR[MSG_HDR_SIZE] =
{0x00, 0x00, 0x00, 0x00, 0x00, 0x96, 0x01, 0x01, 0x0c, 0xff, 0xff, 0x00, 0x00};
