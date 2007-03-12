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
 * @file zdtm_proto.c
 * @brief This is an implementation file for the zdtm protocol.
 *
 * The zdtm_proto.c file is an implementations file for the Zaurus DTM
 * based synchronization protocols private API. It provides a layer that
 * implements functions that represent the logical structures found in
 * the protocol, such as obtain an objects sync id, obtaining device
 * info, etc.
 */

#include "zdtm_proto.h"

int _zdtm_connect(zdtm_lib_env *cur_env, const char *ip_addr) {
    int r;
    zdtm_msg msg;

    /* Create a TCP/IP connection to the synchronization daemon that is
     * running on the Zaurus. */
    r = _zdtm_conn_to_zaurus(cur_env, ip_addr);
    if (r != 0) { return -1; }

    /* Create a RAY message and send it to the synchronization daemon on
     * the Zaurus, to request that it instruct the synchronization
     * client on the Zaurus connect back to the Desktop synchronization
     * daemon which was initialized in the _zdtm_initialize function. */
    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RAY_MSG_TYPE, MSG_TYPE_SIZE);
    r = _zdtm_send_message_to(cur_env, &msg, cur_env->reqfd);
    if (r != 0) { return -2; }

    /* Handle any back logged connections from the synchronization
     * client on the Zaurus, or block waiting for a connection from the
     * synchronization client on the Zaurus. */
    r = _zdtm_handle_zaurus_conn(cur_env);
    if (r != 0) { return -3; }

    return 0;
}

int _zdtm_handle_connection(zdtm_lib_env *cur_env) {
    int r;

    /* Handle any back logged connections from the synchronization
     * client on the Zaurus, or block waiting for a connection from the
     * synchronization client on the Zaurus. */
    r = _zdtm_handle_zaurus_conn(cur_env);
    if (r != 0) { return -1; }

    return 0;
}

int _zdtm_obtain_device_info(zdtm_lib_env *cur_env) {
    int r;
    zdtm_msg msg, rmsg;

    /* Send RIG message to the Zaurus */
    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RIG_MSG_TYPE, MSG_TYPE_SIZE);
    r = _zdtm_wrapped_send_message(cur_env, &msg);
    if (r != 0) {
        _zdtm_log_error(cur_env, "zdtm_obtain_device_info: _zdtm_wrapped_send_message",
            r);
        return -1;
    }

    /* Receive message back from the Zaurus */
    memset(&rmsg, 0, sizeof(zdtm_msg));
    r = _zdtm_wrapped_recv_message(cur_env, &rmsg);
    if (r != 0) {
        _zdtm_log_error(cur_env, "zdtm_obtain_device_info: _zdtm_wrapped_recv_message",
            r);
        return -2;
    }

    /* Check to make sure receive back an AIG message */
    if (!IS_AIG((&rmsg))) {
        _zdtm_clean_message(&rmsg);
        return -3;
    }

    /* copy to the content out of the AIG message */
    memcpy(cur_env->model, rmsg.body.cont.aig.model_str,
        rmsg.body.cont.aig.model_str_len);
    cur_env->model[rmsg.body.cont.aig.model_str_len] = '\0';
    memcpy(cur_env->language, rmsg.body.cont.aig.language, 2);
    cur_env->cur_auth_state = rmsg.body.cont.aig.auth_state;

    cur_env->retreived_device_info = 1;

    _zdtm_clean_message(&rmsg);

    return 0;
}

