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

    /* Set the stored Zaurus IP address to all nulls so that I can check
     * it at a later point to see if the user has set it yet. */
    memset(cur_env->zaurus_ip, '\0', IP_STR_SIZE);

    /* Set the sync_type to 0x00, the default value which represents it
     * not having been set yet. */
    cur_env->sync_type = 0x00;

    cur_env->retreived_device_info = 0;

    /* Set the Sync State flags to appropriate initial values. */
    cur_env->retrieved_sync_state = 0;
    cur_env->todo_slow_sync_required = 0;
    cur_env->calendar_slow_sync_required = 0;
    cur_env->address_book_slow_sync_required = 0;

    /* Set the pramaters format to appropriate initial values. */
    cur_env->num_params = 0;
    cur_env->params = NULL;

    r = _zdtm_listen_for_zaurus(cur_env);
    if (r != 0) { return -2; }

    return 0;
}

int zdtm_set_zaurus_ip(zdtm_lib_env *cur_env, char *ip_addr) {

    memcpy(cur_env->zaurus_ip, ip_addr, IP_STR_SIZE);

    return 0;
}

int zdtm_set_sync_type(zdtm_lib_env *cur_env, unsigned int type) {
    if (type == 0) {            /* ToDo */
        cur_env->sync_type = 0x06;
    } else if (type == 1) {     /* Calendar */
        cur_env->sync_type = 0x01;
    } else if (type == 2) {     /* Address Book */
        cur_env->sync_type = 0x07;
    } else {                    /* Default (Not Set Flag) */
        cur_env->sync_type = 0x00;
    }

    return 0;
}

int zdtm_initiate_sync(zdtm_lib_env *cur_env) {
    int r;
    zdtm_msg msg, rmsg;
    char ip_cmp[IP_STR_SIZE];

    if (cur_env->sync_type == 0x00) {
        return -7;
    }
    
    memset(ip_cmp, '\0', IP_STR_SIZE);

    if (memcmp(cur_env->zaurus_ip, ip_cmp, IP_STR_SIZE) == 0) {
        /* The Zaurus IP address has not been set yet */
        return -5;
    }

    r = _zdtm_connect(cur_env, cur_env->zaurus_ip);
    if (r != 0) {
        return -6;
    }

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
    r = _zdtm_wrapped_recv_message(cur_env, &rmsg);
    if (r != 0) {
        _zdtm_log_error(cur_env, "zdtm_initiate_sync: _zdtm_wrapped_recv_message",
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

int zdtm_check_cur_auth_state(zdtm_lib_env *cur_env) {
    int r;

    if (!cur_env->retreived_device_info) { /* get device info, it is needed */
        r = _zdtm_obtain_device_info(cur_env);
        if (r != 0) {
            return -1;
        }
    }

    if ((cur_env->cur_auth_state == 0x0b) ||
        (cur_env->cur_auth_state == 0x07)) {
        return 1;
    }

    return 0;
}

int zdtm_requires_slow_sync(zdtm_lib_env *cur_env) {
    int r;

    if (!cur_env->retrieved_sync_state) { /* get sync state, it is needed */
        r = _zdtm_obtain_sync_state(cur_env);
        if (r != 0) {
            return -1;
        }
    }

    if (cur_env->sync_type == SYNC_TODO) {
        return cur_env->todo_slow_sync_required;
    } else if (cur_env->sync_type == SYNC_CALENDAR) {
        return cur_env->calendar_slow_sync_required;
    } else if (cur_env->sync_type == SYNC_ADDRESSBOOK) {
        return cur_env->address_book_slow_sync_required;
    } else {
        return -2;
    }
}

int zdtm_terminate_sync(zdtm_lib_env *cur_env) {
    int r;
    zdtm_msg msg, rmsg;

    /* send RQT message */
    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RQT_MSG_TYPE, MSG_TYPE_SIZE);
    memset(msg.body.cont.rqt.null_bytes, 0,
        sizeof(msg.body.cont.rqt.null_bytes));

    r = _zdtm_wrapped_send_message(cur_env, &msg);
    if (r != 0) { return -1; }

    memset(&rmsg, 0, sizeof(zdtm_msg));
    r = _zdtm_wrapped_recv_message(cur_env, &rmsg);
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
    r = _zdtm_disconnect(cur_env);
    if (r != 0) { return -5; }

    return 0;
}

int zdtm_finalize(zdtm_lib_env *cur_env) {
    int r, i;

    r = _zdtm_stop_listening(cur_env);
    if (r != 0) { return -2; }

    r = _zdtm_close_log(cur_env);
    if (r != 0) { return -1; }

    if (cur_env->params != NULL) {
        for (i = 0; i < cur_env->num_params; i++) {
            free(cur_env->params[i].desc);
        }
        free(cur_env->params);
    }

    return 0;
}
