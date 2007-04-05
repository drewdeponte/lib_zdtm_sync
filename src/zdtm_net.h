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
 * @file zdtm_net.h
 * @brief This is a specifications file for private network functions.
 *
 * The zdtm_net.h file is a specifications file for all the private
 * network handling functions that lib_zdtm_sync uses.
 */

#ifndef ZDTM_NET_H
#define ZDTM_NET_H

#include "zdtm_types.h"
#include "zdtm_log.h"

/**
 * Listen for an incoming synchronization connection from a Zaurus.
 *
 * The _zdtm_listen_for_zaurus function creates a socket and configures
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
int _zdtm_listen_for_zaurus(zdtm_lib_env *cur_env);

/**
 * Stop Listening for incoming sync connections from the Zaurus.
 *
 * The _zdtm_stop_listening function closes the socket which was created
 * to listen for connections. Note: In order to stop listening one must
 * have already started listening by using the _zdtm_listen_for_zaurus()
 * function.
 * @param cur_env Pointer to the current zdtm library environment.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully stopped listening for the Zaurus.
 * @retval -1 Failed to close the socket listening for Zaurus conns.
 */
int _zdtm_stop_listening(zdtm_lib_env *cur_env);

/**
 * Handle a Zaurus connection.
 *
 * The _zdtm_handle_zaurus_connection function handles a backlogged
 * Zaurus connection if one exists. If no backlogged Zaurus connection
 * exists then _zdtm_handle_zaurus_connection will block waiting for a
 * Zaurus connection, at which point it will release after accepting the
 * connection from the Zaurus. Note: In order to handle a Zaurus
 * connection one must first call _zdtm_listen_for_zaurus to be in the
 * correct state.
 * @param cur_env Pointer to the current zdtm library environment.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully accepted a Zaurus connection.
 * @retval -1 Failed to accept a Zaurus connection.
 */
int _zdtm_handle_zaurus_conn(zdtm_lib_env *cur_env);

/**
 * Close the Zaurus connection.
 *
 * The _zdtm_close_zaurus_conn function closes the Zaurus connection.
 * Note: In order to close a zaurus connection, a working zaurus
 * connection must already exist. Hence, the _zdtm_listen_for_zaurus and
 * the _zdtm_handle_zaurus_conn must have previously been called and
 * succeeded.
 * @param cur_env Pointer to the current zdtm library environment.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully closed the Zaurus connection.
 * @retval -1 Failed to close the Zaurus connection.
 */
int _zdtm_close_zaurus_conn(zdtm_lib_env *cur_env);

/**
 * Connect to Zaurus
 *
 * The _zdtm_conn_to_zaurus function initiates a connection to the
 * Zaurus. This connection is used to initiate a synchornization
 * originating from the desktop.
 * @param cur_env Pointer to the current zdtm library environment.
 * @param zaurus_ip A string containing dotted quad zaurus ip address.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully connected to the Zaurus.
 * @retval -1 Failed to create a socket to use to connect to the zaurus.
 * @retval -2 Failed to convert the zaurus ip address.
 * @retval -3 Failed to connect to the zaurus.
 */
int _zdtm_conn_to_zaurus(zdtm_lib_env *cur_env, const char *zaurus_ip);

/**
 * Close the connection to the Zaurus.
 *
 * The _zdtm_close_conn_to_zaurus function closes the connetion which was
 * made to the Zaurus to request a synchronization. Note: In order to
 * close a connection using this function a working connection to the
 * zaurus must already exist. Hence, the _zdtm_conn_to_zaurus must have
 * previously been caled and succeeded.
 * @param cur_env Pointer to the current zdtm library environment.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully closed the connection to the Zaurus.
 * @retval -1 Failed to close the connection to the Zaurus.
 */
int _zdtm_close_conn_to_zaurus(zdtm_lib_env *cur_env);

/**
 * Send a raw common message.
 *
 * Send a specified raw common message to a specified socket.
 * @param sockfd Socket descriptor to write the raw common message to.
 * @param data Pointe to buffer containing raw common message.
 * @return An SOCKET representing success (zero) or failure (non-zero).
 * @retval 0 Successfully sent common messaeg.
 * @retval -1 Failed to write raw common message to socket descriptor.
 */
int _zdtm_send_comm_message_to(SOCKET sockfd, char *data);

/**
 * Send acknowledgement message.
 *
 * Send an acknowledgement message to the Zaurus. For, details about
 * specific return values please refer to the return values of the
 * _zdtm_send_comm_message function.
 * @param cur_env Pointer to current zdtm library environment.
 * @return An integer representing success (zero) or failure (non-zero).
 */
int _zdtm_send_ack_message(zdtm_lib_env *cur_env);

/**
 * Send request message.
 *
 * Send a request message to the Zaurus. For, details about specific
 * return values please refer to the return values of the
 * _zdtm_send_comm_message function.
 * @param cur_env Pointer to current zdtm library environment.
 * @return An integer representing success (zero) or failure (non-zero).
 */
int _zdtm_send_rqst_message(zdtm_lib_env *cur_env);

