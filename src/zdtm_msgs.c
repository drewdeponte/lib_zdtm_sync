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
 * @file zdtm_msgs.c
 * @brief This is an implementation file for general message handling.
 *
 * The zdtm_msgs.c file is an implementation file for all the general
 * private message handling functions used inside lib_zdtm_sync.
 */

#include "zdtm_msgs.h"

int _zdtm_is_ack_message(const unsigned char *buff) {
    char msg_data[COM_MSG_SIZE] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x96,
        0x06};

    if (memcmp(buff, msg_data, COM_MSG_SIZE) == 0) {
        // return 1 stating that it is an ack message.
        return 1;
    }

    // return 0 stating that it is NOT an ack message.
    return 0;
}

int _zdtm_is_rqst_message(const unsigned char *buff) {
    char msg_data[COM_MSG_SIZE] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x96,
        0x05};

    if (memcmp(buff, msg_data, COM_MSG_SIZE) == 0) {
        // return 1 stating that it is a rqst message.
        return 1;
    }

    // return 0 stating that it is NOT a rqst message.
    return 0;
}

int _zdtm_is_abrt_message(const unsigned char *buff) {
    char msg_data[COM_MSG_SIZE] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x96,
        0x18};

    if (memcmp(buff, msg_data, COM_MSG_SIZE) == 0) {
        // return 1 stating that it is an abrt message.
        return 1;
    }

    // return 0 stating that it is NOT an abrt message.
    return 0;
}

uint16_t _zdtm_checksum(zdtm_msg *p_msg) {
    uint16_t n;
    unsigned char *buf;
    uint16_t sum = 0;

    for (n = 0; n < MSG_TYPE_SIZE; ++n) {
        sum += p_msg->body.type[n];
    }

    buf = (unsigned char *)p_msg->body.p_raw_content;
    for(n = p_msg->cont_size; n > 0; --n) {
        sum += *(buf++);
    }

    return sum;
}

int _zdtm_clean_message(zdtm_msg *p_msg) {
    if (p_msg->body.p_raw_content != NULL) {
        free(p_msg->body.p_raw_content);
        p_msg->body.p_raw_content = NULL;
    }

    /* Cleanup RRL Messages. */
    if (memcmp(p_msg->body.type, RRL_MSG_TYPE, MSG_TYPE_SIZE) == 0){
        if(p_msg->body.cont.rrl.pw != NULL){
            free(p_msg->body.cont.rrl.pw);
            p_msg->body.cont.rrl.pw = NULL;
        }
    }

    return 0;
}

