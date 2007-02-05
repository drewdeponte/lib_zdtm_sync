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
 * @file zdtm_log.h
 * @brief This is a specifications file for private log functions.
 *
 * The zdtm_log.h file is a specifications file which specifies all the
 * private logging functions that lib_zdtm_uses.
 */

#ifndef ZDTM_LOG_H
#define ZDTM_LOG_H

#include "zdtm_types.h"
#include "zdtm_msgs.h"

/**
 * Open zdtm library log file.
 *
 * The _zdtm_open_log function opens the zdtm libraries log file in
 * append mode so that the log may be written to.
 * @param cur_env Pointer to the current zdtm library environment.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully opened the zdtm library log file.
 * @retval -1 Failed to get HOME environment variable.
 * @retval -2 Not enough free bytes in internal buff to create file path.
 * @retval -3 Failed to open the created file path to append.
 */
int _zdtm_open_log(zdtm_lib_env *cur_env);

/**
 * Write log to zdtm library log file.
 *
 * The _zdtm_write_log function writes content to the zdtm libraries log
 * file in append mode. This function also flushes the file stream so
 * that the content is written to the log file right away, rather than
 * waiting in a buffer somewhere.
 * @param cur_env Pointer to the current zdtm library environment.
 * @param buff Pointer to the content to write to the log.
 * @param size The size, in bytes, of the content to write to the log.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully wrote to the zdtm library log file.
 * @retval -1 Failed, the log file is NOT opened.
 * @retval -2 Wrote fewer bytes to the log than were requested.
 * @retval -3 Failed to flush the log file stream.
 */
int _zdtm_write_log(zdtm_lib_env *cur_env, const char *buff,
    unsigned int size);

int _zdtm_log_error(zdtm_lib_env *cur_env, const char *func_name, int err);

/**
 * Close zdtm library log file.
 *
 * The _zdtm_close_log function closes the zdtm libraries log file.
 * @param cur_env Pointer to the current zdtm library environment.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully closed the zdtm library log file.
 * @retval -1 Failed to close zdtm library log file.
 * @retval -2 Failed, zdtm library log file was NOT open.
 */
int _zdtm_close_log(zdtm_lib_env *cur_env);

/**
 * Dump a zdtm message to the log file.
 *
 * The _zdtm_dump_msg_log function takes a zdtm_msg which has either been
 * read from the network connection using the _zdtm_recv_message function
 * or has been prepared with the _zdtm_prepare_message function and
 * outputs the messages header, type, content, content size, body size
 * and check sum in a readable fashion.
 * @param cur_env Pointer to the current zdtm library environment.
 * @param p_msg Pointer to the message to dump to the log.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully dumped zdtm mesasge to log file.
 * @retval -1 Failed to construct data buff to write to log.
 * @retval -2 Failed, truncated data to fit in buff to write to log.
 * @retval -3 Failed to write data to log.
 */
int _zdtm_dump_msg_log(zdtm_lib_env *cur_env, zdtm_msg *p_msg);

#endif
