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
 * along with lib_zdtm_sync; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

/**
 * @file zdtm_sync.h
 * @brief This is a specifications file for a Zaurus DTM support library.
 *
 * The zdtm_sync.h file is a specifications for a Zaurus DTM based
 * synchronization, general support library. It provides all prototypes
 * of the general functions used to perform a Zaurus DTM based
 * synchronization.
 */

#ifndef ZDTM_SYNC_H
#define ZDTM_SYNC_H

// Standard Input/Output Includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Memory Includes
#include <string.h>

// Network Related Includes
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Standard Integer Type Includes
#include <stdint.h>

#include "config.h"

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
    int listenfd;   // socket - listen for zaurus conn request
    int connfd;     // socket - connection between desktop and zaurus
    FILE *logfp;    // file pointer - used as the log file.
} zdtm_lib_env;

/**
 * Zaurus DTM Message Body.
 *
 * The zdtm_message_body is a structure which represents the body of a
 * Zaurus DTM message.
 */
struct zdtm_message_body {
    unsigned char type[MSG_TYPE_SIZE];  // type identifier for a message
    void *p_content;                    // content for a given message
};

/**
 * Zaurus DTM Message.
 *
 * The zdtm_message is a structure which represents a Zaurus DTM
 * Message.
 */
typedef struct zdtm_message {
    char header[MSG_HDR_SIZE];      // header of the message
    struct zdtm_message_body body;  // body of the msg (type and cont)
    uint16_t body_size;             // size of the msg body in bytes
    uint16_t check_sum;             // sum of each byte in msg body
    uint16_t cont_size;             // msg body size - msg type size
} zdtm_msg;

uint16_t zdtm_liltobigs(uint16_t lilshort);
uint32_t zdtm_liltobigl(uint32_t lillong);
uint16_t zdtm_bigtolils(uint16_t bigshort);
uint32_t zdtm_bigtolill(uint32_t biglong);

int zdtm_listen_for_zaurus(zdtm_lib_env *cur_env);
int zdtm_handle_zaurus_conn(zdtm_lib_env *cur_env);
int zdtm_close_zaurus_conn(zdtm_lib_env *cur_env);

int zdtm_open_log(zdtm_lib_env *cur_env);
int zdtm_write_log(zdtm_lib_env *cur_env, const unsigned char *buff,
    unsigned int size);
int zdtm_close_log(zdtm_lib_env *cur_env);

int zdtm_is_ack_message(const unsigned char *buff);
int zdtm_is_rqst_message(const unsigned char *buff);
int zdtm_is_abrt_message(const unsigned char *buff);

int zdtm_clean_message(zdtm_msg *p_msg);
int zdtm_recv_message(zdtm_lib_env *cur_env, zdtm_msg *p_msg);

#endif