int _zdtm_obtain_sync_state(zdtm_lib_env *cur_env) {
    int r;
    zdtm_msg msg, rmsg;

    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RMG_MSG_TYPE, MSG_TYPE_SIZE);
    msg.body.cont.rmg.uk = 0x01;
    msg.body.cont.rmg.sync_type = cur_env->sync_type;

    r = _zdtm_wrapped_send_message(cur_env, &msg);
    if (r != 0) { return -1; }

    memset(&rmsg, 0, sizeof(zdtm_msg));
    r = _zdtm_wrapped_recv_message(cur_env, &rmsg);
    if (r != 0) { _zdtm_clean_message(&rmsg); return -2; }

    if (memcmp(rmsg.body.type, AMG_MSG_TYPE, MSG_TYPE_SIZE) != 0) {
        _zdtm_clean_message(&rmsg);
        return -3;
    }

    /* check if todo slow sync is required */
    if ((rmsg.body.cont.amg.fullsync_flags & AMG_TODO_MASK) == 0) {
        cur_env->todo_slow_sync_required = 1;
    }

    /* check if calendar slow sync is required */
    if ((rmsg.body.cont.amg.fullsync_flags & AMG_CAL_MASK) == 0) {
        cur_env->calendar_slow_sync_required = 1;
    }

    /* check if address book slow sync is required */
    if ((rmsg.body.cont.amg.fullsync_flags & AMG_ADDR_MASK) == 0) {
        cur_env->address_book_slow_sync_required = 1;
    }

    cur_env->retrieved_sync_state = 1;

    _zdtm_clean_message(&rmsg);

    return 0;
}

int _zdtm_authenticate_passcode(zdtm_lib_env *cur_env, char *passcode) {
    int r, pw_size;
    zdtm_msg msg, rmsg, angmsg;

    pw_size = strlen(passcode);

    /* construct a RRL message to attempt to authenticate */
    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RRL_MSG_TYPE, MSG_TYPE_SIZE);
    msg.body.cont.rrl.pw = malloc(pw_size);
    if (msg.body.cont.rrl.pw == NULL) {
        return -1;
    }
    memcpy(msg.body.cont.rrl.pw, passcode, pw_size);
    msg.body.cont.rrl.pw_size = pw_size;

    /* send RRL message */
    r = _zdtm_wrapped_send_message(cur_env, &msg);
    if (r != 0) { free(msg.body.cont.rrl.pw); return -2; }

    free(msg.body.cont.rrl.pw);

    /* recv response message (AEX if succeeded, abort common msg) */
    memset(&rmsg, 0, sizeof(zdtm_msg));
    r = _zdtm_wrapped_recv_message(cur_env, &rmsg);
    if (r == 3) {
        memset(&angmsg, 0, sizeof(zdtm_msg));
        r = _zdtm_wrapped_recv_message(cur_env, &angmsg);
        if (r != 0) {
            _zdtm_clean_message(&angmsg);
            _zdtm_clean_message(&rmsg);
            return -3;
        }

        if (memcmp(angmsg.body.type, ANG_MSG_TYPE, MSG_TYPE_SIZE) == 0) {
            _zdtm_clean_message(&angmsg);
            _zdtm_clean_message(&rmsg);
            return 1;
        } else {
            _zdtm_clean_message(&angmsg);
            _zdtm_clean_message(&rmsg);
            return -4; 
        }
    } else if (r != 0) { _zdtm_clean_message(&rmsg); return -5; }

    if (memcmp(rmsg.body.type, AEX_MSG_TYPE, MSG_TYPE_SIZE) == 0) {
        _zdtm_clean_message(&rmsg);
        return 0;
    } else {
        _zdtm_clean_message(&rmsg);
        return -6;
    }
}