/**
 * Send abort message.
 *
 * Send an abort message to the Zaurus. For, details about specific
 * return values please refer to the return values of the
 * _zdtm_send_comm_message function.
 * @param cur_env Pointer to current zdtm library environment.
 * @return An integer representing success (zero) or failure (non-zero).
 */
int _zdtm_send_abrt_message(zdtm_lib_env *cur_env);

/**
 * Receive Message.
 *
 * The _zdtm_recv_message function handles receiving a message from the
 * Zaurus after a connection from the Zaurus has already been handled
 * via the _zdtm_handle_zaurus_conn function. This function supports
 * receiving common messages as well as non-common messages. When,
 * receiving a common message the structure pointed to by p_msg is not
 * altered. Note: When this function alters the structure pointed to by
 * p_msg it dynamically allocates memory for the message content. The
 * freeing of this allocated memory for the message content must be
 * handled by you, using the _zdtm_clean_message function. If the
 * function returns in error freeing the message is still required via
 * the _zdtm_clean_message function.
 * @param cur_env Pointer to the current zdtm library environment.
 * @param p_msg Pointer to a zdtm_message structure to store message in.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully received a message from the Zaurus.
 * @retval 1 Successfully received an ack message from the Zaurus.
 * @retval 2 Successfully received a request message from the Zaurus.
 * @retval 3 Successfully received an abort message from the Zaurus.
 * @retval -1 Socket cleanly closed by opposing side.
 * @retval -2 Socket closed by opposing side in middle of recving a message.
 * @retval -3 Failed to successfully read message from connection.
 * @retval -4 Failed to allocate memory for temporary buffer.
 * @retval -5 Failed, p_msg is NULL (no where to store message).
 * @retval -6 Failed, message raw content is not initialized to NULL.
 * @retval RET_MEM_CONTENT Failed to allocate mem for message content.
 * @retval RET_PARSE_RAW_FAIL Failed to parse the raw message.
 */
int _zdtm_recv_message(zdtm_lib_env *cur_env, zdtm_msg *p_msg);

/**
 * Send Message to Zaurus.
 *
 * The _zdtm_send_message function is a general send message function
 * which sends the provided message to the Zaurus via the connection
 * which is established from the Zaurus. For specifics of return values
 * please refer to the return values of the _zdtm_send_message_to
 * function.
 * @param cur_env Pointer to the current zdtm library environment.
 * @param p_msg Pointer to the zdtm_message struct containing message.
 * @return An integer representing success (zero) or failure (non-zero).
 */
int _zdtm_send_message(zdtm_lib_env *cur_env, zdtm_msg *p_msg);

/**
 * Send Message.
 *
 * The _zdtm_send_message_to function handles sending a message to the
 * Zaurus after a connection from the Zaurus has already been handled
 * via the _zdtm_handle_zaurus_conn function. This function takes a message
 * which has its type, and cont filled out and compiles it into the
 * proper format and sends it over the socket specified by the given
 * socket descriptor.
 * @param cur_env Pointer to the current zdtm library environment.
 * @param p_msg Pointer to a zdtm_message structure to store message in.
 * @param sockfd The socket to send the message over.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully sent the message via the connection socket.
 * @retval -1 Failed to prepare message.
 * @retval RET_MALLOC_FAIL Failed to allocate memory for raw message.
 * @retval -2 Failed to write raw message to the connection socket.
 */
int _zdtm_send_message_to(zdtm_lib_env *cur_env, zdtm_msg *p_msg, int sockfd);

/**
 * Send a Message
 *
 * The zdtm_wrapped_send_message function sends a message to the Zaurus
 * after first receiving a request message, and terminates with success
 * only after receiving an acknowledegment message from the Zaurus.
 * @param cur_env Pointer to the current zdtm library environment.
 * @param msg Pointer to zdtm_msg structure to send to the Zaurus.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully sent message to the Zaurus.
 * @retval -1 Failed to receive request message from the Zaurus.
 * @retval -2 Failed to send the message to the Zaurus.
 * @retval -3 Failed to receive acknowledgement message from Zaurus.
 */
int _zdtm_wrapped_send_message(zdtm_lib_env *cur_env, zdtm_msg *msg);

/**
 * Receive a Message
 *
 * The zdtm_wrapped_recv_message function receives a message from the
 * Zaurus after first sending a request message, and terminates with
 * success only after sending an acknowledegment message to the Zaurus.
 * @param cur_env Pointer to the current zdtm library environment.
 * @param msg Pointer to zdtm_msg struct to store received message in.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully received message to the Zaurus.
 * @retval 1 Successfully received an ack message from the Zaurus.
 * @retval 2 Successfully received a request message from the Zaurus.
 * @retval 3 Successfully received an abort message from the Zaurus.
 * @retval -1 Failed to send request message to the Zaurus.
 * @retval -2 Failed to receive message from the Zaurus.
 * @retval -3 Failed to send acknowledgement message to the Zaurus.
 */
int _zdtm_wrapped_recv_message(zdtm_lib_env *cur_env, zdtm_msg *msg);

#endif
