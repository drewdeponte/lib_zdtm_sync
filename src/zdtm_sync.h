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

#include "zdtm_common.h"

// Zaurus Message Includes
#include "zdtm_aay_msg.h"
#include "zdtm_adi_msg.h"
#include "zdtm_aex_msg.h"
#include "zdtm_aig_msg.h"
#include "zdtm_amg_msg.h"
#include "zdtm_ang_msg.h"
#include "zdtm_asy_msg.h"
#include "zdtm_atg_msg.h"

#include "zdtm_ray_msg.h"
#include "zdtm_rig_msg.h"
#include "zdtm_rrl_msg.h"
#include "zdtm_rmg_msg.h"
#include "zdtm_rms_msg.h"
#include "zdtm_rtg_msg.h"
#include "zdtm_rts_msg.h"
#include "zdtm_rdi_msg.h"
#include "zdtm_rsy_msg.h"
#include "zdtm_rdr_msg.h"
#include "zdtm_rdw_msg.h"
#include "zdtm_rdd_msg.h"
#include "zdtm_rds_msg.h"
#include "zdtm_rqt_msg.h"
#include "zdtm_rlr_msg.h"
#include "zdtm_rge_msg.h"

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


/* This is a static message header to be used for messages that
 * originate from the Zaurus side of the synchronization. */
const unsigned char ZMSG_HDR[MSG_HDR_SIZE] =
{0x00, 0x00, 0x00, 0x00, 0x00, 0x96, 0x01, 0x01, 0x00, 0xff, 0xff, 0xff, 0xff};

/* This is a static message header to be used for messages that
 * originate from the Desktop side of the synchronization. The two 0xff
 * bytes are to be replaced by the message content size later. */
const unsigned char DMSG_HDR[MSG_HDR_SIZE] =
{0x00, 0x00, 0x00, 0x00, 0x00, 0x96, 0x01, 0x01, 0x0c, 0xff, 0xff, 0x00, 0x00};

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
typedef struct zdtm_environment {
    int listenfd;   // socket - listen for zaurus conn request
    int connfd;     // socket - connection from zaurus to desktop
    int reqfd;      // socket - connection to zaurus from the desktop
    FILE *logfp;    // file pointer - used as the log file.
    // General Device Information
    char model[256];    // c-string to hold the devices model
    char language[2];   // language abreviation of the device
    unsigned char cur_auth_state;   // current authentication state
} zdtm_lib_env;

/**
 * Zaurus DTM Message Body.
 *
 * The zdtm_message_body is a structure which represents the body of a
 * Zaurus DTM message.
 */
struct zdtm_message_body {
    unsigned char type[MSG_TYPE_SIZE];  // type identifier for a message
    void *p_raw_content;                // content for a given message
    union zdtm_spec_type_content {
        // Content structures for Zaurus messages
        struct zdtm_aay_msg_content aay;
        struct zdtm_aig_msg_content aig;
        struct zdtm_amg_msg_content amg;
        struct zdtm_atg_msg_content atg;
        struct zdtm_aex_msg_content aex;
        struct zdtm_ang_msg_content ang;
        struct zdtm_adi_msg_content adi;
        struct zdtm_asy_msg_content asy;

        // Content structures for Qtopia Desktop messages
        struct zdtm_ray_msg_content ray;
        struct zdtm_rig_msg_content rig;
        struct zdtm_rrl_msg_content rrl;
        struct zdtm_rmg_msg_content rmg;
        struct zdtm_rms_msg_content rms;
        struct zdtm_rtg_msg_content rtg;
        struct zdtm_rts_msg_content rts;
        struct zdtm_rdi_msg_content rdi;
        struct zdtm_rsy_msg_content rsy;
        struct zdtm_rdr_msg_content rdr;
        struct zdtm_rdw_msg_content rdw;
        struct zdtm_rdd_msg_content rdd;
        struct zdtm_rds_msg_content rds;
        struct zdtm_rqt_msg_content rqt;
        struct zdtm_rlr_msg_content rlr;
        struct zdtm_rge_msg_content rge;
    } cont;
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

/* The private connection handling functions */
int _zdtm_listen_for_zaurus(zdtm_lib_env *cur_env);
int _zdtm_handle_zaurus_conn(zdtm_lib_env *cur_env);
int _zdtm_close_zaurus_conn(zdtm_lib_env *cur_env);
int _zdtm_conn_to_zaurus(zdtm_lib_env *cur_env, const char *zaurus_ip);
int _zdtm_close_conn_to_zaurus(zdtm_lib_env *cur_env);

/* The private log handling functions */
int _zdtm_open_log(zdtm_lib_env *cur_env);
int _zdtm_write_log(zdtm_lib_env *cur_env, const char *buff,
    unsigned int size);
int _zdtm_log_error(zdtm_lib_env *cur_env, const char *func_name, int err);
int _zdtm_close_log(zdtm_lib_env *cur_env);
int _zdtm_dump_msg_log(zdtm_lib_env *cur_env, zdtm_msg *p_msg);

/* The private common message handling functions */
int _zdtm_is_ack_message(const unsigned char *buff);
int _zdtm_send_comm_message_to(int sockfd, char *data);
int _zdtm_send_ack_message(zdtm_lib_env *cur_env);
int _zdtm_send_rqst_message(zdtm_lib_env *cur_env);
int _zdtm_send_abrt_message(zdtm_lib_env *cur_env);
int _zdtm_is_rqst_message(const unsigned char *buff);
int _zdtm_is_abrt_message(const unsigned char *buff);

/* The private general message handling functions */
uint16_t _zdtm_checksum(zdtm_msg *p_msg);
int _zdtm_clean_message(zdtm_msg *p_msg);
int _zdtm_recv_message(zdtm_lib_env *cur_env, zdtm_msg *p_msg);
int _zdtm_prepare_message(zdtm_lib_env *cur_env, zdtm_msg *p_msg);
int _zdtm_send_message(zdtm_lib_env *cur_env, zdtm_msg *p_msg);
int _zdtm_send_message_to(zdtm_lib_env *cur_env, zdtm_msg *p_msg, int sockfd);
int _zdtm_parse_raw_msg(zdtm_msg *p_msg);

/* The general public API functions are below this line */
int zdtm_initialize(zdtm_lib_env *cur_env);
int zdtm_finalize(zdtm_lib_env *cur_env);
int zdtm_connect(zdtm_lib_env *cur_env, const char *ip_addr);
int zdtm_send_message(zdtm_lib_env *cur_env, zdtm_msg *msg);
int zdtm_recv_message(zdtm_lib_env *cur_env, zdtm_msg *msg);
int zdtm_initiate_sync(zdtm_lib_env *cur_env);
int zdtm_obtain_device_info(zdtm_lib_env *cur_env);
int zdtm_disconnect(zdtm_lib_env *cur_env);

#endif