int _zdtm_obtain_last_time_synced(zdtm_lib_env *cur_env, time_t *p_time) {
    zdtm_msg msg, rmsg;
    int r;
    char tmp_buf[5];
    struct tm tmp_time;
    time_t last_sync_time;

    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RTG_MSG_TYPE, MSG_TYPE_SIZE);

    r = _zdtm_wrapped_send_message(cur_env, &msg);
    if (r != 0) { return -1; }
    
    memset(&rmsg, 0, sizeof(zdtm_msg));
    r = _zdtm_wrapped_recv_message(cur_env, &rmsg);
    if (r != 0) { _zdtm_clean_message(&rmsg); return -2; }

    if (memcmp(rmsg.body.type, ATG_MSG_TYPE, MSG_TYPE_SIZE) != 0) {
        _zdtm_clean_message(&rmsg);
        return -3;
    }

    memcpy(tmp_buf, rmsg.body.cont.atg.year, 4);
    tmp_buf[4] = '\0';
    tmp_time.tm_year = (atoi(tmp_buf) - 1900);

    memcpy(tmp_buf, rmsg.body.cont.atg.month, 2);
    tmp_buf[2] = '\0';
    tmp_time.tm_mon = (atoi(tmp_buf) - 1);

    memcpy(tmp_buf, rmsg.body.cont.atg.day, 2);
    tmp_buf[2] = '\0';
    tmp_time.tm_mday = atoi(tmp_buf);
    
    memcpy(tmp_buf, rmsg.body.cont.atg.hour, 2);
    tmp_buf[2] = '\0';
    tmp_time.tm_hour = atoi(tmp_buf);
    
    memcpy(tmp_buf, rmsg.body.cont.atg.minutes, 2);
    tmp_buf[2] = '\0';
    tmp_time.tm_min = atoi(tmp_buf);

    memcpy(tmp_buf, rmsg.body.cont.atg.seconds, 2);
    tmp_buf[2] = '\0';
    tmp_time.tm_sec = atoi(tmp_buf);

    tmp_time.tm_isdst = -1;

    last_sync_time = mktime(&tmp_time);
    if (last_sync_time == -1) { /* error in call to mktime() */
        _zdtm_clean_message(&rmsg);
        return -4;
    }

    _zdtm_clean_message(&rmsg);

    (*p_time) = last_sync_time;

    return 0;
}

int _zdtm_set_last_time_synced(zdtm_lib_env *cur_env, time_t time_synced) {
    struct tm time_brkdwn;
    char msg_data[15];
    zdtm_msg msg, rmsg;
    int r;

    if (localtime_r(&time_synced, &time_brkdwn) == NULL) {
        return -1;
    }

    r = snprintf(msg_data, 15, "%.4d%.2d%.2d%.2d%.2d%.2d",
                 (time_brkdwn.tm_year + 1900),
                 (time_brkdwn.tm_mon + 1),
                 time_brkdwn.tm_mday,
                 time_brkdwn.tm_hour,
                 time_brkdwn.tm_min,
                 time_brkdwn.tm_sec);
    if (r < 0) {
        return -2;
    }

    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RTS_MSG_TYPE, MSG_TYPE_SIZE);
    memcpy(msg.body.cont.rts.date, msg_data, RTS_DATE_LEN);

    r = _zdtm_wrapped_send_message(cur_env, &msg);
    if (r != 0) { return -3; }
    
    memset(&rmsg, 0, sizeof(zdtm_msg));
    r = _zdtm_wrapped_recv_message(cur_env, &rmsg);
    if (r != 0) { _zdtm_clean_message(&rmsg); return -4; }

    if (memcmp(rmsg.body.type, AEX_MSG_TYPE, MSG_TYPE_SIZE) != 0) {
        _zdtm_clean_message(&rmsg);
        return -5;
    }

    _zdtm_clean_message(&rmsg);

    return 0;
}

