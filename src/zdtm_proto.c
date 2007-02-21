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
