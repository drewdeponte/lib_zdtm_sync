/*
 * Copyright 2005-2006 Andrew De Ponte
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
 * along with os_zdtm_plugin; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

/**
 * @file zdtm.h
 * @brief This is a specifications file for a Zaurus DTM support library.
 *
 * The zdtm.h file is a specifications for a Zaurus DTM based
 * synchronization, general support library. It provides all prototypes
 * of the general functions used to perform the Zaurus side of a
 * synchronization.
 */

#ifndef ZDTM_H
#define ZDTM_H

// Standard Input/Output Includes
#include <stdio.h>

// Memory Includes
#include <string.h>

// Network Related Includes
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "config.h"

// This is the port that the Zaurus listens on waiting for a connection
// to initiate a synchronization from the Desktop.
#define ZLISTPORT 4244
// This is the port that the Desktop synchronization server listens on
// waiting for a connection from the Zaurus to perform a synchronization.
#define DLISTPORT 4245

// This is the predefined constant size a Zaurus DTM Message header.
#define MSG_HDR_SIZE 13

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
typedef struct zdtm_environment {
    int listenfd;
    int connfd;
} zdtm_lib_env;

/**
 * Zaurus DTM Message Body.
 *
 * The zdtm_message_body is a structure which represents the body of a
 * Zaurus DTM message.
 */
struct zdtm_message_body {
    unsigned char type[3];
    void *p_content;
};

/**
 * Zaurus DTM Message.
 *
 * The zdtm_message is a structure which represents a Zaurus DTM
 * Message.
 */
struct zdtm_message {
    char header[MSG_HDR_SIZE];
    struct zdtm_message_body body;
    unsigned short int body_size;
    unsigned short int check_sum;
    unsigned short int cont_size;
};

int zdtm_listen_for_zaurus(zdtm_lib_env *cur_env);
int zdtm_handle_zaurus_conn(zdtm_lib_env *cur_env);
int zdtm_close_zaurus_conn(zdtm_lib_env *cur_env);

#endif
