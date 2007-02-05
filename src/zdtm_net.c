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
 * @file zdtm_net.c
 * @brief This is an implementation file for private network functions.
 *
 * The zdtm_net.c file is an implementation file for all the private
 * network handling functions that lib_zdtm_sync uses.
 */

#include "zdtm_net.h"

int _zdtm_listen_for_zaurus(zdtm_lib_env *cur_env) {
    int retval;
    struct sockaddr_in servaddr;
    int reuse_set_flag = 1;

    cur_env->listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (cur_env->listenfd == -1) {
        perror("_zdtm_listen_for_zaurus - socket");
        return -1;
    }

    // Here I set a socket option so that if the applications ends
    // prematurely the socket is not blocked by the TIME_WAIT state.
    retval = setsockopt(cur_env->listenfd, SOL_SOCKET, SO_REUSEADDR,
        (void *)&reuse_set_flag, sizeof(reuse_set_flag));
    if (retval == -1) {
        perror("_zdtm_listen_for_zaurus - setsockopt");
        return -2;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(DLISTPORT);

    retval = bind(cur_env->listenfd, (struct sockaddr *)&servaddr,
        sizeof(servaddr));
    if (retval == -1) {
        perror("_zdtm_listen_for_zaurus - bind");
        return -3;
    }

    retval = listen(cur_env->listenfd, 1);
    if (retval == -1) {
        perror("_zdtm_listen_for_zaurus - listen");
        return -4;
    }

    // Return success
    return 0;
}

int _zdtm_handle_zaurus_conn(zdtm_lib_env *cur_env) {
    struct sockaddr_in clntaddr;
    char source_addr[16];
    socklen_t len;
    char *retval;

    memset(&clntaddr, 0, sizeof(clntaddr));
    len = sizeof(clntaddr);
    cur_env->connfd = accept(cur_env->listenfd,
        (struct sockaddr *)&clntaddr, &len);
    if (cur_env->connfd == -1) {
        perror("_zdtm_handle_zaurus_connection - accept");
        return -1;
    }

    retval = (char *)inet_ntop(AF_INET, &clntaddr.sin_addr,
        source_addr, 16);
    if (retval == NULL) {
        perror("_zdtm_handle_zaurus_connection - inet_ntop");
        return -2;
    }

    /*
    printf("_zdtm_handle_zaurus_connection: Received connection from \
        %s, port %d.\n", source_addr, ntohs(clntaddr.sin_port));
    */

    // Return in success.
    return 0;
}

int _zdtm_close_zaurus_conn(zdtm_lib_env *cur_env) {
    int retval;

    retval = close(cur_env->connfd);
    if (retval) {
        perror("_zdtm_close_zaurus_conn - close");
        return -1;
    }
    
    return 0;
}

int _zdtm_conn_to_zaurus(zdtm_lib_env *cur_env, const char *zaurus_ip) {
    struct sockaddr_in servaddr;
    int retval;

    cur_env->reqfd = socket(AF_INET, SOCK_STREAM, 0);
    if (cur_env->reqfd == -1) {
        perror("_zdtm_conn_to_zaurus - socket");
        return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(ZLISTPORT);
    if (inet_pton(AF_INET, zaurus_ip, &servaddr.sin_addr) <= 0) {
        perror("_zdtm_conn_to_zaurus - inet_pton");
        return -2;
    }

    retval = connect(cur_env->reqfd, (struct sockaddr *)&servaddr,
        sizeof(servaddr));
    if(retval == -1) {
        perror("_zdtm_conn_to_zaurus - connect");
        return -3;
    }

    return 0;
}

int _zdtm_close_conn_to_zaurus(zdtm_lib_env *cur_env) {
    int retval;

    retval = close(cur_env->reqfd);
    if (retval) {
        perror("_zdtm_close_conn_to_zaurus - close");
        return -1;
    }

    return 0;
}

int _zdtm_send_comm_message_to(int sockfd, char *data) {
    int bytes_written;
    
    bytes_written = write(sockfd, data, COM_MSG_SIZE);
    if (bytes_written == -1) {
        perror("zdtm_send_message - write");
        return -1;
    } else if (bytes_written == 0) {
        return -2;
    } else if (bytes_written < COM_MSG_SIZE) {
        return -3;
    }

    return 0;
}

int _zdtm_send_ack_message(zdtm_lib_env *cur_env) {
    int retval;
    char msg_data[COM_MSG_SIZE] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x96,
        0x06};

    retval = _zdtm_send_comm_message_to(cur_env->connfd, msg_data);
    return retval;
}

int _zdtm_send_rqst_message(zdtm_lib_env *cur_env) {
    int retval;
    char msg_data[COM_MSG_SIZE] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x96,
        0x05};

    retval = _zdtm_send_comm_message_to(cur_env->connfd, msg_data);
    return retval;
}

int _zdtm_send_abrt_message(zdtm_lib_env *cur_env) {
    int retval;
    char msg_data[COM_MSG_SIZE] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x96,
        0x18};

    retval = _zdtm_send_comm_message_to(cur_env->connfd, msg_data);
    return retval;
}