int _zdtm_reset_sync_log(zdtm_lib_env *cur_env) {
    int r;
    zdtm_msg msg, rmsg;

    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RMS_MSG_TYPE, MSG_TYPE_SIZE);
    msg.body.cont.rms.log_size = 0;
    memset(msg.body.cont.rms.log, 0x00, RMS_LOG_SIZE);

    r = _zdtm_wrapped_send_message(cur_env, &msg);
    if (r != 0) { return -1; }

    memset(&rmsg, 0, sizeof(zdtm_msg));
    r = _zdtm_wrapped_recv_message(cur_env, &rmsg);
    if (r != 3) { /* failed to recv abrt common message */
        _zdtm_clean_message(&rmsg); return -2;
    }
    
    memset(&rmsg, 0, sizeof(zdtm_msg));
    r = _zdtm_wrapped_recv_message(cur_env, &rmsg);
    if (r != 0) { /* failed to recv response message */
        _zdtm_clean_message(&rmsg); return -3;
    }

    if (memcmp(rmsg.body.type, ANG_MSG_TYPE, MSG_TYPE_SIZE) != 0) {
        _zdtm_clean_message(&rmsg);
        return -4;
    }

    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RMS_MSG_TYPE, MSG_TYPE_SIZE);
    msg.body.cont.rms.log_size = 5;
    msg.body.cont.rms.log[0] = 0x01;
    msg.body.cont.rms.log[1] = 0x02;
    msg.body.cont.rms.log[2] = 0x03;
    msg.body.cont.rms.log[3] = 0x04;
    msg.body.cont.rms.log[4] = 0x05;
    r = _zdtm_wrapped_send_message(cur_env, &msg);
    if (r != 0) { _zdtm_clean_message(&rmsg); return -5; }
   
    /* I am not 100% sure we want to do this here, so I am commenting it
     * out for now. If we decide in the future that it is the proper
     * place then we will uncomment this.
    cur_env->retrieved_sync_state = 0;
    cur_env->todo_slow_sync_required = 1;
    cur_env->calendar_slow_sync_required = 1;
    cur_env->address_book_slow_sync_required = 1;
    */
    
    _zdtm_clean_message(&rmsg);

    memset(&rmsg, 0, sizeof(zdtm_msg));
    r = _zdtm_wrapped_recv_message(cur_env, &rmsg);
    if (r != 0) { /* failed to recv response message */
        _zdtm_clean_message(&rmsg); return -6;
    }

    if (memcmp(rmsg.body.type, AEX_MSG_TYPE, MSG_TYPE_SIZE) != 0) {
        _zdtm_clean_message(&rmsg);
        return -7;
    }

    _zdtm_clean_message(&rmsg);

    return 0;
}

int _zdtm_obtain_sync_id_lists(zdtm_lib_env *cur_env,
    uint32_t **pp_new_sync_ids, uint16_t *p_num_new_sync_ids,
    uint32_t **pp_mod_sync_ids, uint16_t *p_num_mod_sync_ids,
    uint32_t **pp_del_sync_ids, uint16_t *p_num_del_sync_ids) {

    int r;
    zdtm_msg msg, rmsg;
    uint32_t *p_new_sync_ids;
    uint32_t *p_mod_sync_ids;
    uint32_t *p_del_sync_ids;
    uint16_t num_new_sync_ids, num_mod_sync_ids, num_del_sync_ids;

    p_new_sync_ids = NULL;
    p_mod_sync_ids = NULL;
    p_del_sync_ids = NULL;

    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RSY_MSG_TYPE, MSG_TYPE_SIZE);
    msg.body.cont.rsy.sync_type = cur_env->sync_type;
    msg.body.cont.rsy.uk = 0x07;

    r = _zdtm_wrapped_send_message(cur_env, &msg);
    if(r != 0){ return -1; }

    memset(&rmsg, 0, sizeof(zdtm_msg));
    r = _zdtm_wrapped_recv_message(cur_env, &rmsg);
    if(r != 0){ _zdtm_clean_message(&rmsg); return -2; }

    if (memcmp(rmsg.body.type, ASY_MSG_TYPE, MSG_TYPE_SIZE) != 0) {
        _zdtm_clean_message(&rmsg);
        return -3;
    }

    num_new_sync_ids = rmsg.body.cont.asy.num_new_sync_ids;
    p_new_sync_ids = malloc(sizeof(uint32_t) * num_new_sync_ids);
    if (p_new_sync_ids == NULL) {
        _zdtm_clean_message(&rmsg);
        return -4;
    }
    num_mod_sync_ids = rmsg.body.cont.asy.num_mod_sync_ids;
    p_mod_sync_ids = malloc(sizeof(uint32_t) * num_mod_sync_ids);
    if (p_mod_sync_ids == NULL) {
        free(p_new_sync_ids);
        _zdtm_clean_message(&rmsg);
        return -4;
    }
    num_del_sync_ids = rmsg.body.cont.asy.num_del_sync_ids;
    p_del_sync_ids = malloc(sizeof(uint32_t) * num_del_sync_ids);
    if (p_del_sync_ids == NULL) {
        free(p_mod_sync_ids);
        free(p_new_sync_ids);
        _zdtm_clean_message(&rmsg);
        return -4;
    }

    /* At this point I know I have allocated all the necessary space so
     * I can copy the lists over and assign the pointers and their
     * associated lengths. */
    memcpy(p_new_sync_ids, rmsg.body.cont.asy.new_sync_ids,
           (sizeof(uint32_t) * num_new_sync_ids));
    memcpy(p_mod_sync_ids, rmsg.body.cont.asy.mod_sync_ids,
           (sizeof(uint32_t) * num_mod_sync_ids));
    memcpy(p_del_sync_ids, rmsg.body.cont.asy.del_sync_ids,
           (sizeof(uint32_t) * num_del_sync_ids));

    (*pp_new_sync_ids) = p_new_sync_ids;
    (*pp_mod_sync_ids) = p_mod_sync_ids;
    (*pp_del_sync_ids) = p_del_sync_ids;
    (*p_num_new_sync_ids) = num_new_sync_ids;
    (*p_num_mod_sync_ids) = num_mod_sync_ids;
    (*p_num_del_sync_ids) = num_del_sync_ids;

    _zdtm_clean_message(&rmsg);
    
    return 0;
}

