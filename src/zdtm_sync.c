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

/**
 * Convert little-endian short to a big-endian short.
 *
 * The zdtm_liltobigs function converts a little-endian uint16_t
 * to a big-endian uint16_t.
 * @param lilshort The lil-end uint16_t to convert to big-end uint16_t.
 * @return The big-endian version of given little-endian uint16_t.
 */
uint16_t zdtm_liltobigs(uint16_t lilshort) {
    int size, i;
    unsigned char buff[sizeof(uint16_t)];

    size = sizeof(uint16_t);

    for (i = 0; i < size; i++) {
        buff[i] = ((unsigned char *)&lilshort)[(size - i - 1)];
    }

    return *(uint16_t *)buff;
}

/**
 * Convert little-endian long to a big-endian long.
 *
 * The zdtm_liltobigl function converts a little-endian uint32_t
 * to a big-endian uint32_t.
 * @param lillong The lil-end uint32_t to convert to big-end uint32_t.
 * @return The big-endian version of given little-endian uint32_t.
 */
uint32_t zdtm_liltobigl(uint32_t lillong) {
    int size, i;
    unsigned char buff[sizeof(uint32_t)];

    size = sizeof(uint32_t);

    for (i = 0; i < size; i++) {
        buff[i] = ((unsigned char *)&lillong)[(size - i - 1)];
    }

    return *(uint32_t *)buff;
}

/**
 * Convert big-endian short to a little-endian short.
 *
 * The zdtm_bigtolils function converts a big-endian uint16_t to a
 * little-endian uint16_t.
 * @param bigshort The big-end uint16_t to convert to lil-end uint16_t.
 * @return The little-endian version of given big-endian uint16_t.
 */
uint16_t zdtm_bigtolils(uint16_t bigshort) {
    return zdtm_liltobigs(bigshort);
}

/**
 * Convert big-endian long to a little-endian long.
 *
 * The zdtm_bigtolils function converts a big-endian uint32_t to a
 * little-endian uint32_t.
 * @param biglong The big-end uint32_t to convert to lil-end uint32_t.
 * @return The little-endian version of given big-endian uint32_t.
 */
uint32_t zdtm_bigtolill(uint32_t biglong) {
    return zdtm_liltobigl(biglong);
}

/**
 * Sum all the bytes in a buffer.
 *
 * @param buf The buffer to sum.
 * @param n The number of bytes in the buffer.
 */
