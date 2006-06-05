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

// Return values
#define RET_NNULL_RAW     -7
#define RET_SIZE_MISMATCH -8
#define RET_MEM_CONTENT   -9
#define RET_UNK_TYPE      -10
#define RET_BAD_SIZE      -11
#define RET_MALLOC_FAIL   -13
#define RET_UNK_VAR      -12

// Sync Types
#define SYNC_TYPE_CALENDAR  0x01
#define SYNC_TYPE_TODO      0x06
#define SYNC_TYPE_ADDRESS   0x07

// This is the static message header for the Zaurus
const unsigned char ZMSG_HDR[MSG_HDR_SIZE] =
{0x00, 0x00, 0x00, 0x00, 0x00, 0x96, 0x01, 0x01, 0x00, 0xff, 0xff, 0xff, 0xff};

// This is the static message header for the Desktop
// The two 0xff bytes are to be replaced by the message content size later
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
} zdtm_lib_env;

/*
 * ### This section denotes the start of the definitions of the specific
 * ### message content type definitions which are designed to be used
 * ### within a union inside the message type.
 */

/**
 * zdtm_todo contains the information for a todo message. 
 */ 
struct zdtm_todo{
    uint32_t category_len;
    char *category;
    char start_date[5];
    char due_date[5];
    char completed_date[5];
    char progress;
    char priority;
    uint32_t description_len;
    char *description;
    uint32_t notes_len;
    char *notes;
};

/**
 * Zaurus AAY message content.
 *
 * The zdtm_aay_msg_content is a structure which represents an AAY
 * Zaurus message content after being parsed from the raw message
 * content.
 */
struct zdtm_aay_msg_content {
    unsigned char uk_data_0[3]; // general unknown data
};

const char *AAY_MSG_TYPE = "AAY";
#define IS_AAY(x) (memcmp(x->body.type, AAY_MSG_TYPE, MSG_TYPE_SIZE) == 0)

/**
 * Zaurus AIG message content.
 *
 * The zdtm_aig_msg_content is a structure which represents an AIG
 * Zaurus message content after being parsed from the raw message
 * content.
 */
struct zdtm_aig_msg_content {
    uint16_t model_str_len;     // length of the model string
    unsigned char *model_str;   // string of the zaurus model info
    unsigned char uk_data_0[5]; // general unknown data
    unsigned char language[2];  // an identifier of the zaurus lang
    unsigned char auth_state;   // zaurus authentication state
    unsigned char uk_data_1[6]; // general unknown data
};

const char *AIG_MSG_TYPE = "AIG";
#define IS_AIG(x) (memcmp(x->body.type, AIG_MSG_TYPE, MSG_TYPE_SIZE) == 0)

/**
 * Zaurus AMG message content.
 *
 * The zdtm_amg_msg_content is a structure which represents an AMG
 * Zaurus message content after being parsed from the raw message
 * content.
 */
struct zdtm_amg_msg_content {

};
const char *AMG_MSG_TYPE = "AMG";
#define IS_AMG(x) (memcmp(x->body.type, AMG_MSG_TYPE, MSG_TYPE_SIZE) == 0)

/**
 * Zaurus ATG message content.
 *
 * The zdtm_atg_msg_content is a structure which represents an ATG
 * Zaurus message content after being parsed from the raw message
 * content.
 */
struct zdtm_atg_msg_content {
    unsigned char year[4];
    unsigned char month[2];
    unsigned char day[2];
    unsigned char hour[2];
    unsigned char minutes[2];
    unsigned char seconds[2];
};

const char *ATG_MSG_TYPE = "ATG";
#define IS_ATG(x) (memcmp(x->body.type, ATG_MSG_TYPE, MSG_TYPE_SIZE) == 0)

/**
 * Zaurus AEX message content.
 *
 * The zdtm_aex_msg_content is a structure which represents an AEX
 * Zaurus message content after being parsed from the raw message
 * content.
 */
struct zdtm_aex_msg_content {

};
const char *AEX_MSG_TYPE = "AEX";
#define IS_AEX(x) (memcmp(x->body.type, AEX_MSG_TYPE, MSG_TYPE_SIZE) == 0)

/**
 * Desktop RAY message content.
 *
 * The zdtm_ray_msg_content represents an RAY Desktop to Zaurus message
 * in response to the AAY message.  It contains no specific content.
 */
struct zdtm_ray_msg_content {

};

