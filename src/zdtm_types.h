/*
 * Copyright 2005-2007 Andrew De Ponte
 * 
 * This file is part of lib_zdtm_sync.
 * 
 * lib_zdtm_sync is free software; you can redistribute it and/or
 * modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
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

/**
 * @file zdtm_types.h
 * @brief This is a specifiactions file for lib_zdtm_sync public types.
 *
 * The zdtm_types.h file is a specifications file which defines all the
 * public types that should by used by client applications with
 * lib_zdtm_sync.
 */

#ifndef ZDTM_TYPES_H
#define ZDTM_TYPES_H

#include "zdtm_export.h"
#include "zdtm_gentypes.h"

// This is the port that the Zaurus listens on waiting for a connection
// to initiate a synchronization from the Desktop.
#define ZLISTPORT 4244
// This is the port that the Desktop synchronization server listens on
// waiting for a connection from the Zaurus to perform a synchronization.
#define DLISTPORT 4245

// This is the size, in bytes,  of a Zaurus DTM Message header.
#define MSG_HDR_SIZE 13
// This is the size, in bytes,  of a Zaurus DTM Message type.
#define MSG_TYPE_SIZE 3
// This is the size, in bytes,  of a common messages.
#define COM_MSG_SIZE 7

#define IP_STR_SIZE 16

#define SYNC_TODO 0x06
#define SYNC_CALENDAR 0x01
#define SYNC_ADDRESSBOOK 0x07

/* This is a static message header to be used for messages that
 * originate from the Zaurus side of the synchronization. */
extern const unsigned char ZMSG_HDR[MSG_HDR_SIZE];

/* This is a static message header to be used for messages that
 * originate from the Desktop side of the synchronization. The two 0xff
 * bytes are to be replaced by the message content size later. */
extern const unsigned char DMSG_HDR[MSG_HDR_SIZE];

#define MSG_HDR_CONT_OFFSET 0x09

/**
 * Zaurus DTM library environment.
 *
 * The zdtm_lib_env is a type defined to represent the environment for a
 * given synchronization. It contains all the data that is required to
 * be shared among all the different zdtm library functions. Hence,
 * every synchronization must first have it's own non-shared (not shared
 * between synchronizations) environment before being able to use
 * the provided functions. Note: It does not have to be initialized in
 * any certain way, it can be filled with garbage as long as the
 * provided functions are called in the proper order.
 */
typedef struct ZDTM_EXPORT zdtm_environment {
    SOCKET listenfd;   // socket - listen for zaurus conn request
    SOCKET connfd;     // socket - connection from zaurus to desktop
    SOCKET reqfd;      // socket - connection to zaurus from the desktop
    FILE *logfp;    // file pointer - used as the log file.
    // General Device Information
    int retreived_device_info; // flag stating device info has been obtained
    char model[256];    // c-string to hold the devices model
    char language[2];   // language abreviation of the device
    unsigned char cur_auth_state;   // current authentication state
    char zaurus_ip[IP_STR_SIZE]; // zaurus IP address & flag for net backend
    unsigned char sync_type; // synchronization type
    int retrieved_sync_state; // flag stating sync state has been obtained 
    int todo_slow_sync_required; // flag if slow sync is required
    int calendar_slow_sync_required; // flag if slow sync is required
    int address_book_slow_sync_required; // flag if slow sync is required
} zdtm_lib_env;

#endif