uint16_t zdtm_checksum(unsigned char *buf, uint16_t n) {
    uint16_t sum = 0;

    for(;n > 0; --n){
        sum += *(buf++);
    }

    return sum;
}

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
int zdtm_handle_zaurus_conn(zdtm_lib_env *cur_env) {
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

    /*
    printf("zdtm_handle_zaurus_connection: Received connection from \
        %s, port %d.\n", source_addr, ntohs(clntaddr.sin_port));
    */

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
int zdtm_close_zaurus_conn(zdtm_lib_env *cur_env) {
    int retval;

    retval = close(cur_env->connfd);
    if (retval) {
        perror("zdtm_close_zaurus_conn - close");
        return -1;
    }
    
    return 0;
}

/**
 * Connect to Zaurus
 *
 * The zdtm_conn_to_zaurus function initiates a connection to the
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
int zdtm_conn_to_zaurus(zdtm_lib_env *cur_env, const char *zaurus_ip) {
    struct sockaddr_in servaddr;
    int retval;

    cur_env->reqfd = socket(AF_INET, SOCK_STREAM, 0);
    if (cur_env->reqfd == -1) {
        perror("zdtm_conn_to_zaurus - socket");
        return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(ZLISTPORT);
    if (inet_pton(AF_INET, zaurus_ip, &servaddr.sin_addr) <= 0) {
        perror("zdtm_conn_to_zaurus - inet_pton");
        return -2;
    }

    retval = connect(cur_env->reqfd, (struct sockaddr *)&servaddr,
        sizeof(servaddr));
    if(retval == -1) {
        perror("zdtm_conn_to_zaurus - connect");
        return -3;
    }

    return 0;
}

/**
 * Open zdtm library log file.
 *
 * The zdtm_open_log function opens the zdtm libraries log file in
 * append mode so that the log may be written to.
 * @param cur_env Pointer to the current zdtm library environment.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully opened the zdtm library log file.
 * @retval -1 Failed to get HOME environment variable.
 * @retval -2 Not enough free bytes in internal buff to create file path.
 * @retval -3 Failed to open the created file path to append.
 */
int zdtm_open_log(zdtm_lib_env *cur_env) {
    char *home_env;
    char file_path[256];
    int free_bytes;

    home_env = getenv("HOME");
    if (home_env == NULL) {
        return -1;
    }

    strncpy(file_path, home_env, 256);

    free_bytes = 256 - strlen(file_path) - 1;
    /*
     * Note: the 19 check here is tied to length of cat str below hence
     * if the name of the cat str used for the log file below changes
     * then the 19 should change to match the length of the new cat str
     * as well.
     */
    if (free_bytes >= 19) {
        strncat(file_path, "/.lib_zdtm_sync.log", free_bytes);
    } else {
        return -2;
    }

    cur_env->logfp = fopen(file_path, "a");
    if (cur_env->logfp == NULL) {
        perror("zdtm_open_log - fopen");
        return -3;
    }

    return 0;
}

/**
 * Write log to zdtm library log file.
 *
 * The zdtm_write_log function writes content to the zdtm libraries log
 * file in append mode. This function also flushes the file stream so
 * that the content are written to the log file now, rather than waiting
 * in a buffer somewhere.
 * @param cur_env Pointer to the current zdtm library environment.
 * @param buff Pointer to the content to write to the log.
 * @param size The size, in bytes, of the content to write to the log.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully wrote to the zdtm library log file.
 * @retval -1 Failed, the log file is NOT opened.
 * @retval -2 Wrote fewer bytes to the log than were requested.
 * @retval -3 Failed to flush the log file stream.
 */
int zdtm_write_log(zdtm_lib_env *cur_env, const unsigned char *buff,
    unsigned int size) {
    int bytes_written;  // The number of bytes written

    if (cur_env->logfp == NULL) {
        return -1;
    }

    bytes_written = fwrite((const void *)buff, 1, (size_t)size,
        cur_env->logfp);
    if (bytes_written < size) {
        return -2;
    }

    if (fflush(cur_env->logfp) != 0) {
        perror("zdtm_write_log - fflush");
        return -3;
    }
   
    return 0;
}

/**
 * Close zdtm library log file.
 *
 * The zdtm_close_log function closes the zdtm libraries log file.
 * @param cur_env Pointer to the current zdtm library environment.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully closed the zdtm library log file.
 * @retval -1 Failed to close zdtm library log file.
 * @retval -2 Failed, zdtm library log file was NOT open.
 */
int zdtm_close_log(zdtm_lib_env *cur_env) {
    int retval;

    if (cur_env->logfp != NULL) {
        retval = fclose(cur_env->logfp);
        if (retval != 0) {
            perror("zdtm_close_log - fclose");
            return -1;
        }
        return 0;
    }

    return -2;
}

/**
 * Check if a message is an Ack message.
 *
 * The zdtm_is_ack_message() function checks if a message is an ack
 * message. This function is designed to test if the first n bytes are
 * equivalent, where n is the size in bytes of the common messages. This
 * would allow for buffers of greater length to be passed to this
 * function and still be evaluated true if the first n bytes of the
 * buffer are equivalent to the bytes of the ack common message.
 * @param buff Pointer to raw message to compare against.
 * @return An integer representing a boolean value.
 * @retval 1 True, the message is an Ack message.
 * @retval 0 False, the message is NOT an ack message.
 */
int zdtm_is_ack_message(const unsigned char *buff) {
    char msg_data[COM_MSG_SIZE] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x96,
        0x06};

    if (memcmp(buff, msg_data, COM_MSG_SIZE) == 0) {
        // return 1 stating that it is an ack message.
        return 1;
    }

    // return 0 stating that it is NOT an ack message.
    return 0;
}

