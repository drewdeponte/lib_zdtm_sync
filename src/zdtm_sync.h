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
#include "zdtm_proto.h"
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
 * Set the Zaurus IP address.
 *
 * The zdtm_set_zaurus_ip function sets the Zaurus IP address in the
 * the passed zdtm_lib_env structure so that it may be used throughout
 * the rest of the synchronization process. Note: This function also
 * acts as a flag. Hence, if this function is called it is assumed that
 * the client program is using the network based backend rather than the
 * direct USB backend.
 * @param cur_env Pointer to the current zdtm library environment.
 * @ip_addr C-String containing a dotted-quad string of the zaurus ip.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully set the zaurus ip address.
 */
ZDTM_EXPORT int zdtm_set_zaurus_ip(zdtm_lib_env *cur_env, char *ip_addr);

/**
 * Set the Synchronization type.
 *
 * The zdtm_set_sync_type function sets the synchronization type for the
 * current zdtm_lib_env structure so that it may be used throughout the
 * rest of the synchronization process. Note: This function must be
 * called before the zdtm_initate_sync() function, or
 * zdtm_initiate_sync() will return in error.
 * @param cur_env Pointer to the current zdtm library environment.
 * @type sync type (0 - Todo, 1 - Calendar, 2 - Address Book)
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully set the synchronization type.
 */
ZDTM_EXPORT int zdtm_set_sync_type(zdtm_lib_env *cur_env, unsigned int type);

/**
 * Set the Passcode.
 *
 * The zdtm_set_passcode() function sets the passcode to use in the
 * synchronization if a passcode is required.
 * @param cur_env Pointer to the current zdtm library environment.
 * @param passcode Pointer to C-string containing the passcode to use.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully set the synchronization passcode.
 * @retval -1 Failed to allocate memory to store the passcode.
 */
ZDTM_EXPORT int zdtm_set_passcode(zdtm_lib_env *cur_env, char *passcode);

/**
 * Initiate Synchronization
 *
 * The zdtm_initiate_sync function initiates the synchronization process
 * by handling the initial handshake of the synchronization protocol.
 * This function should be the first function called after calling the
 * zdtm_connect function.
 * @param cur_env Pointer to current zdtm library environment.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully initiated the synchronization process.
 * @retval -1 Received message other than an ack when expecting an ack.
 * @retval -2 Failed to receive ack message.
 * @retval -3 Failed to receive AAY message.
 * @retval -4 Received message other than an AAY when expeccting an AAY.
 * @retval -5 The Zaurus IP address has not been set yet.
 * @retval -6 Failed to connect to the Zaurus.
 * @retval -7 The synchronization type has not been set yet.
 */
ZDTM_EXPORT int zdtm_initiate_sync(zdtm_lib_env *cur_env);

/**
 * Check current authentication state.
 *
 * The zdtm_check_cur_auth_state function checks the cur_auth_state
 * which is kept in the current zdtm library environment and returns a
 * value representing one of it's possible states.
 * @param cur_env Pointer to current zdtm library environment.
 * @return An integer representing current authentication state.
 * @retval 0 current auth state does NOT require a password.
 * @retval 1 current auth state DOES require a password.
 * @retval -1 Failed to obtain device info to check auth state.
 */
ZDTM_EXPORT int zdtm_check_cur_auth_state(zdtm_lib_env *cur_env);

/**
 * Check if requires a slow sync.
 *
 * The zdtm_requires_slow_sync function checks the sync state obtained
 * from the Zaurus to see if a slow sync is required for the current
 * synchronization type.
 * @param cur_env Pointer to current zdtm library environment.
 * @return An integer representing true/false/error (1/0/negative num).
 * @retval 0 Does NOT require a slow sync.
 * @retval 1 DOES require a slow sync.
 * @retval -1 Failed to obtain sync state.
 * @retval -2 Sync type is not a recognized sync type.
 */
ZDTM_EXPORT int zdtm_requires_slow_sync(zdtm_lib_env *cur_env);

/**
 * Obtain Sync ID Lists
 *
 * The zdtm_obtain_sync_id_lists function attempts to obtain the three
 * lists of synchronization IDs from the Zaurus. One list contains all
 * the IDs of the new items, one contains all the IDs of modified items,
 * and the third list contains all the IDs of deleted items. In the case
 * of failure none of the vairiables pointed to by the parameters will
 * be altered. If this function succeeds the memory containing the lists
 * needs to be freed using the free() function because it is allocated
 * dynamically inside this function.
 * @param cur_env Pointer to the current zdtm library environment.
 * @param pp_new_sync_ids Pointer to a pointer to head of new ID list.
 * @param p_num_new_sync_ids Pointer to var to store num new IDs in.
 * @param pp_mod_sync_ids Pointer to a pointer to head of mod ID list.
 * @param p_num_mod_sync_ids Pointer to var to store num mod IDs in.
 * @param pp_del_sync_ids Pointer to a pointer to head of del ID list.
 * @param p_num_del_sync_ids Pointer to var to store num del IDs in.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully obtained sync ID lists.
 * @retval -1 Failed to send RSY message.
 * @retval -2 Failed to recv response message.
 * @retval -3 Failed, response message is NOT an ASY message.
 * @retval -4 Failed to allocate memory for ID lists.
 */
