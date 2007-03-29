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
    struct sockaddr_in servaddr;
    int retval;
    int reuse_set_flag;

    reuse_set_flag = 1;

    cur_env->listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (cur_env->listenfd == INVALID_SOCKET) {
        perror("_zdtm_listen_for_zaurus - socket");
        return -1;
    }

    // Here I set a socket option so that if the applications ends
    // prematurely the socket is not blocked by the TIME_WAIT state.
#ifdef WIN32
    retval = setsockopt(cur_env->listenfd, SOL_SOCKET, SO_REUSEADDR,
        (const char *)&reuse_set_flag, (socklen_t)sizeof(reuse_set_flag));
#else
    retval = setsockopt(cur_env->listenfd, SOL_SOCKET, SO_REUSEADDR,
        (const void *)&reuse_set_flag, (socklen_t)sizeof(reuse_set_flag));
#endif
    if (retval == SOCKET_ERROR) {
        perror("_zdtm_listen_for_zaurus - setsockopt");
        return -2;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(DLISTPORT);

    retval = bind(cur_env->listenfd, (struct sockaddr *)&servaddr,
        (socklen_t)sizeof(servaddr));
    if (retval == SOCKET_ERROR) {
        perror("_zdtm_listen_for_zaurus - bind");
        return -3;
    }

    retval = listen(cur_env->listenfd, 1);
    if (retval == SOCKET_ERROR) {
        perror("_zdtm_listen_for_zaurus - listen");
        return -4;
    }

    // Return success
    return 0;
}

int _zdtm_stop_listening(zdtm_lib_env *cur_env) {
    int retval;

#ifdef WIN32
    retval = closesocket(cur_env->listenfd);
#else
    retval = close(cur_env->listenfd);
#endif
    if (retval == SOCKET_ERROR) {
        perror("_zdtm_stop_listening - close");
        return -1;
    }
    
    return 0;
}

int _zdtm_handle_zaurus_conn(zdtm_lib_env *cur_env) {
    struct sockaddr_in clntaddr;
    socklen_t len;
    /*
    char source_addr[16];
    char *retval;
    */

    memset(&clntaddr, 0, sizeof(clntaddr));
    len = sizeof(clntaddr);
    cur_env->connfd = accept(cur_env->listenfd,
        (struct sockaddr *)&clntaddr, &len);
    if (cur_env->connfd == INVALID_SOCKET) {
        perror("_zdtm_handle_zaurus_connection - accept");
        return -1;
    }

    /*
    retval = (char *)inet_ntop(AF_INET, &clntaddr.sin_addr,
        source_addr, 16);
    if (retval == NULL) {
        perror("_zdtm_handle_zaurus_connection - inet_ntop");
        return -2;
    }

    printf("_zdtm_handle_zaurus_connection: Received connection from \
        %s, port %d.\n", source_addr, ntohs(clntaddr.sin_port));
    */

    // Return in success.
    return 0;
}

int _zdtm_close_zaurus_conn(zdtm_lib_env *cur_env) {
    int retval;

#ifdef WIN32
    retval = closesocket(cur_env->connfd);
#else
    retval = close(cur_env->connfd);
#endif
    if (retval == SOCKET_ERROR) {
        perror("_zdtm_close_zaurus_conn - close");
        return -1;
    }
    
    return 0;
}

int _zdtm_conn_to_zaurus(zdtm_lib_env *cur_env, const char *zaurus_ip) {
    struct sockaddr_in servaddr;
    int retval;

    cur_env->reqfd = socket(AF_INET, SOCK_STREAM, 0);
    if (cur_env->reqfd == INVALID_SOCKET) {
        perror("_zdtm_conn_to_zaurus - socket");
        return -1;
    }

    memset(&servaddr, 0, sizeof(struct sockaddr_in));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(ZLISTPORT);
#ifdef WIN32
    servaddr.sin_addr.s_addr = inet_addr(zaurus_ip);
    if (servaddr.sin_addr.s_addr == INADDR_NONE) {
        return -2;
    }
#else
    if (inet_pton(AF_INET, zaurus_ip, &servaddr.sin_addr) <= 0) {
        perror("_zdtm_conn_to_zaurus - inet_pton");
        return -2;
    }
#endif

    retval = connect(cur_env->reqfd, (struct sockaddr *)&servaddr,
        sizeof(struct sockaddr));
    if(retval == SOCKET_ERROR) {
        perror("_zdtm_conn_to_zaurus - connect");
        return -3;
    }

    return 0;
}

