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
 * @file zdtm_proto.h
 * @brief This is a specifications file for the zdtm protocol.
 *
 * The zdtm_proto.h file is a specifications file for the Zaurus DTM
 * based synchronization protocols private API. This is a layer which
 * basically provides function prototypes for all the private functions
 * which implement logical components found in the protocol, such as
 * obtain an objects sync id, obtaining device info, etc.
 */

#ifndef ZDTM_PROTO_H
#define ZDTM_PROTO_H

#include "zdtm_types.h"
#include "zdtm_net.h"
#include "zdtm_log.h"

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
int _zdtm_connect(zdtm_lib_env *cur_env, const char *ip_addr);

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
int _zdtm_handle_connection(zdtm_lib_env *cur_env);

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
int _zdtm_obtain_device_info(zdtm_lib_env *cur_env);

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
int _zdtm_disconnect(zdtm_lib_env *cur_env);

#endif
