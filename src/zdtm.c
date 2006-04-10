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
 * along with os_zdtm_plugin; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

/**
 * @file zdtm.c
 * @brief This is an implementation file for a Zaurus DTM support library.
 *
 * The zdtm.c file is an implementation for a Zaurus DTM based
 * synchronization, general support library. It provides all the
 * definitions for all the general functions used to perform the Zaurus
 * side of a synchronization.
 */

#include "zdtm.h"

/**
 * Listen for an incoming synchronization connection from a Zaurus.
 *
 * The zdtm_listen_for_zaurus function creates a socket and configures
 * it to listen for a synchronization connection from a Zaurus. Note:
 * This function does not handle accepting a connection from the Zaurus
 * it just creates a socket and puts it in the proper state so that a
 * Zaurus may make a connection to it, which will be backlogged and can
 * be handled at a later point in time by a function which handles
 * accepting a connection from a Zaurus.
 * @param cur_env Pointer to the current zdtm library environment.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully listening for a Zaurus connection.
 * @retval -1 Failed to create a socket.
 * @retval -2 Failed to set the socket REUSEADDR option.
 * @retval -3 Failed to bind the address to the socket.
 * @retval -4 Failed to put socket into a listening state.
 */
int zdtm_listen_for_zaurus(zdtm_lib_env *cur_env) {
    int retval;
    struct sockaddr_in servaddr;
    int reuse_set_flag = 1;

    cur_env->listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (cur_env->listenfd == -1) {
        perror("zdtm_listen_for_zaurus - socket");
        return -1;
    }

    // Here I set a socket option so that if the applications ends
    // prematurely the socket is not blocked by the TIME_WAIT state.
    retval = setsockopt(cur_env->listenfd, SOL_SOCKET, SO_REUSEADDR,
        (void *)&reuse_set_flag, sizeof(reuse_set_flag));
    if (retval == -1) {
        perror("zdtm_listen_for_zaurus - setsockopt");
        return -2;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(DLISTPORT);

    retval = bind(cur_env->listenfd, (struct sockaddr *)&servaddr,
        sizeof(servaddr));
    if (retval == -1) {
        perror("zdtm_listen_for_zaurus - bind");
        return -3;
    }

    retval = listen(cur_env->listenfd, 1);
    if (retval == -1) {
        perror("zdtm_listen_for_zaurus - listen");
        return -4;
    }

    // Return success
    return 0;
}

/**
 * Handle a Zaurus connection.
 *
 * The zdtm_handle_zaurus_connection function handles a backlogged
 * Zaurus connection if one exists. If no backlogged Zaurus connection
 * exists then zdtm_handle_zaurus_connection will block waiting for a
 * Zaurus connection, at which point it will release after accepting the
 * connection from the Zaurus. Note: In order to handle a Zaurus
 * connection one must first call zdtm_listen_for_zaurus to be in the
 * correct state.
 * @param cur_env Pointer to the current zdtm library environment.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully accepted a Zaurus connection.
 * @retval -1 Failed to accept a Zaurus connection.
 * @retval -2 Failed to convert client address to quad dot format.
 */
int zdtm_handle_zaurus_connection(zdtm_lib_env *cur_env) {
    struct sockaddr_in clntaddr;
    char source_addr[16];
    socklen_t len;
    char *retval;

    memset(&clntaddr, 0, sizeof(clntaddr));
    len = sizeof(clntaddr);
    cur_env->connfd = accept(cur_env->listenfd,
        (struct sockaddr *)&clntaddr, &len);
    if (cur_env->connfd == -1) {
        perror("zdtm_handle_zaurus_connection - accept");
        return -1;
    }

    retval = (char *)inet_ntop(AF_INET, &clntaddr.sin_addr,
        source_addr, 16);
    if (retval == NULL) {
        perror("zdtm_handle_zaurus_connection - inet_ntop");
        return -2;
    }

    //printf("zdtm_handle_zaurus_connection: Received connection from \
    //    %s, port %d.\n", source_addr, ntohs(clntaddr.sin_port));

    // Return in success.
    return 0;
}

/**
 * Close the Zaurus connection.
 *
 * The zdtm_close_zaurus_conn function closes the Zaurus connection.
 * Note: In order for to close a zaurus connection a working zaurus
 * connection must already exist. Hence, the zdtm_listen_for_zaurus and
 * the zdtm_handle_zaurus_conn must have previously been called.
 * @param cur_env Pointer to the current zdtm library environment.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully closed the Zaurus connection.
 * @retval -1 Failed to close the Zaurus connection.
 */
zdtm_close_zaurus_conn(zdtm_lib_env *cur_env) {
    int retval;

    retval = close(cur_env->connfd);
    if (retval) {
        perror("zdtm_close_zaurus_conn - close");
        return -1;
    }
    
    return 0;
}