int _zdtm_close_conn_to_zaurus(zdtm_lib_env *cur_env) {
    int retval;

#ifdef WIN32
    retval = closesocket(cur_env->reqfd);
#else
    retval = close(cur_env->reqfd);
#endif
    if (retval == SOCKET_ERROR) {
        perror("_zdtm_close_conn_to_zaurus - close");
        return -1;
    }

    return 0;
}

int _zdtm_send_comm_message_to(SOCKET sockfd, char *data) {
    int bytes_to_send;
    zdtm_ssize_t bytes_sent;
    zdtm_ssize_t tot_bytes_sent;

    bytes_to_send = COM_MSG_SIZE;

    tot_bytes_sent = 0;
    while (tot_bytes_sent < bytes_to_send) {
        bytes_sent = send(sockfd, (const zdtm_buf_t)data,
            (zdtm_size_t)(bytes_to_send - tot_bytes_sent), 0);
        if (bytes_sent < 0) {
            perror("_zdtm_send_comm_message_to - send");
            return -1;
        }

        tot_bytes_sent += bytes_sent;
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
    unsigned char header[MSG_HDR_SIZE + sizeof(uint16_t)];
    unsigned char *buff;
    unsigned char *cur_buff_pos;
    unsigned char *tmp_p;
    uint16_t body_size;
    uint16_t check_sum;
    zdtm_ssize_t bytes_to_read;
    zdtm_ssize_t tot_bytes_read;
    zdtm_ssize_t bytes_read;

    bytes_to_read = COM_MSG_SIZE;
    tot_bytes_read = 0;
    /* Attempt to read in a 7 byte common message. If the seven bytes
     * are a common message then return with a value representing which
     * common message was recveived. If the seven bytes read in are not
     * a common message then assume that it is a general message and
     * read the following 6 bytes of header in as well as the following
     * 2 bytes representing the body size. */
    while(tot_bytes_read < bytes_to_read) {
        bytes_read = recv(cur_env->connfd,
            (zdtm_buf_t)(header + tot_bytes_read),
            (zdtm_size_t)(bytes_to_read - tot_bytes_read), 0);
        if (bytes_read == 0) {
            if (tot_bytes_read == 0) {
                /* Socket was closed cleanly by opposite end. */
                return -1;
            } else {
                /* Socket was closed on opposite end in mid of msg. */
                return -2;
            }
        } else if (bytes_read == SOCKET_ERROR) {
            /* error */
            return -3;
        }
        tot_bytes_read += bytes_read;
    }

    /* If I made it this far then I know that 7 bytes have successfully
     * been read in and I want to compare the 7 bytes to known common
     * messages to see if it is one of the common messages or not. */
    if (_zdtm_is_ack_message(header)) {
        return 1;
    } else if (_zdtm_is_rqst_message(header)) {
        return 2;
    } else if (_zdtm_is_abrt_message(header)) {
        return 3;
    }

    bytes_to_read = MSG_HDR_SIZE + sizeof(uint16_t);
    /* If I made it this far then I know that it is none of the know
     * common messages. Hence, I assume that it is a general message and
     * read in the reast of the message header and the body size. */
    while(tot_bytes_read < bytes_to_read) {
        bytes_read = recv(cur_env->connfd,
            (zdtm_buf_t)(header + tot_bytes_read),
            (zdtm_size_t)(bytes_to_read - tot_bytes_read), 0);
        if (bytes_read == 0) {
            if (tot_bytes_read == 0) {
                /* Socket was closed cleanly by opposite end. */
                return -1;
            } else {
                /* Socket was closed on opposite end in mid of msg. */
                return -2;
            }
        } else if (bytes_read == SOCKET_ERROR) {
            /* error */
            return -3;
        }
        tot_bytes_read += bytes_read;
    }

    /* If I made it this far then I know that I have successfully read
     * in the number of bytes used in a general message header as well
     * as the following body size. */
    tmp_p = header + MSG_HDR_SIZE;
    body_size = *((uint16_t *)tmp_p);
#ifdef WORDS_BIGENDIAN
    body_size = zdtm_liltobigs(body_size);
#endif

    /* Now, I want to allocate memory necessary to temporarily store the
     * body of the general message. Read in the general messages body
     * and checksum. */
    bytes_to_read = body_size + sizeof(uint16_t);
    tot_bytes_read = 0;
    buff = (unsigned char *)malloc((zdtm_size_t)bytes_to_read);
    if (buff == NULL) {
        perror("_zdtm_recv_message - malloc");
        return -4;
    }
    
    while(tot_bytes_read < bytes_to_read) {
        bytes_read = recv(cur_env->connfd,
            (zdtm_buf_t)(buff + tot_bytes_read),
            (zdtm_size_t)(bytes_to_read - tot_bytes_read), 0);
        if (bytes_read == 0) {
            if (tot_bytes_read == 0) {
                /* Socket was closed cleanly by opposite end. */
                free((void *)buff);
                return -1;
            } else {
                /* Socket was closed on opposite end in mid of msg. */
                free((void *)buff);
                return -2;
            }
        } else if (bytes_read == SOCKET_ERROR) {
            /* error */
            free((void *)buff);
            return -3;
        }
        tot_bytes_read += bytes_read;
    }

    /* If I made it this far I know that I have successfully read in the
     * body size number of bytes as well as the following 2 bytes
     * representing the checksum. */
    tmp_p = buff + body_size;
    check_sum = *((uint16_t *)tmp_p);
#ifdef WORDS_BIGENDIAN
    check_sum = zdtm_liltobigs(check_sum);
#endif

    /*
     * Since it made it this far I know that it is an acceptable message
     * based on size alone. Hence, I am going to need somewhere to put
     * the data.
     */
    if (p_msg == NULL) {
        free((void *)buff);
        return -5;
    }

    /*
     * Check if the body raw content is initialized to NULL so that
     * in-case of failure at some point so the user can determine if
     * they need to free the content or not.
     */
    if (p_msg->body.p_raw_content != NULL) {
        free((void *)buff);
        return -6;
    }

    /*
     * Now that I know the size is acceptable I am going to parse the
     * data into the proper pieces to fill the zdtm_message structure so
     * that the information may be easily obtained later on. Note, I am
     * only parsing into to the raw portions. Later parsing will occur
     * in a seperate function for each specific type of message.
     */

    // Set the zdtm_message header
    memcpy((void *)p_msg->header, header, MSG_HDR_SIZE);

    // Set the zdtm_message body size
    p_msg->body_size = body_size;

    cur_buff_pos = buff;
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
    p_msg->check_sum = check_sum;

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
    int retval;
    zdtm_ssize_t bytes_written;
    zdtm_ssize_t tot_bytes_written;
    zdtm_ssize_t bytes_to_write;

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

    bytes_to_write = msg_size;
    tot_bytes_written = 0;
    while (tot_bytes_written < bytes_to_write) {
        bytes_written = send(sockfd, (const zdtm_buf_t)p_wire_msg,
            (zdtm_size_t)(bytes_to_write - tot_bytes_written), 0);
        if (bytes_written < 0) {
            perror("_zdtm_send_message_to - send");
            _zdtm_clean_message(p_msg);
            free(p_wire_msg);
            return -2;
        }

        tot_bytes_written += bytes_written;
    }

    _zdtm_clean_message(p_msg);
    free(p_wire_msg);

    return 0;
}

int _zdtm_wrapped_send_message(zdtm_lib_env *cur_env, zdtm_msg *msg) {
    int r;
    zdtm_msg rmsg;

    /* recv rqst message */
    memset(&rmsg, 0, sizeof(zdtm_msg));
    r = _zdtm_recv_message(cur_env, &rmsg);
    if (r != 2) {
        _zdtm_clean_message(&rmsg); return -1; }

    /* send general message */
    r = _zdtm_send_message(cur_env, msg);
    if (r != 0) {
        _zdtm_clean_message(&rmsg); return -2; }

    /* recv ack message */
    memset(&rmsg, 0, sizeof(zdtm_msg));
    r = _zdtm_recv_message(cur_env, &rmsg);
    if (r != 1) {
        _zdtm_clean_message(&rmsg); return -3; }

    return 0;
}

int _zdtm_wrapped_recv_message(zdtm_lib_env *cur_env, zdtm_msg *msg) {
    int r;

    /* send rqst message */
    r = _zdtm_send_rqst_message(cur_env);
    if (r != 0) {
        _zdtm_log_error(cur_env, "_zdtm_send_rqst_message", r);
        return -1;
    }

    /* recv general message */
    r = _zdtm_recv_message(cur_env, msg);
    if (r < 0) {
        _zdtm_log_error(cur_env, "_zdtm_recv_message", r);
        return -2;
    } else if (r > 0) {
        return r;
    }

    /* send ack message */
    r = _zdtm_send_ack_message(cur_env);
    if (r != 0) {
        _zdtm_log_error(cur_env, "_zdtm_send_ack_message", r);
        return -3;
    }

    return 0;
}