int _zdtm_prepare_message(zdtm_lib_env *cur_env, zdtm_msg *p_msg) {
    
    void *p_body;
    uint16_t *p_cont_size;

    // First we will calculate the body size -- all messages have
    // the message type.
    p_msg->body_size = MSG_TYPE_SIZE;

    if(IS_RRL(p_msg)) {
        p_msg->body_size += zdtm_rrl_length(&p_msg->body.cont.rrl);

    }else if(IS_RMG(p_msg)) {
        p_msg->body_size += zdtm_rmg_length(&p_msg->body.cont.rmg);

    }else if(IS_RMS(p_msg)) {
        p_msg->body_size += zdtm_rms_length(&p_msg->body.cont.rms);

    }else if(IS_RTS(p_msg)) {
        p_msg->body_size += zdtm_rts_length(&p_msg->body.cont.rts); 

    }else if(IS_RDI(p_msg)) {
        p_msg->body_size += zdtm_rdi_length(&p_msg->body.cont.rdi);

    }else if(IS_RSY(p_msg)) {
        p_msg->body_size += zdtm_rsy_length(&p_msg->body.cont.rsy);

    }else if(IS_RDR(p_msg)) {
        p_msg->body_size += zdtm_rdr_length(&p_msg->body.cont.rdr);

    }else if(IS_RDW(p_msg)) {
        p_msg->body_size += zdtm_rdw_length(&p_msg->body.cont.rdw);

    }else if(IS_RDD(p_msg)){
        p_msg->body_size += zdtm_rdd_length(&p_msg->body.cont.rdd);

    }else if(IS_RDS(p_msg)){
        p_msg->body_size += zdtm_rds_length(&p_msg->body.cont.rds);

    }else if(IS_RQT(p_msg)){
        p_msg->body_size += zdtm_rqt_length(&p_msg->body.cont.rqt);

    }else if(IS_RLR(p_msg)){
        p_msg->body_size += zdtm_rlr_length(&p_msg->body.cont.rlr); 

    }else if(IS_RGE(p_msg)){
        p_msg->body_size += zdtm_rge_length(&p_msg->body.cont.rge);

    }else if(IS_RAY(p_msg) || IS_RIG(p_msg) || IS_RTG(p_msg)) {
        // No additional content 
        
    }else {
        // Unknown message type 
        return RET_UNK_TYPE;
    }

    // The cont_size is the body - the type size
    p_msg->cont_size = p_msg->body_size - MSG_TYPE_SIZE;

    // Set up the header
    memcpy(p_msg->header, DMSG_HDR, MSG_HDR_SIZE);
    // Write the content size;
    p_cont_size = (uint16_t *)(p_msg->header + MSG_HDR_CONT_OFFSET);

#ifdef WORDS_BIGENDIAN
    *p_cont_size = zdtm_liltobigs(p_msg->cont_size);
#else
    *p_cont_size = p_msg->cont_size;
#endif

    // Initialize the raw content.
    if(p_msg->body.p_raw_content != NULL) return RET_NNULL_RAW;

    // Allocate the raw message.
    p_body = p_msg->body.p_raw_content = malloc(p_msg->cont_size);
    if (p_body == NULL) {
        return -1;
    }

    // Fill in the rest for non-trivial messages
    if(IS_RRL(p_msg)) {
        p_body = zdtm_rrl_write(p_body, &p_msg->body.cont.rrl);

    }else if(IS_RMG(p_msg)){
        p_body = zdtm_rmg_write(p_body, &p_msg->body.cont.rmg);

    }else if(IS_RMS(p_msg)){
        p_body = zdtm_rms_write(p_body, &p_msg->body.cont.rms);

        /*
        if(p_body == NULL) return RET_BAD_SIZE;
        */

    }else if(IS_RTS(p_msg)){
        p_body = zdtm_rts_write(p_body, &p_msg->body.cont.rts);

    }else if(IS_RDI(p_msg)){
        p_body = zdtm_rdi_write(p_body, &p_msg->body.cont.rdi);

    }else if(IS_RSY(p_msg)){
        p_body = zdtm_rsy_write(p_body, &p_msg->body.cont.rsy);

    }else if(IS_RDR(p_msg)){
        p_body = zdtm_rdr_write(p_body, &p_msg->body.cont.rdr);

    }else if(IS_RDW(p_msg)){
        p_body = zdtm_rdw_write(p_body, &p_msg->body.cont.rdw);

        if(p_body == NULL) return RET_UNK_TYPE;

    }else if(IS_RDD(p_msg)){
        p_body = zdtm_rdd_write(p_body, &p_msg->body.cont.rdd);

    }else if(IS_RDS(p_msg)){
        p_body = zdtm_rds_write(p_body, &p_msg->body.cont.rds);

    }else if(IS_RQT(p_msg)){
        p_body = zdtm_rqt_write(p_body, &p_msg->body.cont.rqt);

    }else if(IS_RLR(p_msg)){
        p_body = zdtm_rlr_write(p_body, &p_msg->body.cont.rlr);

    }else if(IS_RGE(p_msg)){
        p_body = zdtm_rge_write(p_body, &p_msg->body.cont.rge);


    }

    // Compute the checksum -- sill in host byte order
    p_msg->check_sum = _zdtm_checksum(p_msg);

    return 0; 
}

int _zdtm_parse_raw_msg(zdtm_msg *p_msg) {

    if (IS_AAY(p_msg)) {
        if (zdtm_parse_raw_aay_msg(p_msg->body.p_raw_content,
                    &p_msg->body.cont.aay))
            return -1;
    } else if (IS_AIG(p_msg)) {
        if (zdtm_parse_raw_aig_msg(p_msg->body.p_raw_content,
                    &p_msg->body.cont.aig))
            return -2;
    } else if (IS_AMG(p_msg)) {
        if (zdtm_parse_raw_amg_msg(p_msg->body.p_raw_content,
                    &p_msg->body.cont.amg))
            return -3;
    } else if (IS_ATG(p_msg)) {
        if (zdtm_parse_raw_atg_msg(p_msg->body.p_raw_content,
                    &p_msg->body.cont.atg))
            return -4;
    } else if (IS_AEX(p_msg)) {
        /*
         * For now the following function does not exist because there
         * is no content inside the AEX messages. Hence, there is
         * nothing to parse.
        if (zdtm_parse_raw_aex_msg(p_msg->body.p_raw_content,
                    &p_msg->body.cont.aex))
            return -5;
         */
    } else if (IS_ANG(p_msg)) {
        if (zdtm_parse_raw_ang_msg(p_msg->body.p_raw_content,
                    &p_msg->body.cont.ang))
            return -6;
    } else if (IS_ADI(p_msg)) {
        if (zdtm_parse_raw_adi_msg(p_msg->body.p_raw_content,
                    &p_msg->body.cont.adi))
            return -7;
    } else if (IS_ASY(p_msg)) {
        if (zdtm_parse_raw_asy_msg(p_msg->body.p_raw_content,
                    &p_msg->body.cont.asy))
            return -8;
    } else if (IS_ADR(p_msg)) {
        if (zdtm_parse_raw_adr_msg(p_msg->body.p_raw_content,
                    &p_msg->body.cont.adr))
            return -9;
    } else {
        return -255;
    }
    
    return 0;
}