int _zdtm_recv_message(zdtm_lib_env *cur_env, zdtm_msg *p_msg) {
    ssize_t bytes_read;
    uint32_t max_buff_size;
    unsigned char *buff;
    unsigned char *cur_buff_pos;

    cur_buff_pos = NULL;

    max_buff_size = MSG_HDR_SIZE + sizeof(uint16_t) + UINT16_MAX + \
        sizeof(uint16_t);
   
    buff = (unsigned char *)malloc((size_t)max_buff_size);
    if (buff == NULL) {
        perror("_zdtm_recv_message - malloc");
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
        perror("_zdtm_recv_message - read");
        return -3;
    }

    // Check for common messages and unknown messages based on size
    if (bytes_read < 20) {
        if (bytes_read == COM_MSG_SIZE) {
            if (_zdtm_is_ack_message(buff)) {
                free((void *)buff);
                return 1;
            } else if (_zdtm_is_rqst_message(buff)) {
                free((void *)buff);
                return 2;
            } else if (_zdtm_is_abrt_message(buff)) {
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
    if (p_msg->cont_size > 0) {
        p_msg->body.p_raw_content = malloc((size_t)p_msg->cont_size);
        if (p_msg->body.p_raw_content == NULL) {
            free((void *)buff);
            // return signifying that malloc failed
            perror("_zdtm_recv_message - malloc");
            return RET_MEM_CONTENT;
        }
        memcpy(p_msg->body.p_raw_content, (const void *)cur_buff_pos,
            p_msg->cont_size);
        cur_buff_pos = cur_buff_pos + p_msg->cont_size;
    }

    // Set the zdtm_message check_sum
    p_msg->check_sum = *((uint16_t *)(cur_buff_pos));
#ifdef WORDS_BIGENDIAN
    p_msg->check_sum = zdtm_liltobigs(p_msg->check_sum);
#endif

    // Free the temp message buffer since I am done with it
    free((void *)buff);

    /*
     * At this point everything in the zdtm_message struct is filled in
     * except for the p_msg->body.cont.
     */

    if (_zdtm_parse_raw_msg(p_msg) != 0) {
        return RET_PARSE_RAW_FAIL;
    }
    
    _zdtm_dump_msg_log(cur_env, p_msg);
    
    return 0;
}

int _zdtm_send_message(zdtm_lib_env *cur_env, zdtm_msg *p_msg) {
    int retval;

    _zdtm_dump_msg_log(cur_env, p_msg);

    retval = _zdtm_send_message_to(cur_env, p_msg, cur_env->connfd);
    return retval;
}

int _zdtm_send_message_to(zdtm_lib_env *cur_env, zdtm_msg *p_msg, int sockfd) {
    void *p_wire_msg;
    void *p_cur_pos;
    unsigned int msg_size;
    int bytes_written, retval;

    p_wire_msg = NULL;

    retval = _zdtm_prepare_message(cur_env, p_msg);
    if (retval != 0) {
        _zdtm_clean_message(p_msg);
        return -1;
    }
    
    msg_size = MSG_HDR_SIZE + sizeof(uint16_t) +p_msg->body_size + \
               sizeof(uint16_t);

    p_wire_msg = malloc((size_t)msg_size);
    if (p_wire_msg == NULL) {
        _zdtm_clean_message(p_msg);
        return RET_MALLOC_FAIL;
    }

    p_cur_pos = p_wire_msg;

    // Copy the header into the raw message
    memcpy(p_cur_pos, (void *)p_msg->header, (size_t)MSG_HDR_SIZE);
    p_cur_pos = p_cur_pos + MSG_HDR_SIZE;
    
    // Copy the body size into the raw message
    memcpy(p_cur_pos, (void *)&p_msg->body_size, sizeof(uint16_t));
    p_cur_pos = p_cur_pos + sizeof(uint16_t);

    // Copy the message type into the raw message
    memcpy(p_cur_pos, (void *)p_msg->body.type, (size_t)MSG_TYPE_SIZE);
    p_cur_pos = p_cur_pos + MSG_TYPE_SIZE;

    // Copy the message content into the raw message.
    memcpy(p_cur_pos, p_msg->body.p_raw_content, (size_t)p_msg->cont_size);
    p_cur_pos = p_cur_pos + p_msg->cont_size;

    // Copy the message check sum into the raw message.
    memcpy(p_cur_pos, (void *)&p_msg->check_sum, sizeof(uint16_t));
    p_cur_pos = p_cur_pos + sizeof(uint16_t);

    // At this point the wire message should be properly built.

    bytes_written = write(sockfd, p_wire_msg, msg_size);
    if (bytes_written == -1) {
        perror("_zdtm_send_message_to - write");
        _zdtm_clean_message(p_msg);
        free(p_wire_msg);
        return -2;
    } else if (bytes_written == 0) {
        _zdtm_clean_message(p_msg);
        free(p_wire_msg);
        return -3;
    } else if (bytes_written < msg_size) {
        _zdtm_clean_message(p_msg);
        free(p_wire_msg);
        return -4;
    }
    
    _zdtm_clean_message(p_msg);
    free(p_wire_msg);

    return 0;
}
