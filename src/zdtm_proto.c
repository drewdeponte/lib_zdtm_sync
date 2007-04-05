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

int _zdtm_reset_sync_state(zdtm_lib_env *cur_env) {
    int r;
    zdtm_msg msg, rmsg;

    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RSS_MSG_TYPE, MSG_TYPE_SIZE);
    msg.body.cont.rss.uk_1 = 0x01;
    msg.body.cont.rss.sync_type = cur_env->sync_type;
    msg.body.cont.rss.uk_2 = 0x01;

    r = _zdtm_wrapped_send_message(cur_env, &msg);
    if (r != 0) { return -1; }
    
    memset(&rmsg, 0, sizeof(zdtm_msg));
    r = _zdtm_wrapped_recv_message(cur_env, &rmsg);
    if (r != 0) { _zdtm_clean_message(&rmsg); return -2; }

    if (memcmp(rmsg.body.type, AEX_MSG_TYPE, MSG_TYPE_SIZE) != 0) {
        _zdtm_clean_message(&rmsg);
        return -3;
    }

    switch(cur_env->sync_type) {
        case 0:  cur_env->todo_slow_sync_required = 1;
        case 1:  cur_env->calendar_slow_sync_required = 1;
        default: cur_env->address_book_slow_sync_required = 1;
    }
    
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

    if ((rmsg.body.cont.atg.year == 0) &&
        (rmsg.body.cont.atg.month == 0) &&
        (rmsg.body.cont.atg.day == 0) &&
        (rmsg.body.cont.atg.hour == 0) &&
        (rmsg.body.cont.atg.minutes == 0) &&
        (rmsg.body.cont.atg.seconds == 0)) {
        
        _zdtm_clean_message(&rmsg);
        return 1;
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

int _zdtm_obtain_item(zdtm_lib_env *cur_env, uint32_t sync_id,
    struct zdtm_adr_msg_param **p_params, uint16_t *p_num_params) {

    zdtm_msg msg, rmsg;
    int r;

    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RDR_MSG_TYPE, MSG_TYPE_SIZE);
    msg.body.cont.rdr.sync_type = cur_env->sync_type;
    msg.body.cont.rdr.num_sync_ids = 1;
    msg.body.cont.rdr.sync_id = sync_id;

    r = _zdtm_wrapped_send_message(cur_env, &msg);
    if (r != 0) { return -1; }
    
    memset(&rmsg, 0, sizeof(zdtm_msg));
    r = _zdtm_wrapped_recv_message(cur_env, &rmsg);
    if (r != 0) { _zdtm_clean_message(&rmsg); return -2; }
    
    if (memcmp(rmsg.body.type, ADR_MSG_TYPE, MSG_TYPE_SIZE) != 0) {
        _zdtm_clean_message(&rmsg);
        return -3;
    }

    (*p_params) = rmsg.body.cont.adr.params;
    (*p_num_params) = rmsg.body.cont.adr.num_params;

    _zdtm_clean_message(&rmsg);

    return 0;
}

int _zdtm_free_params(zdtm_lib_env *cur_env,
    struct zdtm_adr_msg_param *p_params, uint16_t num_params) {

    int i;

    if (p_params == NULL) {
        return -1;
    }

    for (i = 0; i < num_params; i++) {
        if ((p_params[i].param_len != 0) && (p_params[i].param_data != NULL)) {
            free((void *)p_params[i].param_data);
        }
    }

    free((void *)p_params);

    return 0;
}