const char *RAY_MSG_TYPE = "RAY";
#define IS_RAY(x) (memcmp(x->body.type, RAY_MSG_TYPE, MSG_TYPE_SIZE) == 0)

/**
 * Desktop RIG message content.
 *
 * The zdtm_rig_msg_content represents an RIG Desktop to Zaurus message
 * to request an AIG information packet.
 */
struct zdtm_rig_msg_content {
};

const char *RIG_MSG_TYPE = "RIG";
#define IS_RIG(x) (memcmp(x->body.type, RIG_MSG_TYPE, MSG_TYPE_SIZE) == 0)

/**
 * Desktop RRL message content.
 *
 * The zdtm_rrl_msg_content represents an RRL Desktop to Zaurus message
 * for authentication.
 */
struct zdtm_rrl_msg_content {
    unsigned char pw_size;
    char *pw;
};

const char *RRL_MSG_TYPE = "RRL";
#define IS_RRL(x) (memcmp(x->body.type, RRL_MSG_TYPE, MSG_TYPE_SIZE) == 0)

/**
 * Desktop RMG message content.
 *
 * The zdtm_rmg_msg_content represents an RMG Desktop to Zaurus message
 * to indicate the synchronization type.
 *      - uk is typically 0x01
 *      - sync_type
 *          - todo 0x06
 *          - calendar 0x01
 *          - address book 0x07
 * Synchronization log.
 */

struct zdtm_rmg_msg_content {
    unsigned char uk;
    unsigned char sync_type;
};

const char *RMG_MSG_TYPE = "RMG";
#define IS_RMG(x) (memcmp(x->body.type, RMG_MSG_TYPE, MSG_TYPE_SIZE) == 0)


/**
 * Desktop RMS message content.
 *
 * The zdtm_rms_msg_content represents an RMS Desktop to Zaurus message
 * in the process of doing a full synchronization.
 * Not really implemented at the moment.
 * First two bytes are size, the remaining 38 are log message.
 * The message is padded with 0x00 if it is not the full size.
 */

struct zdtm_rms_msg_content {
    uint16_t log_size;
    unsigned char log[38];
};

const char *RMS_MSG_TYPE = "RMS";
#define IS_RMS(x) (memcmp(x->body.type, RMS_MSG_TYPE, MSG_TYPE_SIZE) == 0)

/**
 * Desktop RTG message content.
 *
 * The zdtm_rtg_msg_content represents an RTG Desktop to Zaurus message
 * Request for time stamp.
 */

struct zdtm_rtg_msg_content {
};

const char *RTG_MSG_TYPE = "RTG";
#define IS_RTG(x) (memcmp(x->body.type, RTG_MSG_TYPE, MSG_TYPE_SIZE) == 0)

/**
 * Desktop RTS message content.
 *
 * The zdtm_rts_msg_content represents an RTS Desktop to Zaurus message
 * notify Zaurus of the time.
 *
 * String of YYYYMMDDhhmmss
 */

#define RTS_DATE_LEN 14

struct zdtm_rts_msg_content {
    char date[RTS_DATE_LEN];
};

const char *RTS_MSG_TYPE = "RTS";
#define IS_RTS(x) (memcmp(x->body.type, RTS_MSG_TYPE, MSG_TYPE_SIZE) == 0)

/**
 * Desktop RDI message content.
 *
 * The zdtm_rdi_msg_content represents an RDI Desktop to Zaurus message
 * solicits ADI message.
 *      - sync_type
 *          - todo 0x06
 *          - calendar 0x01
 *          - address book 0x07
 *      - uk is typically 0x07
 */

struct zdtm_rdi_msg_content {
    unsigned char sync_type;
    unsigned char uk;
};

const char *RDI_MSG_TYPE = "RDI";
#define IS_RDI(x) (memcmp(x->body.type, RDI_MSG_TYPE, MSG_TYPE_SIZE) == 0)

/**
 * Desktop RSY message content.
 *
 * The zdtm_rsy_msg_content represents an RSY Desktop to Zaurus message
 * solicits ASY message.
 *      - sync_type
 *          - todo 0x06
 *          - calendar 0x01
 *          - address book 0x07
 *      - uk is typically 0x07
 */

struct zdtm_rsy_msg_content {
    unsigned char sync_type;
    unsigned char uk;
};

const char *RSY_MSG_TYPE = "RSY";
#define IS_RSY(x) (memcmp(x->body.type, RSY_MSG_TYPE, MSG_TYPE_SIZE) == 0)