/**
 * Check if a message is an Request message.
 *
 * The zdtm_is_rqst_message() function checks if a message is a request
 * message. This function is designed to test if the first n bytes are
 * equivalent, where n is the size in bytes of the common messages. This
 * would allow for buffers of greater length to be passed to this
 * function and still be evaluated true if the first n bytes of the
 * buffer are equivalent to the bytes of the rqst common message.
 * @param buff Pointer to raw message to compare against.
 * @return An integer representing a boolean value.
 * @retval 1 True, the message is a rqst message.
 * @retval 0 False, the message is NOT a rqst message.
 */
int zdtm_is_rqst_message(const unsigned char *buff) {
    char msg_data[COM_MSG_SIZE] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x96,
        0x05};

    if (memcmp(buff, msg_data, COM_MSG_SIZE) == 0) {
        // return 1 stating that it is a rqst message.
        return 1;
    }

    // return 0 stating that it is NOT a rqst message.
    return 0;
}

/**
 * Check if a message is an Abort message.
 *
 * The zdtm_is_abrt_message() function checks if a message is an abort
 * message. This function is designed to test if the first n bytes are
 * equivalent, where n is the size in bytes of the common messages. This
 * would allow for buffers of greater length to be passed to this
 * function and still be evaluated true if the first n bytes of the
 * buffer are equivalent to the bytes of the abort common message.
 * @param buff Pointer to raw message to compare against.
 * @return An integer representing a boolean value.
 * @retval 1 True, the message is an abrt message.
 * @retval 0 False, the message is NOT an abrt message.
 */
int zdtm_is_abrt_message(const unsigned char *buff) {
    char msg_data[COM_MSG_SIZE] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x96,
        0x18};

    if (memcmp(buff, msg_data, COM_MSG_SIZE) == 0) {
        // return 1 stating that it is an abrt message.
        return 1;
    }

    // return 0 stating that it is NOT an abrt message.
    return 0;
}

/**
 * Clean Message
 *
 * The zdtm_clean_message function handles the checking and freeing of
 * any members of the message that have been dynamically allocated. Such
 * dynamic allocation occurs in the zdtm_recv_message function.
 * @param p_msg Pointer to a zdtm_message structure to free members of.
 * @return An integer representing success (zero) or failure (non-zero).
 */
int zdtm_clean_message(zdtm_msg *p_msg) {
    if (p_msg->body.p_raw_content != NULL) {
        free(p_msg->body.p_raw_content);
        p_msg->body.p_raw_content = NULL;
    }

    return 0;
}

/**
 * Receive Message.
 *
 * The zdtm_recv_message function handles receiving a message from the
 * Zaurus after a connection from the Zaurus has already been handled
 * via the zdtm_handle_zaurus_conn() function. This function supports
 * receiving common messages as well as non-common messages. When,
 * receiving a common message the structure pointed to by p_msg is not
 * altered. Note: When this function alters the structure pointed to by
 * p_msg it dynamically allocates memory for the message content. The
 * freeing of this allocated memory for the message content must be
 * handled by you. If the function returns in error one can determine if
 * the content is needed to be freed by checking if the content pointer
 * is equal no NULL. If it is equal to NULL there is no need to free it.
 * If it is NOT equal to NULL it must be freed. The zdtm_clean_message
 * function may help with handling the freeing of message content.
 * @param cur_env Pointer to the current zdtm library environment.
 * @param p_msg Pointer to a zdtm_message structure to store message in.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully received a message from the Zaurus.
 * @retval 1 Successfully received an ack message from the Zaurus.
 * @retval 2 Successfully received a request message from the Zaurus.
 * @retval 3 Successfully received an abort message from the Zaurus.
 * @retval -1 Failed to allocate mem for temp message buffer.
 * @retval -2 Falied to read, reached end-of-file on a socket.
 * @retval -3 Failed to successfully read message from connection.
 * @retval -4 Failed to identify 7 byte message.
 * @retval -5 Failed to identify less than 20 byte message.
 * @retval -6 Failed, p_msg is NULL (no where to store message).
 * @retval RET_NNULL_RAW Failed, message raw content is not initialized to NULL.
 * @retval RET_SIZE_MISMATCH  Failed, bytes in != expected size 
 *                            (based on body size).
 *
 * @retval RET_MEM_CONTENT Failed to allocate mem for message content.
 */