int _zdtm_obtain_param_format(zdtm_lib_env *cur_env) {
    zdtm_msg msg, rmsg;
    int r;

    if (cur_env->params != NULL) {
        return -4;
    }

    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RDI_MSG_TYPE, MSG_TYPE_SIZE);
    msg.body.cont.rdi.sync_type = cur_env->sync_type;
    msg.body.cont.rdi.uk = 0x07;

    r = _zdtm_wrapped_send_message(cur_env, &msg);
    if (r != 0) { return -1; }

    memset(&rmsg, 0, sizeof(zdtm_msg));
    r = _zdtm_wrapped_recv_message(cur_env, &rmsg);
    if (r != 0) { _zdtm_clean_message(&rmsg); return -2; }

    if (memcmp(rmsg.body.type, ADI_MSG_TYPE, MSG_TYPE_SIZE) != 0) {
        _zdtm_clean_message(&rmsg);
        return -3;
    }

    cur_env->num_params = rmsg.body.cont.adi.num_params;
    cur_env->params = rmsg.body.cont.adi.params;

    _zdtm_clean_message(&rmsg);

    return 0;
}

int _zdtm_state_sync_done(zdtm_lib_env *cur_env) {
    zdtm_msg msg, rmsg;
    int r;
    
    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RDS_MSG_TYPE, MSG_TYPE_SIZE);
    msg.body.cont.rds.sync_type = cur_env->sync_type;
    msg.body.cont.rds.status = 0x07;
    msg.body.cont.rds.null_bytes[0] = 0x00;
    msg.body.cont.rds.null_bytes[1] = 0x00;

    r = _zdtm_wrapped_send_message(cur_env, &msg);
    if (r != 0) { return -1; }
    
    memset(&rmsg, 0, sizeof(zdtm_msg));
    r = _zdtm_wrapped_recv_message(cur_env, &rmsg);
    if (r != 0) { _zdtm_clean_message(&rmsg); return -2; }

    if (memcmp(rmsg.body.type, AEX_MSG_TYPE, MSG_TYPE_SIZE) != 0) {
        _zdtm_clean_message(&rmsg);
        return -3;
    }

    _zdtm_clean_message(&rmsg);
    
    return 0;
}

int _zdtm_disconnect(zdtm_lib_env *cur_env) {
    int r;

    /* close connection to the Zaurus */
    r = _zdtm_close_conn_to_zaurus(cur_env);
    if (r != 0) { return -1; }

    return 0;
}