int _zdtm_parse_todo_item_params(struct zdtm_adi_msg_param *p_param_format,
    uint16_t num_format_params, struct zdtm_adr_msg_param *params,
    uint16_t num_params, struct zdtm_todo_item *p_todo_item) {

    int i;

    if (num_format_params != num_params) {
        return -1;
    }

    /* iterate through the format params and for each iteration */
    for (i = 0; i < num_format_params; i++) {
        switch (p_param_format[i].type_id) {
            case DATA_ID_TIME:
                if (memcmp(p_param_format[i].abrev, "CTTM", 4) == 0) {
                    memcpy(p_todo_item->creation_date, params[i].param_data,
                        params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "MDTM", 4) == 0) {
                    memcpy(p_todo_item->modification_date, params[i].param_data,
                        params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "ETDY", 4) == 0) {
                    memcpy(p_todo_item->start_date, params[i].param_data,
                        params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "LTDY", 4) == 0) {
                    memcpy(p_todo_item->due_date, params[i].param_data,
                        params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "FNDY", 4) == 0) {
                    memcpy(p_todo_item->completed_date, params[i].param_data,
                        params[i].param_len);
                }
                break;
            case DATA_ID_BIT:
                if (memcmp(p_param_format[i].abrev, "ATTR", 4) == 0) {
                    memcpy(&p_todo_item->attribute, params[i].param_data,
                        params[i].param_len);
                }
                break;
            case DATA_ID_UCHAR:
                if (memcmp(p_param_format[i].abrev, "MARK", 4) == 0) {
                    memcpy(&p_todo_item->progress, params[i].param_data,
                        params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "PRTY", 4) == 0) {
                    memcpy(&p_todo_item->priority, params[i].param_data,
                        params[i].param_len);
                }
                break;
            case DATA_ID_BARRAY:
                if (memcmp(p_param_format[i].abrev, "CTGR", 4) == 0) {
                    p_todo_item->category_len = params[i].param_len;
                    p_todo_item->category = malloc(params[i].param_len);
                    if (p_todo_item->category == NULL) {
                        return -2;
                    }
                    memcpy(p_todo_item->category, params[i].param_data,
                        params[i].param_len);

                }
                break;
            case DATA_ID_UTF8:
                if (memcmp(p_param_format[i].abrev, "TITL", 4) == 0) {
                    p_todo_item->description_len = params[i].param_len;
                    p_todo_item->description = malloc(params[i].param_len);
                    if (p_todo_item->description == NULL) {
                        return -3;
                    }
                    memcpy(p_todo_item->description, params[i].param_data,
                        params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "MEM1", 4) == 0) {
                    p_todo_item->notes_len = params[i].param_len;
                    p_todo_item->notes = malloc(params[i].param_len);
                    if (p_todo_item->notes == NULL) {
                        return -4;
                    }
                    memcpy(p_todo_item->notes, params[i].param_data,
                        params[i].param_len);
                }
                break;
            case DATA_ID_ULONG:
                if (memcmp(p_param_format[i].abrev, "SYID", 4) == 0) {
                    memcpy(&p_todo_item->sync_id, params[i].param_data,
                        params[i].param_len);
                }
                break;
            default:
                break;
        }
    }

    return 0;
}