ZDTM_EXPORT int zdtm_obtain_sync_id_lists(zdtm_lib_env *cur_env,
    uint32_t **pp_new_sync_ids, uint16_t *p_num_new_sync_ids,
    uint32_t **pp_mod_sync_ids, uint16_t *p_num_mod_sync_ids,
    uint32_t **pp_del_sync_ids, uint16_t *p_num_del_sync_ids);

/**
 * Obtain Todo Item.
 *
 * The zdtm_obtain_todo_item function attempts to obtain the data for a
 * Todo item and build a struture to represent the Todo item given a
 * sync id.
 * @param cur_env Pointer to the current zdtm library environment.
 * @param sync_id The sync id of the item to obtain.
 * @param p_todo_item Pointer to zdtm_todo_item structure to store
 * results in.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully obtained Todo item.
 * @retval -1 Failed, current environment is not set to Todo sync type.
 * @retval -2 Failed to obtain item data from the Zaurus.
 * @retval -3 Failed to build zdtm_todo_item struct from item data.
 * */
ZDTM_EXPORT int zdtm_obtain_todo_item(zdtm_lib_env *cur_env,
    uint32_t sync_id, struct zdtm_todo_item *p_todo_item);

/**
 * Obtain Calendar Item.
 *
 * The zdtm_obtain_calendar_item function attempts to obtain the data
 * for a Calendar item and build a struture to represent the Calendar
 * item given a sync id.
 * @param cur_env Pointer to the current zdtm library environment.
 * @param sync_id The sync id of the item to obtain.
 * @param p_calendar_item Pointer to zdtm_calendar_item structure to
 * store results in.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully obtained Todo item.
 * @retval -1 Failed, current environment is not set to Calendar sync type.
 * @retval -2 Failed to obtain item data from the Zaurus.
 * @retval -3 Failed to build zdtm_calendar_item struct from item data.
 * */
ZDTM_EXPORT int zdtm_obtain_calendar_item(zdtm_lib_env *cur_env,
    uint32_t sync_id, struct zdtm_calendar_item *p_calendar_item);

/**
 * Obtain Address Item.
 *
 * The zdtm_obtain_address_item function attempts to obtain the data
 * for a Address item and build a struture to represent the Address item
 * given a sync id.
 * @param cur_env Pointer to the current zdtm library environment.
 * @param sync_id The sync id of the item to obtain.
 * @param p_address_item Pointer to zdtm_address_item structure to store
 * results in.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully obtained Address item.
 * @retval -1 Failed, current environment is not set to Address sync type.
 * @retval -2 Failed to obtain item data from the Zaurus.
 * @retval -3 Failed to build zdtm_address_item struct from item data.
 * */
ZDTM_EXPORT int zdtm_obtain_address_item(zdtm_lib_env *cur_env,
    uint32_t sync_id, struct zdtm_address_item *p_address_item);

/**
 * Delete Item.
 *
 * The zdtm_delete_item function attempts to delete an item from the
 * Zaurus database. This is generally done with a Desktop software has
 * had the item matching the given sync_id removed as well.
 * @param cur_env Pointer to the current zdtm library environment.
 * @param sync_id The sync id of the item to delete.
 * @return An integer representintg success (zero) or failure (non-zero).
 * @retval 0 Successfully deleted the item.
 * @retval -1 Failed to sent the RDD message.
 * @retval -2 Failed to recv the response message.
 * @retval -3 Failed, response message received was NOT an AEX message.
 */
ZDTM_EXPORT int zdtm_delete_item(zdtm_lib_env *cur_env, uint32_t sync_id);

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
 * Finalize the library.
 *
 * The zdtm_finalize function finalizes the current library environment
 * so that all the loose ends are taken care of.
 * @param cur_env Pointer to the current zdtm library environment.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully finalized library environment.
 * @retval -1 Failed to close log file.
 * @retval -2 Failed to stop listening for Zaurus connections.
 */
ZDTM_EXPORT int zdtm_finalize(zdtm_lib_env *cur_env);

#ifdef __cplusplus
}
#endif

#endif
