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

#ifdef __cplusplus
extern "C" {
#endif

#include "zdtm_export.h"
#include "zdtm_types.h"
#include "zdtm_net.h"
#include "zdtm_log.h"

/**
 * Initialize the library.
 *
 * The zdtm_initialize function prepares the current library environment
 * to be used by all other lib_zdtm_sync API functions as well as
 * putting the library in a state of listening for a connection from the
 * Zaurus. Note: Any connections made to the library are queued until
 * the zdtm_connect() function is called.
 * @param cur_env Pointer to the current zdtm library environment.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully initialized library environment.
 * @retval -1 Failed to open log file.
 * @retval -2 Failed to listen for Zaurus connections.
 */
ZDTM_EXPORT int zdtm_initialize(zdtm_lib_env *cur_env);

/**
 * Connect to Zaurus.
 *
 * The zdtm_connect function connects the current library environment to
 * the Zaurus so that a synchronization may be performed. Note: This
 * should be called directly after zdtm_initalize and only if you want
 * to perform a Desktop initiated synchronization.
 * @param cur_env Pointer to the current zdtm library environment.
 * @param ip_addr IP address of Zaurus to connect to in dotted-quad.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully connected to the Zaurus.
 * @retval -1 Failed to make TCP/IP connection to the Zaurus.
 * @retval -2 Failed to send RAY message to the Zaurus.
 * @retval -3 Falied to handle the incoming Zaurus connection.
 */
ZDTM_EXPORT int zdtm_connect(zdtm_lib_env *cur_env, const char *ip_addr);

/**
 * Handle Connection.
 *
 * The zdtm_handle_connection function handles accepting a
 * synchronization connection that was initation from the Zaurus. If no
 * connections have been made yet then it blocks waiting for a
 * connection to be made from the Zaurus. Note: This should be called
 * directly after zdtm_initialize and only if you are not perform a
 * Desktop initiated synchronization.
 * @param cur_env Pointer to the current zdtm library environment.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully connected to the Zaurus.
 * @retval -1 Failed to handle the incoming Zaurus connection.
 */
ZDTM_EXPORT int zdtm_handle_connection(zdtm_lib_env *cur_env);

/**
 * Initiate Synchronization
 *
 * The zdtm_initiate_sync function initiates the synchronization process
 * by handling the initial handshake of the synchronization protocol.
 * This function should be the first function called after calling the
 * zdtm_connect function.
 * @param cur_env Pointer to currenty zdtm library environment.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully initiated the synchronization process.
 * @retval -1 Received message other than an ack when expecting an ack.
 * @retval -2 Failed to receive ack message.
 * @retval -3 Failed to receive AAY message.
 * @retval -4 Received message other than an AAY when expeccting an AAY.
 */
ZDTM_EXPORT int zdtm_initiate_sync(zdtm_lib_env *cur_env);

/**
 * Obtain Device Information
 *
 * The zdtm_obtain_device_info function obtains the device info (Model,
 * Language, Authentication State, etc). This function updates the key
 * device information stored in the current zdtm library environment.
 * @param cur_env Pointer to the current zdtm library environment.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully obtained and updated device information.
 * @retval -1 Failed to send RIG message.
 * @retval -2 Failed to send AIG message.
 * @retval -3 Received message other than AIG after sending RIG message.
 */
ZDTM_EXPORT int zdtm_obtain_device_info(zdtm_lib_env *cur_env);

/**
 * Terminate Synchronization
 *
 * The zdtm_terminate_sync function terminates the synchronization
 * process by handling the termination handshake of the synchronization
 * protocol.  This function should be called before calling the
 * zdtm_disconnect function.
 * @param cur_env Pointer to currenty zdtm library environment.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully disconnected from the Zaurus.
 * @retval -1 Failed to send RQT message to the Zaurus.
 * @retval -2 Failed to receive AEX message from the Zaurus.
 * @retval -3 Failed to receive request message from the Zaurus.
 * @retval -4 Failed to send RAY message to the Zaurus.
 * @retval -5 Failed to close TCP/IP connection from Zaurus.
 */
ZDTM_EXPORT int zdtm_terminate_sync(zdtm_lib_env *cur_env);

/**
 * Disconnect from Zaurus.
 *
 * The zdtm_disconnect function disconnects the current library
 * environment from the Zaurus terminating the capability of
 * synchronizing over the current library environment.
 * @param cur_env Pointer to the current zdtm library environment.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully disconnected from the Zaurus.
 * @retval -1 Failed to close TCP/IP connection to Zaurus.
 */
ZDTM_EXPORT int zdtm_disconnect(zdtm_lib_env *cur_env);

/**
 * Finalize the library.
 *
 * The zdtm_finalize function finalizes the current library environment
 * so that all the loose ends are taken care of.
 * @param cur_env Pointer to the current zdtm library environment.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully finalized library environment.
 * @retval -1 Failed to close log file.
 */
ZDTM_EXPORT int zdtm_finalize(zdtm_lib_env *cur_env);

#ifdef __cplusplus
}
#endif

#endif