int zdtm_recv_message(zdtm_lib_env *cur_env, zdtm_msg *p_msg) {
    ssize_t bytes_read;
    uint32_t max_buff_size;
    unsigned char *buff;
    unsigned char *cur_buff_pos;

    cur_buff_pos = NULL;

    max_buff_size = MSG_HDR_SIZE + sizeof(uint16_t) + UINT16_MAX + \
        sizeof(uint16_t);
   
    buff = (unsigned char *)malloc((size_t)max_buff_size);
    if (buff == NULL) {
        perror("zdtm_recv_message - malloc");
        return -1;
    }
   
    bytes_read = read(cur_env->connfd, (void *)buff, max_buff_size);
    if (bytes_read == 0) {
        // Reached the end of file
        free((void *)buff);
        return -2;
    } else if (bytes_read == -1) {
        // read - returned an error and set errno
        free((void *)buff);
        return -3;
        perror("zdtm_recv_message - read");
    }

    // Check for common messages and unknown messages based on size
    if (bytes_read < 20) {
        if (bytes_read == COM_MSG_SIZE) {
            if (zdtm_is_ack_message(buff)) {
                free((void *)buff);
                return 1;
            } else if (zdtm_is_rqst_message(buff)) {
                free((void *)buff);
                return 2;
            } else if (zdtm_is_abrt_message(buff)) {
                free((void *)buff);
                return 3;
            } else {
                free((void *)buff);
                // return signifying received 7 bytes of unknown msg data
                return -4;
            }
        } else {
            free((void *)buff);
            // signifying received less than 20 bytes of unkown data
            return -5;
        }
    }

    /*
     * Since it made it this far I know that it is an acceptable message
     * based on size alone. Hence, I am going to need somewhere to put
     * the data.
     */
    if (p_msg == NULL) {
        free((void *)buff);
        return -6;
    }

    /*
     * Check if the body raw content is initialized to NULL so that
     * in-case of failure at some point so the user can determine if
     * they need to free the content or not.
     */
    if (p_msg->body.p_raw_content != NULL) {
        free((void *)buff);
        return -7;
    }

    /*
     * Now that I know the size is acceptable I am going to parse the
     * data into the proper pieces to fill the zdtm_message structure so
     * that the information may be easily obtained later on. Note, I am
     * only parsing into to the raw portions. Later parsing will occur
     * in a seperate function for each specific type of message.
     */

    cur_buff_pos = buff;

    // Set the zdtm_message header
    memcpy((void *)p_msg->header, (const void *)cur_buff_pos,
        MSG_HDR_SIZE);
    cur_buff_pos = cur_buff_pos + MSG_HDR_SIZE;

    // Set the zdtm_message body size
    p_msg->body_size = *((uint16_t *)(cur_buff_pos));
#ifdef WORDS_BIGENDIAN
    p_msg->body_size = zdtm_liltobigs(p_msg->body_size);
#endif
    cur_buff_pos = cur_buff_pos + sizeof(uint16_t);

    // Check to see if the number of bytes read in coincides with the
    // body size that was just obtained from the raw message data.
    if (bytes_read != (MSG_HDR_SIZE + sizeof(uint16_t) + \
        p_msg->body_size + sizeof(uint16_t))) {
        free((void *)buff);
        // return signifying that body size miss-match
        return RET_SIZE_MISMATCH;
    }

    // Set the zdtm_message_body type
    memcpy((void *)p_msg->body.type, (const void *)cur_buff_pos,
        MSG_TYPE_SIZE);
    cur_buff_pos = cur_buff_pos + MSG_TYPE_SIZE;

    // Set the zdtm_message cont_size and the zdtm_message_body content
    p_msg->cont_size = p_msg->body_size - MSG_TYPE_SIZE;
    p_msg->body.p_raw_content = malloc((size_t)p_msg->cont_size);
    if (p_msg->body.p_raw_content == NULL) {
        free((void *)buff);
        // return signifying that malloc failed
        perror("zdtm_recv_message - malloc");
        return RET_MEM_CONTENT;
    }
    memcpy(p_msg->body.p_raw_content, (const void *)cur_buff_pos,
        p_msg->cont_size);
    cur_buff_pos = cur_buff_pos + p_msg->cont_size;

    // Set the zdtm_message check_sum
    p_msg->check_sum = *((uint16_t *)(cur_buff_pos));
#ifdef WORDS_BIGENDIAN
    p_msg->check_sum = zdtm_liltobigs(p_msg->check_sum);
#endif

    // Free the temp message buffer since I am done with it
    free((void *)buff);

    return 0;
}