int _zdtm_parse_calendar_item_params(struct zdtm_adi_msg_param *p_param_format,
    uint16_t num_format_params, struct zdtm_adr_msg_param *params,
    uint16_t num_params, struct zdtm_calendar_item *p_calendar_item) {

    int i;

    if (num_format_params != num_params) {
        return -1;
    }

    /* iterate through the format params and for each iteration */
    for (i = 0; i < num_format_params; i++) {
        switch (p_param_format[i].type_id) {
            case DATA_ID_TIME:
                if (memcmp(p_param_format[i].abrev, "CTTM", 4) == 0) {
                    memcpy(p_calendar_item->creation_date, params[i].param_data,
                        params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "MDTM", 4) == 0) {
                    memcpy(p_calendar_item->modification_date,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "TIM1", 4) == 0) {
                    memcpy(p_calendar_item->start_time,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "TIM2", 4) == 0) {
                    memcpy(p_calendar_item->end_time,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "REDT", 4) == 0) {
                    memcpy(p_calendar_item->repeat_end_date,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "ALSD", 4) == 0) {
                    memcpy(p_calendar_item->all_day_start_date,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "ALED", 4) == 0) {
                    memcpy(p_calendar_item->all_day_end_date,
                        params[i].param_data, params[i].param_len);
                }
                break;
            case DATA_ID_BIT:
                if (memcmp(p_param_format[i].abrev, "ATTR", 4) == 0) {
                    memcpy(&p_calendar_item->attribute, params[i].param_data,
                        params[i].param_len);
                }
                break;
            case DATA_ID_UCHAR:
                if (memcmp(p_param_format[i].abrev, "ADAY", 4) == 0) {
                    memcpy(&p_calendar_item->schedule_type,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "ARON", 4) == 0) {
                    memcpy(&p_calendar_item->alarm,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "ARSD", 4) == 0) {
                    memcpy(&p_calendar_item->alarm_setting,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "RTYP", 4) == 0) {
                    memcpy(&p_calendar_item->repeat_type,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "RDYS", 4) == 0) {
                    memcpy(&p_calendar_item->repeat_date,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "REND", 4) == 0) {
                    memcpy(&p_calendar_item->repeat_end_date_setting,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "MDAY", 4) == 0) {
                    memcpy(&p_calendar_item->multiple_days_flag,
                        params[i].param_data, params[i].param_len);
                }
                break;
            case DATA_ID_WORD:
                if (memcmp(p_param_format[i].abrev, "ARMN", 4) == 0) {
                    memcpy(&p_calendar_item->alarm_time,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "RFRQ", 4) == 0) {
                    memcpy(&p_calendar_item->repeat_period,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "RPOS", 4) == 0) {
                    memcpy(&p_calendar_item->repeat_position,
                        params[i].param_data, params[i].param_len);
                }
                break;
            case DATA_ID_BARRAY:
                if (memcmp(p_param_format[i].abrev, "CTGR", 4) == 0) {
                    p_calendar_item->category_len = params[i].param_len;
                    p_calendar_item->category = malloc(params[i].param_len);
                    if (p_calendar_item->category == NULL) {
                        return -2;
                    }
                    memcpy(p_calendar_item->category, params[i].param_data,
                        params[i].param_len);

                }
                break;
            case DATA_ID_UTF8:
                if (memcmp(p_param_format[i].abrev, "DSRP", 4) == 0) {
                    p_calendar_item->description_len = params[i].param_len;
                    p_calendar_item->description = malloc(params[i].param_len);
                    if (p_calendar_item->description == NULL) {
                        return -3;
                    }
                    memcpy(p_calendar_item->description, params[i].param_data,
                        params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "PLCE", 4) == 0) {
                    p_calendar_item->location_len = params[i].param_len;
                    p_calendar_item->location = malloc(params[i].param_len);
                    if (p_calendar_item->location == NULL) {
                        return -4;
                    }
                    memcpy(p_calendar_item->location, params[i].param_data,
                        params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "MEM1", 4) == 0) {
                    p_calendar_item->notes_len = params[i].param_len;
                    p_calendar_item->notes = malloc(params[i].param_len);
                    if (p_calendar_item->notes == NULL) {
                        return -5;
                    }
                    memcpy(p_calendar_item->notes, params[i].param_data,
                        params[i].param_len);
                }
                break;
            case DATA_ID_ULONG:
                if (memcmp(p_param_format[i].abrev, "SYID", 4) == 0) {
                    memcpy(&p_calendar_item->sync_id, params[i].param_data,
                        params[i].param_len);
                }
                break;
            default:
                break;
        }
    }

    return 0;
}