/**
 * Desktop RDR message content.
 *
 * The zdtm_rdr_msg_content represents an RDR Desktop to Zaurus message
 * solicits ADR messages, one message for each specific item.
 *      - sync_type
 *          - todo 0x06
 *          - calendar 0x01
 *          - address book 0x07
 *      - num_sync_ids Usually 1
 *      - sync_id
 */

struct zdtm_rdr_msg_content {
    unsigned char sync_type;
    uint16_t num_sync_ids;
    uint32_t sync_id;
};

const char *RDR_MSG_TYPE = "RDR";
#define IS_RDR(x) (memcmp(x->body.type, RDR_MSG_TYPE, MSG_TYPE_SIZE) == 0)

/**
 * Desktop RDW message content.
 *
 * The zdtm_rdw_msg_content represents an RDW Desktop to Zaurus message
 * used for updating or adding items.
 *
 *    - variation -- added to help the preparation process.
 *
 */


struct zdtm_rdw_msg_content {
    unsigned char sync_type;
    uint16_t num_sync_ids;
    uint32_t sync_id;
    unsigned char variation;
    union zdtm_rdw_variations {
        struct {
            unsigned char padding[16];
        } one;

        struct {
            char attribute;
        } two;

        struct {
            char attribute;
            char card_created_date_time[5];
            char card_mod_date_time[5];
            uint32_t sync_id;
        } three;
    } vars;

    union {
        struct zdtm_todo todo;
    } cont;
};

const char *RDW_MSG_TYPE = "RDW";
#define IS_RDW(x) (memcmp(x->body.type, RDW_MSG_TYPE, MSG_TYPE_SIZE) == 0)

/**
 * Desktop RDD message content.
 *
 * The zdtm_rdd_msg_content represents an RDD Desktop to Zaurus message
 * indicates that an item is to be deleted during synchronizaion.
 *
 *      - num_sync_ids is always 1, like a happy vestigial organ.
 */

struct zdtm_rdd_msg_content {
    unsigned char sync_type;
    uint16_t num_sync_ids;
    uint32_t sync_id;
};

const char *RDD_MSG_TYPE = "RDD";
#define IS_RDD(x) (memcmp(x->body.type, RDD_MSG_TYPE, MSG_TYPE_SIZE) == 0)

/**
 * Desktop RDS message content.
 *
 * The zdtm_rds_msg_content represents an RDS Desktop to Zaurus message
 * indicates that synchronization was successful. 
 *
 * status is always 0x7.
 */

struct zdtm_rds_msg_content {
    unsigned char sync_type;
    char status;
    char null_bytes[2];
};

const char *RDS_MSG_TYPE = "RDS";
#define IS_RDS(x) (memcmp(x->body.type, RDS_MSG_TYPE, MSG_TYPE_SIZE) == 0)

/**
 * Desktop RQT message content.
 *
 * The zdtm_rqt_msg_content represents an RQT Desktop to Zaurus message
 * terminates the zaurus connection.
 */

struct zdtm_rqt_msg_content {
    char null_bytes[3];
};

const char *RQT_MSG_TYPE = "RQT";
#define IS_RQT(x) (memcmp(x->body.type, RQT_MSG_TYPE, MSG_TYPE_SIZE) == 0)

/**
 * Desktop RLR message content.
 *
 * The zdtm_rlr_msg_content represents an RLR Desktop to Zaurus message
 * requests all synchronization ids of a given type.
 *
 */

struct zdtm_rlr_msg_content {
    unsigned char sync_type;
};

const char *RLR_MSG_TYPE = "RLR";
#define IS_RLR(x) (memcmp(x->body.type, RLR_MSG_TYPE, MSG_TYPE_SIZE) == 0)

/**
 * Desktop RGE message content.
 *
 * The zdtm_rge_msg_content represents an RGE Desktop to Zaurus message
 * requests the contents of a DTM index file or a DTM box file.
 *
 */

struct zdtm_rge_msg_content {
    uint16_t path_len;
    char *path;
};

const char *RGE_MSG_TYPE = "RGE";
#define IS_RGE(x) (memcmp(x->body.type, RGE_MSG_TYPE, MSG_TYPE_SIZE) == 0)

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

uint16_t zdtm_liltobigs(uint16_t lilshort);
uint32_t zdtm_liltobigl(uint32_t lillong);
uint16_t zdtm_bigtolils(uint16_t bigshort);
uint32_t zdtm_bigtolill(uint32_t biglong);