/**
 * Prepare the raw message for sending based on the p_msg content.
 * @return RET_NNULL_RAW Failed, raw message not null.
 * @return RET_UNK_TYPE Failed, unknown message type. 
 */
int zdtm_prepare_message(zdtm_lib_env *cur_env, zdtm_msg *p_msg) {
    
    void *p_body;
    uint16_t *p_cont_size;

    // First we will calculate the body size -- all messages have
    // the message type.
    p_msg->body_size = MSG_TYPE_SIZE;

    if(memcmp(p_msg->body.type, RRL_MSG_TYPE, MSG_TYPE_SIZE) == 0) {
        p_msg->body_size += sizeof(uint8_t) + p_msg->body.cont.rrl.pw_size;

    } else if(memcmp(p_msg->body.type, RAY_MSG_TYPE, MSG_TYPE_SIZE) == 0 ||
              memcmp(p_msg->body.type, RIG_MSG_TYPE, MSG_TYPE_SIZE) == 0) {
        // No additional content 
        
    } else {
        // Unknown message type 
        return RET_UNK_TYPE;
    }

    // The cont_size is the body - the type size
    p_msg->cont_size = p_msg->body_size - MSG_TYPE_SIZE;

    // Set up the header
    memcpy(p_msg->header, DMSG_HDR, MSG_HDR_SIZE);
    // Write the content size;
    p_cont_size = (uint16_t *)(p_msg->header + MSG_HDR_CONT_OFFSET);

#ifdef WORDS_BIGENDIAN
    *p_cont_size = zdtm_liltobigs(p_msg->cont_size);
#else
    *p_cont_size = p_msg->cont_size;
#endif

    // Initialize the raw content.
    if(p_msg->body.p_raw_content != NULL) return RET_NNULL_RAW;

    // Allocate the raw message.
    p_body = p_msg->body.p_raw_content = malloc(p_msg->body_size);

    // Copy in the type
    memcpy(p_body, p_msg->body.type, MSG_TYPE_SIZE);
    p_body += 3;

    // Fill in the rest for non-trivial messages
    if(memcmp(p_msg->body.type, RRL_MSG_TYPE, MSG_TYPE_SIZE) == 0) {
        *((unsigned char*)p_body++) = p_msg->body.cont.rrl.pw_size;
        memcpy(p_body, p_msg->body.cont.rrl.pw, p_msg->body.cont.rrl.pw_size);
    }

    // Compute the checksum -- sill in host byte order
    p_msg->check_sum = zdtm_checksum(p_msg->body.p_raw_content, 
                                     p_msg->body_size);

    return 0; 
}

/*
int zdtm_send_message(zdtm_lib_env *cur_env, zdtm_msg *p_msg) {
    unsigned char *wire_msg;
    unsigned int msg_size;

    wire_msg = NULL;

    
    

    return 0;
}
*/