int _zdtm_parse_address_item_params(struct zdtm_adi_msg_param *p_param_format,
    uint16_t num_format_params, struct zdtm_adr_msg_param *params,
    uint16_t num_params, struct zdtm_address_item *p_address_item) {

    int i;

    if (num_format_params != num_params) {
        return -1;
    }

    /* iterate through the format params and for each iteration */
    for (i = 0; i < num_format_params; i++) {
        switch (p_param_format[i].type_id) {
            case DATA_ID_TIME:
                if (memcmp(p_param_format[i].abrev, "CTTM", 4) == 0) {
                    memcpy(p_address_item->creation_date,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "MDTM", 4) == 0) {
                    memcpy(p_address_item->modification_date,
                        params[i].param_data, params[i].param_len);
                }
                break;
            case DATA_ID_BIT:
                if (memcmp(p_param_format[i].abrev, "ATTR", 4) == 0) {
                    memcpy(&p_address_item->attribute, params[i].param_data,
                        params[i].param_len);
                }
                break;
            case DATA_ID_BARRAY:
                if (memcmp(p_param_format[i].abrev, "CTGR", 4) == 0) {
                    p_address_item->category_len = params[i].param_len;
                    p_address_item->category = malloc(params[i].param_len);
                    if (p_address_item->category == NULL) {
                        return -2;
                    }
                    memcpy(p_address_item->category, params[i].param_data,
                        params[i].param_len);

                }
                break;
            case DATA_ID_UTF8:
                if (memcmp(p_param_format[i].abrev, "FULL", 4) == 0) {
                    p_address_item->full_name_len = params[i].param_len;
                    p_address_item->full_name = malloc(params[i].param_len);
                    if (p_address_item->full_name == NULL) {
                        return -3;
                    }
                    memcpy(p_address_item->full_name, params[i].param_data,
                        params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "NAPR", 4) == 0) {
                    p_address_item->full_name_pronun_len = params[i].param_len;
                    p_address_item->full_name_pronun = malloc(
                        params[i].param_len);
                    if (p_address_item->full_name_pronun == NULL) {
                        return -4;
                    }
                    memcpy(p_address_item->full_name_pronun,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "TITL", 4) == 0) {
                    p_address_item->title_len = params[i].param_len;
                    p_address_item->title = malloc(
                        params[i].param_len);
                    if (p_address_item->title == NULL) {
                        return -5;
                    }
                    memcpy(p_address_item->title, params[i].param_data,
                        params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "LNME", 4) == 0) {
                    p_address_item->last_name_len = params[i].param_len;
                    p_address_item->last_name = malloc(
                        params[i].param_len);
                    if (p_address_item->last_name == NULL) {
                        return -6;
                    }
                    memcpy(p_address_item->last_name, params[i].param_data,
                        params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "FNME", 4) == 0) {
                    p_address_item->first_name_len = params[i].param_len;
                    p_address_item->first_name = malloc(
                        params[i].param_len);
                    if (p_address_item->first_name == NULL) {
                        return -7;
                    }
                    memcpy(p_address_item->first_name, params[i].param_data,
                        params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "MNME", 4) == 0) {
                    p_address_item->middle_name_len = params[i].param_len;
                    p_address_item->middle_name = malloc(
                        params[i].param_len);
                    if (p_address_item->middle_name == NULL) {
                        return -8;
                    }
                    memcpy(p_address_item->middle_name, params[i].param_data,
                        params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "SUFX", 4) == 0) {
                    p_address_item->suffix_len = params[i].param_len;
                    p_address_item->suffix = malloc(
                        params[i].param_len);
                    if (p_address_item->suffix == NULL) {
                        return -9;
                    }
                    memcpy(p_address_item->suffix, params[i].param_data,
                        params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "FLAS", 4) == 0) {
                    p_address_item->alternative_name_len = params[i].param_len;
                    p_address_item->alternative_name = malloc(
                        params[i].param_len);
                    if (p_address_item->alternative_name == NULL) {
                        return -10;
                    }
                    memcpy(p_address_item->alternative_name,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "LNPR", 4) == 0) {
                    p_address_item->last_name_pronun_len = params[i].param_len;
                    p_address_item->last_name_pronun = malloc(
                        params[i].param_len);
                    if (p_address_item->last_name_pronun == NULL) {
                        return -11;
                    }
                    memcpy(p_address_item->last_name_pronun,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "FNPR", 4) == 0) {
                    p_address_item->first_name_pronun_len = params[i].param_len;
                    p_address_item->first_name_pronun = malloc(
                        params[i].param_len);
                    if (p_address_item->first_name_pronun == NULL) {
                        return -12;
                    }
                    memcpy(p_address_item->first_name_pronun,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "CPNY", 4) == 0) {
                    p_address_item->company_len = params[i].param_len;
                    p_address_item->company = malloc(
                        params[i].param_len);
                    if (p_address_item->company == NULL) {
                        return -13;
                    }
                    memcpy(p_address_item->company,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "CPPR", 4) == 0) {
                    p_address_item->company_pronun_len = params[i].param_len;
                    p_address_item->company_pronun = malloc(
                        params[i].param_len);
                    if (p_address_item->company_pronun == NULL) {
                        return -14;
                    }
                    memcpy(p_address_item->company_pronun,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "SCTN", 4) == 0) {
                    p_address_item->department_len = params[i].param_len;
                    p_address_item->department = malloc(
                        params[i].param_len);
                    if (p_address_item->department == NULL) {
                        return -15;
                    }
                    memcpy(p_address_item->department,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "PSTN", 4) == 0) {
                    p_address_item->job_title_len = params[i].param_len;
                    p_address_item->job_title = malloc(
                        params[i].param_len);
                    if (p_address_item->job_title == NULL) {
                        return -16;
                    }
                    memcpy(p_address_item->job_title,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "TEL2", 4) == 0) {
                    p_address_item->work_phone_len = params[i].param_len;
                    p_address_item->work_phone = malloc(
                        params[i].param_len);
                    if (p_address_item->work_phone == NULL) {
                        return -17;
                    }
                    memcpy(p_address_item->work_phone,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "FAX2", 4) == 0) {
                    p_address_item->work_fax_len = params[i].param_len;
                    p_address_item->work_fax = malloc(
                        params[i].param_len);
                    if (p_address_item->work_fax == NULL) {
                        return -18;
                    }
                    memcpy(p_address_item->work_fax,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "CPS2", 4) == 0) {
                    p_address_item->work_mobile_len = params[i].param_len;
                    p_address_item->work_mobile = malloc(
                        params[i].param_len);
                    if (p_address_item->work_mobile == NULL) {
                        return -19;
                    }
                    memcpy(p_address_item->work_mobile,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "BSTA", 4) == 0) {
                    p_address_item->work_state_len = params[i].param_len;
                    p_address_item->work_state = malloc(
                        params[i].param_len);
                    if (p_address_item->work_state == NULL) {
                        return -20;
                    }
                    memcpy(p_address_item->work_state,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "BCTY", 4) == 0) {
                    p_address_item->work_city_len = params[i].param_len;
                    p_address_item->work_city = malloc(
                        params[i].param_len);
                    if (p_address_item->work_city == NULL) {
                        return -21;
                    }
                    memcpy(p_address_item->work_city,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "BSTR", 4) == 0) {
                    p_address_item->work_street_len = params[i].param_len;
                    p_address_item->work_street = malloc(
                        params[i].param_len);
                    if (p_address_item->work_street == NULL) {
                        return -22;
                    }
                    memcpy(p_address_item->work_street,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "BZIP", 4) == 0) {
                    p_address_item->work_zip_len = params[i].param_len;
                    p_address_item->work_zip = malloc(
                        params[i].param_len);
                    if (p_address_item->work_zip == NULL) {
                        return -23;
                    }
                    memcpy(p_address_item->work_zip,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "BCTR", 4) == 0) {
                    p_address_item->work_country_len = params[i].param_len;
                    p_address_item->work_country = malloc(
                        params[i].param_len);
                    if (p_address_item->work_country == NULL) {
                        return -24;
                    }
                    memcpy(p_address_item->work_country,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "BWEB", 4) == 0) {
                    p_address_item->work_web_page_len = params[i].param_len;
                    p_address_item->work_web_page = malloc(
                        params[i].param_len);
                    if (p_address_item->work_web_page == NULL) {
                        return -25;
                    }
                    memcpy(p_address_item->work_web_page,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "OFCE", 4) == 0) {
                    p_address_item->office_len = params[i].param_len;
                    p_address_item->office = malloc(
                        params[i].param_len);
                    if (p_address_item->office == NULL) {
                        return -26;
                    }
                    memcpy(p_address_item->office,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "PRFS", 4) == 0) {
                    p_address_item->profession_len = params[i].param_len;
                    p_address_item->profession = malloc(
                        params[i].param_len);
                    if (p_address_item->profession == NULL) {
                        return -27;
                    }
                    memcpy(p_address_item->profession,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "ASST", 4) == 0) {
                    p_address_item->assistant_len = params[i].param_len;
                    p_address_item->assistant = malloc(
                        params[i].param_len);
                    if (p_address_item->assistant == NULL) {
                        return -28;
                    }
                    memcpy(p_address_item->assistant,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "MNGR", 4) == 0) {
                    p_address_item->manager_len = params[i].param_len;
                    p_address_item->manager = malloc(
                        params[i].param_len);
                    if (p_address_item->manager == NULL) {
                        return -29;
                    }
                    memcpy(p_address_item->manager,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "BPGR", 4) == 0) {
                    p_address_item->pager_len = params[i].param_len;
                    p_address_item->pager = malloc(
                        params[i].param_len);
                    if (p_address_item->pager == NULL) {
                        return -30;
                    }
                    memcpy(p_address_item->pager,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "CPS1", 4) == 0) {
                    p_address_item->cellular_len = params[i].param_len;
                    p_address_item->cellular = malloc(
                        params[i].param_len);
                    if (p_address_item->cellular == NULL) {
                        return -31;
                    }
                    memcpy(p_address_item->cellular,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "TEL1", 4) == 0) {
                    p_address_item->home_phone_len = params[i].param_len;
                    p_address_item->home_phone = malloc(
                        params[i].param_len);
                    if (p_address_item->home_phone == NULL) {
                        return -32;
                    }
                    memcpy(p_address_item->home_phone,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "FAX1", 4) == 0) {
                    p_address_item->home_fax_len = params[i].param_len;
                    p_address_item->home_fax = malloc(
                        params[i].param_len);
                    if (p_address_item->home_fax == NULL) {
                        return -33;
                    }
                    memcpy(p_address_item->home_fax,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "HSTA", 4) == 0) {
                    p_address_item->home_state_len = params[i].param_len;
                    p_address_item->home_state = malloc(
                        params[i].param_len);
                    if (p_address_item->home_state == NULL) {
                        return -34;
                    }
                    memcpy(p_address_item->home_state,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "HCTY", 4) == 0) {
                    p_address_item->home_city_len = params[i].param_len;
                    p_address_item->home_city = malloc(
                        params[i].param_len);
                    if (p_address_item->home_city == NULL) {
                        return -35;
                    }
                    memcpy(p_address_item->home_city,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "HSTR", 4) == 0) {
                    p_address_item->home_street_len = params[i].param_len;
                    p_address_item->home_street = malloc(
                        params[i].param_len);
                    if (p_address_item->home_street == NULL) {
                        return -36;
                    }
                    memcpy(p_address_item->home_street,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "HZIP", 4) == 0) {
                    p_address_item->home_zip_len = params[i].param_len;
                    p_address_item->home_zip = malloc(
                        params[i].param_len);
                    if (p_address_item->home_zip == NULL) {
                        return -37;
                    }
                    memcpy(p_address_item->home_zip,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "HCTR", 4) == 0) {
                    p_address_item->home_country_len = params[i].param_len;
                    p_address_item->home_country = malloc(
                        params[i].param_len);
                    if (p_address_item->home_country == NULL) {
                        return -38;
                    }
                    memcpy(p_address_item->home_country,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "HWEB", 4) == 0) {
                    p_address_item->home_web_page_len = params[i].param_len;
                    p_address_item->home_web_page = malloc(
                        params[i].param_len);
                    if (p_address_item->home_web_page == NULL) {
                        return -39;
                    }
                    memcpy(p_address_item->home_web_page,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "DMAL", 4) == 0) {
                    p_address_item->default_email_len = params[i].param_len;
                    p_address_item->default_email = malloc(
                        params[i].param_len);
                    if (p_address_item->default_email == NULL) {
                        return -40;
                    }
                    memcpy(p_address_item->default_email,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "MAL1", 4) == 0) {
                    p_address_item->emails_len = params[i].param_len;
                    p_address_item->emails = malloc(
                        params[i].param_len);
                    if (p_address_item->emails == NULL) {
                        return -41;
                    }
                    memcpy(p_address_item->emails,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "SPUS", 4) == 0) {
                    p_address_item->spouse_len = params[i].param_len;
                    p_address_item->spouse = malloc(
                        params[i].param_len);
                    if (p_address_item->spouse == NULL) {
                        return -42;
                    }
                    memcpy(p_address_item->spouse,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "GNDR", 4) == 0) {
                    p_address_item->gender_len = params[i].param_len;
                    p_address_item->gender = malloc(
                        params[i].param_len);
                    if (p_address_item->gender == NULL) {
                        return -43;
                    }
                    memcpy(p_address_item->gender,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "BRTH", 4) == 0) {
                    p_address_item->birthday_len = params[i].param_len;
                    p_address_item->birthday = malloc(
                        params[i].param_len);
                    if (p_address_item->birthday == NULL) {
                        return -44;
                    }
                    memcpy(p_address_item->birthday,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "ANIV", 4) == 0) {
                    p_address_item->anniversary_len = params[i].param_len;
                    p_address_item->anniversary = malloc(
                        params[i].param_len);
                    if (p_address_item->anniversary == NULL) {
                        return -45;
                    }
                    memcpy(p_address_item->anniversary,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "NCNM", 4) == 0) {
                    p_address_item->nickname_len = params[i].param_len;
                    p_address_item->nickname = malloc(
                        params[i].param_len);
                    if (p_address_item->nickname == NULL) {
                        return -46;
                    }
                    memcpy(p_address_item->nickname,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "CLDR", 4) == 0) {
                    p_address_item->children_len = params[i].param_len;
                    p_address_item->children = malloc(
                        params[i].param_len);
                    if (p_address_item->children == NULL) {
                        return -47;
                    }
                    memcpy(p_address_item->children,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "MEM1", 4) == 0) {
                    p_address_item->memo_len = params[i].param_len;
                    p_address_item->memo = malloc(
                        params[i].param_len);
                    if (p_address_item->memo == NULL) {
                        return -48;
                    }
                    memcpy(p_address_item->memo,
                        params[i].param_data, params[i].param_len);
                } else if (memcmp(p_param_format[i].abrev, "GRPS", 4) == 0) {
                    p_address_item->group_len = params[i].param_len;
                    p_address_item->group = malloc(
                        params[i].param_len);
                    if (p_address_item->group == NULL) {
                        return -49;
                    }
                    memcpy(p_address_item->group,
                        params[i].param_data, params[i].param_len);
                }
                break;
            case DATA_ID_ULONG:
                if (memcmp(p_param_format[i].abrev, "SYID", 4) == 0) {
                    memcpy(&p_address_item->sync_id, params[i].param_data,
                        params[i].param_len);
                }
                break;
            default:
                break;
        }
    }

    return 0;
}