uint16_t zdtm_checksum(unsigned char *buf, uint16_t n);

int zdtm_listen_for_zaurus(zdtm_lib_env *cur_env);
int zdtm_handle_zaurus_conn(zdtm_lib_env *cur_env);
int zdtm_close_zaurus_conn(zdtm_lib_env *cur_env);
int zdtm_conn_to_zaurus(zdtm_lib_env *cur_env, const char *zaurus_ip);

int zdtm_open_log(zdtm_lib_env *cur_env);
int zdtm_write_log(zdtm_lib_env *cur_env, const unsigned char *buff,
    unsigned int size);
int zdtm_close_log(zdtm_lib_env *cur_env);

int zdtm_is_ack_message(const unsigned char *buff);
int zdtm_is_rqst_message(const unsigned char *buff);
int zdtm_is_abrt_message(const unsigned char *buff);

int zdtm_clean_message(zdtm_msg *p_msg);
int zdtm_recv_message(zdtm_lib_env *cur_env, zdtm_msg *p_msg);
int zdtm_prepare_message(zdtm_lib_env *cur_env, zdtm_msg *p_msg);
//int zdtm_send_message(zdtm_lib_env *cur_env, zdtm_msg *p_msg);

int zdtm_prepare_message(zdtm_lib_env *cur_env, zdtm_msg *p_msg);

int zdtm_parse_raw_msg(zdtm_msg *p_msg);
int zdtm_parse_raw_aay_msg(zdtm_msg *p_msg);
int zdtm_parse_raw_aig_msg(zdtm_msg *p_msg);

inline int zdtm_todo_length(struct zdtm_todo * todo);
inline void *zdtm_todo_write(void *buf, struct zdtm_todo *todo);

inline int zdtm_rrl_length(struct zdtm_rrl_msg_content *rrl);
inline void *zdtm_rrl_write(void *buf, struct zdtm_rrl_msg_content *rrl);

inline int zdtm_rmg_length(struct zdtm_rmg_msg_content *rmg);
inline void *zdtm_rmg_write(void *buf, struct zdtm_rmg_msg_content *rmg);

inline int zdtm_rms_length(struct zdtm_rms_msg_content *rms);
inline void *zdtm_rms_write(void *buf, struct zdtm_rms_msg_content *rms);

inline int zdtm_rms_length(struct zdtm_rms_msg_content *rms);
inline void *zdtm_rms_write(void *buf, struct zdtm_rms_msg_content *rms);

inline int zdtm_rts_length(struct zdtm_rts_msg_content *rts);
inline void *zdtm_rts_write(void *buf, struct zdtm_rts_msg_content *rts);

inline int zdtm_rdi_length(struct zdtm_rdi_msg_content *rdi);
inline void *zdtm_rdi_write(void *buf, struct zdtm_rdi_msg_content *rdi);

inline int zdtm_rsy_length(struct zdtm_rsy_msg_content *rsy);
inline void *zdtm_rsy_write(void *buf, struct zdtm_rsy_msg_content *rsy);

inline int zdtm_rdr_length(struct zdtm_rdr_msg_content *rdr);
inline void *zdtm_rdr_write(void *buf, struct zdtm_rdr_msg_content *rdr);

inline int zdtm_rdw_length(struct zdtm_rdw_msg_content *rdw);
inline void *zdtm_rdw_write(void *buf, struct zdtm_rdw_msg_content *rdw);

inline int zdtm_rdd_length(struct zdtm_rdd_msg_content *rdd);
inline void *zdtm_rdd_write(void *buf, struct zdtm_rdd_msg_content *rdd);

inline int zdtm_rds_length(struct zdtm_rds_msg_content *rds);
inline void *zdtm_rds_write(void *buf, struct zdtm_rds_msg_content *rds);

inline int zdtm_rqt_length(struct zdtm_rqt_msg_content *rqt);
inline void *zdtm_rqt_write(void *buf, struct zdtm_rqt_msg_content *rqt);

inline int zdtm_rlr_length(struct zdtm_rlr_msg_content *rlr);
inline void *zdtm_rlr_write(void *buf, struct zdtm_rlr_msg_content *rlr);

inline int zdtm_rge_length(struct zdtm_rge_msg_content *rge);
inline void *zdtm_rge_write(void *buf, struct zdtm_rge_msg_content *rge);

#endif
