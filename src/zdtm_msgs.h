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
 * @file zdtm_msgs.h
 * @brief This is a specifications file for general message handling.
 *
 * The zdtm_msgs.h file is a specifications file for a all the general
 * private message handling functions used inside lib_zdtm_sync.
 */

#ifndef ZDTM_MSGS_H
#define ZDTM_MSGS_H

#include "zdtm_types.h"

#include "zdtm_aay_msg.h"
#include "zdtm_adi_msg.h"
#include "zdtm_adr_msg.h"
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
#include "zdtm_rss_msg.h"

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
        struct zdtm_adr_msg_content adr;
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
        struct zdtm_rss_msg_content rss;
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

/* The private common message handling functions */

/**
 * Check if a message is an Ack message.
 *
 * The _zdtm_is_ack_message function checks if a message is an ack
 * message. This function is designed to test if the first n bytes are
 * equivalent, where n is the size in bytes of the common messages. This
 * would allow for buffers of greater length to be passed to this
 * function and still be evaluated true if the first n bytes of the
 * buffer are equivalent to the bytes of the ack common message.
 * @param buff Pointer to raw message to compare against.
 * @return An integer representing a boolean value.
 * @retval 1 True, the message is an Ack message.
 * @retval 0 False, the message is NOT an ack message.
 */
int _zdtm_is_ack_message(const unsigned char *buff);

/**
 * Check if a message is a Request message.
 *
 * The _zdtm_is_rqst_message function checks if a message is a request
 * message. This function is designed to test if the first n bytes are
 * equivalent, where n is the size in bytes of the common messages. This
 * would allow for buffers of greater length to be passed to this
 * function and still be evaluated true if the first n bytes of the
 * buffer are equivalent to the bytes of the rqst common message.
 * @param buff Pointer to raw message to compare against.
 * @return An integer representing a boolean value.
 * @retval 1 True, the message is a rqst message.
 * @retval 0 False, the message is NOT a rqst message.
 */
int _zdtm_is_rqst_message(const unsigned char *buff);

/**
 * Check if a message is an Abort message.
 *
 * The _zdtm_is_abrt_message function checks if a message is an abort
 * message. This function is designed to test if the first n bytes are
 * equivalent, where n is the size in bytes of the common messages. This
 * would allow for buffers of greater length to be passed to this
 * function and still be evaluated true if the first n bytes of the
 * buffer are equivalent to the bytes of the abort common message.
 * @param buff Pointer to raw message to compare against.
 * @return An integer representing a boolean value.
 * @retval 1 True, the message is an abrt message.
 * @retval 0 False, the message is NOT an abrt message.
 */
int _zdtm_is_abrt_message(const unsigned char *buff);

/* The private general message handling functions */

/**
 * Sum all the bytes in a buffer.
 *
 * @param buf The buffer to sum.
 * @param n The number of bytes in the buffer.
 * @return Summation of bytes in passed buffer.
 */
uint16_t _zdtm_checksum(zdtm_msg *p_msg);

/**
 * Clean Message
 *
 * The _zdtm_clean_message function handles the checking and freeing of
 * any members of the message that have been dynamically allocated. Such
 * dynamic allocation occurs in the _zdtm_recv_message function.
 * @param p_msg Pointer to a zdtm_message structure to free members of.
 * @return An integer representing success (zero) or failure (non-zero).
 */
int _zdtm_clean_message(zdtm_msg *p_msg);

/**
 * Prepare the raw message for sending based on the p_msg content.
 * Fills in the Message Header, body_size, cont_size, check_sum,
 * and p_raw_content in p_msg.
 *
 * The p_raw_content must be freed at a later point.
 *
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval RET_NNULL_RAW Failed, raw message not null.
 * @retval RET_UNK_TYPE Failed, unknown message type. 
 * @retval RET_BAD_SIZE Failed, size field bad.
 * @retval -1 Failed to allocate memory for raw content.
 * @return RET_UNK_VAR  Failed, unknown variation for RDW message.
 */
int _zdtm_prepare_message(zdtm_lib_env *cur_env, zdtm_msg *p_msg);

/**
 * Parse a raw message.
 *
 * The _zdtm_parse_raw_msg function is designed to take in a raw message
 * that has just been read from the network and parse it into the proper
 * components filling out the proper message type structure so that it may
 * easily be accessed at a later point in time.
 * @param p_msg Pointer to zdtm_message struct containing raw message.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval -1 Failed to parse AAY message.
 * @retval -2 Failed to parse AIG message.
 * @retval -3 Failed to parse AMG message.
 * @retval -4 Failed to parse ATG message.
 * @retval -5 Failed to parse AEX message.
 * @retval -6 Failed to parse ANG message.
 * @retval -7 Failed to parse ADI message.
 * @retval -8 Failed to parse ASY message.
 * @retval -9 Failed to parse ADR message.
 * @retval -10 Failed to parse ADW message.
 * @retval -11 Failed to parse ALR message.
 * @retval -12 Failed to parse AGE messaeg.
 * @retval -255 Failed, unkown message type.
 */
int _zdtm_parse_raw_msg(zdtm_msg *p_msg);


#endif