int _zdtm_obtain_todo_item(zdtm_lib_env *cur_env, uint32_t sync_id,
    struct zdtm_todo_item *p_todo_item) {

    int r;
    struct zdtm_adr_msg_param *params;
    uint16_t num_params;

    if (cur_env->sync_type != SYNC_TYPE_TODO) {
        return -1;
    }

    r = _zdtm_obtain_item(cur_env, sync_id, &params, &num_params);
    if (r != 0) {
        return -2;
    }

    r = _zdtm_parse_todo_item_params(cur_env->params, cur_env->num_params,
        params, num_params, p_todo_item);
    if (r != 0) {
        _zdtm_free_params(cur_env, params, num_params);
        return -3;
    }
    
    _zdtm_free_params(cur_env, params, num_params);

    return 0;
}

int _zdtm_obtain_calendar_item(zdtm_lib_env *cur_env, uint32_t sync_id,
    struct zdtm_calendar_item *p_calendar_item) {

    int r;
    struct zdtm_adr_msg_param *params;
    uint16_t num_params;

    if (cur_env->sync_type != SYNC_TYPE_CALENDAR) {
        return -1;
    }

    r = _zdtm_obtain_item(cur_env, sync_id, &params, &num_params);
    if (r != 0) {
        return -2;
    }

    r = _zdtm_parse_calendar_item_params(cur_env->params, cur_env->num_params,
        params, num_params, p_calendar_item);
    if (r != 0) {
        _zdtm_free_params(cur_env, params, num_params);
        return -3;
    }
    
    _zdtm_free_params(cur_env, params, num_params);

    return 0;
}

int _zdtm_obtain_address_item(zdtm_lib_env *cur_env, uint32_t sync_id,
    struct zdtm_address_item *p_address_item) {

    int r;
    struct zdtm_adr_msg_param *params;
    uint16_t num_params;

    if (cur_env->sync_type != SYNC_TYPE_ADDRESS) {
        return -1;
    }

    r = _zdtm_obtain_item(cur_env, sync_id, &params, &num_params);
    if (r != 0) {
        return -2;
    }

    r = _zdtm_parse_address_item_params(cur_env->params, cur_env->num_params,
        params, num_params, p_address_item);
    if (r != 0) {
        _zdtm_free_params(cur_env, params, num_params);
        return -3;
    }
    
    _zdtm_free_params(cur_env, params, num_params);

    return 0;
}

int _zdtm_delete_item(zdtm_lib_env *cur_env, uint32_t sync_id) {
    zdtm_msg msg, rmsg;
    int r;

    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RDD_MSG_TYPE, MSG_TYPE_SIZE);
    msg.body.cont.rdd.sync_type = cur_env->sync_type;
    msg.body.cont.rdd.num_sync_ids = 1;
    msg.body.cont.rdd.sync_id = sync_id;

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
