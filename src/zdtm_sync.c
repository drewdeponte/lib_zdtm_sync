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
 * @file zdtm_sync.c
 * @brief This is an implementation file for a Zaurus DTM support library.
 *
 * The zdtm_sync.c file is an implementation for a Zaurus DTM based
 * synchronization, general support library. It provides all the
 * definitions for all the general functions used to perform the Zaurus
 * side of a synchronization.
 */

#include "zdtm_sync.h"

int zdtm_initialize(zdtm_lib_env *cur_env) {
    int r;

    r = _zdtm_open_log(cur_env);
    if (r != 0) { return -1; }

    r = _zdtm_listen_for_zaurus(cur_env);
    if (r != 0) { return -2; }

    return 0;
}

int zdtm_finalize(zdtm_lib_env *cur_env) {
    int r;

    r = _zdtm_close_log(cur_env);
    if (r != 0) { return -1; }

    return 0;
}

int zdtm_connect(zdtm_lib_env *cur_env, const char *ip_addr) {
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

int zdtm_send_message(zdtm_lib_env *cur_env, zdtm_msg *msg) {
    int r;
    zdtm_msg rmsg;

    /* recv rqst message */
    memset(&rmsg, 0, sizeof(zdtm_msg));
    r = _zdtm_recv_message(cur_env, &rmsg);
    if (r != 2) { _zdtm_clean_message(&rmsg); return -1; }

    /* send general message */
    r = _zdtm_send_message(cur_env, msg);
    if (r != 0) { return -2; }

    /* recv ack message */
    memset(&rmsg, 0, sizeof(zdtm_msg));
    r = _zdtm_recv_message(cur_env, &rmsg);
    if (r != 1) { _zdtm_clean_message(&rmsg); return -3; }

    return 0;
}

int zdtm_recv_message(zdtm_lib_env *cur_env, zdtm_msg *msg) {
    int r;

    /* send rqst message */
    r = _zdtm_send_rqst_message(cur_env);
    if (r != 0) {
        _zdtm_log_error(cur_env, "_zdtm_send_rqst_message", r);
        return -1;
    }

    /* recv general message */
    r = _zdtm_recv_message(cur_env, msg);
    if (r != 0) {
        _zdtm_log_error(cur_env, "_zdtm_recv_message", r);
        return -2;
    }

    /* send ack message */
    r = _zdtm_send_ack_message(cur_env);
    if (r != 0) {
        _zdtm_log_error(cur_env, "_zdtm_send_ack_message", r);
        return -3;
    }

    return 0;
}

int zdtm_initiate_sync(zdtm_lib_env *cur_env) {
    int r;
    zdtm_msg msg, rmsg;

    /* Send RAY message to the Zaurus */
    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RAY_MSG_TYPE, MSG_TYPE_SIZE);
    r = _zdtm_send_message(cur_env, &msg);
    if (r != 0) {
        _zdtm_log_error(cur_env, "zdtm_initiate_sync: _zdtm_send_message",
            r);
        return -1;
    }

    /* receive an ack */
    memset(&rmsg, 0, sizeof(zdtm_msg));
    r = _zdtm_recv_message(cur_env, &rmsg);
    if (r != 1) {
        _zdtm_log_error(cur_env, "zdtm_initiate_sync: _zdtm_recv_message",
            r);
        if (r == 0) {
            _zdtm_clean_message(&rmsg);
            return -1;  /* received message other than an ack */
        } else {
            _zdtm_clean_message(&rmsg);
            return -2;
        }
    }
    _zdtm_clean_message(&rmsg);

    /* Receive a AAY message */
    memset(&rmsg, 0, sizeof(zdtm_msg));
    r = zdtm_recv_message(cur_env, &rmsg);
    if (r != 0) {
        _zdtm_log_error(cur_env, "zdtm_initiate_sync: zdtm_recv_message",
            r);
        _zdtm_clean_message(&rmsg);
        return -3;
    }

    /* Check if the received message is an AAY message */
    if (!IS_AAY((&rmsg))) {
        _zdtm_clean_message(&rmsg);
        return -4;
    }

    _zdtm_clean_message(&rmsg);

    return 0;
}

int zdtm_obtain_device_info(zdtm_lib_env *cur_env) {
    int r;
    zdtm_msg msg, rmsg;

    /* Send RIG message to the Zaurus */
    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RIG_MSG_TYPE, MSG_TYPE_SIZE);
    r = zdtm_send_message(cur_env, &msg);
    if (r != 0) {
        _zdtm_log_error(cur_env, "zdtm_obtain_device_info: zdtm_send_message",
            r);
        return -1;
    }

    /* Receive message back from the Zaurus */
    memset(&rmsg, 0, sizeof(zdtm_msg));
    r = zdtm_recv_message(cur_env, &rmsg);
    if (r != 0) {
        _zdtm_log_error(cur_env, "zdtm_obtain_device_info: zdtm_recv_message",
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

    _zdtm_clean_message(&rmsg);

    return 0;
}

int zdtm_disconnect(zdtm_lib_env *cur_env) {
    int r;
    zdtm_msg msg, rmsg;

    /* send RQT message */
    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RQT_MSG_TYPE, MSG_TYPE_SIZE);
    memset(msg.body.cont.rqt.null_bytes, 0,
        sizeof(msg.body.cont.rqt.null_bytes));

    r = zdtm_send_message(cur_env, &msg);
    if (r != 0) { return -1; }

    memset(&rmsg, 0, sizeof(zdtm_msg));
    r = zdtm_recv_message(cur_env, &rmsg);
    _zdtm_clean_message(&rmsg);
    if (r != 0) { return -2; }

    /* recv rqst message */
    memset(&rmsg, 0, sizeof(zdtm_msg));
    r = _zdtm_recv_message(cur_env, &rmsg);
    if (r != 2) { _zdtm_clean_message(&rmsg); return -3; }

    /* send RAY message */
    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RAY_MSG_TYPE, MSG_TYPE_SIZE);
    r = _zdtm_send_message(cur_env, &msg);
    if (r != 0) { return -4; }

    /* close connection from the Zaurus */
    r = _zdtm_close_zaurus_conn(cur_env);
    if (r != 0) { return -5; }

    /* close connection to the Zaurus */
    r = _zdtm_close_conn_to_zaurus(cur_env);
    if (r != 0) { return -6; }

    return 0;
}
