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
 * @file zdtm_log.c
 * @brief This is an implementation file for private log functions.
 *
 * The zdtm_log.c file is an implementation file that defines all the
 * private log handling functions used by lib_zdtm_sync.
 */

#include "zdtm_log.h"

int _zdtm_open_log(zdtm_lib_env *cur_env) {
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

    cur_env->logfp = fopen(file_path, "w");
    if (cur_env->logfp == NULL) {
        perror("_zdtm_open_log - fopen");
        return -3;
    }

    return 0;
}

int _zdtm_write_log(zdtm_lib_env *cur_env, const char *buff,
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
        perror("_zdtm_write_log - fflush");
        return -3;
    }
   
    return 0;
}


int _zdtm_log_error(zdtm_lib_env *cur_env, const char *func_name, int err) {
    char buff[256];
    int buff_size;
    int retval;

    buff_size = 256;

    retval = snprintf(buff, buff_size, "Error: %s - %d", func_name, err);
    if (retval == -1) {
        return -1;
    } else if (retval == buff_size) {
        return -2;
    }
    retval = _zdtm_write_log(cur_env, buff, retval);
    if (retval != 0) {
        return -3;
    }

    return 0;
}

int _zdtm_close_log(zdtm_lib_env *cur_env) {
    int retval;

    if (cur_env->logfp != NULL) {
        retval = fclose(cur_env->logfp);
        if (retval != 0) {
            perror("_zdtm_close_log - fclose");
            return -1;
        }
        return 0;
    }

    return -2;
}

int _zdtm_dump_msg_log(zdtm_lib_env *cur_env, zdtm_msg *p_msg) {
    int i, j;       /* loop counters */
    char buff[256]; /* temp buffer to hold output data */
    int buff_size;  /* the size of buff in bytes */
    int buff_bytes; /* bytes of buff used */
    int retval;     /* retval temporary holder */

    buff_size = 256;
    buff_bytes = 0;

    retval = snprintf(buff, buff_size, "== Message Dump Start ==\n");
    if (retval == -1) {
        return -1;
    } else if (retval == buff_size) {
        return -2;
    }
    retval = _zdtm_write_log(cur_env, buff, retval);
    if (retval != 0) {
        return -3;
    }

    /* Dump the header. */
    retval = snprintf(buff, buff_size, "Header: ");
    if (retval == -1) {
        return -1;
    } else if (retval == buff_size) {
        return -2;
    }
    buff_bytes = buff_bytes + retval;

    for (i = 0; i < MSG_HDR_SIZE; ++i) {
        retval = snprintf(buff + buff_bytes, buff_size - buff_bytes, "0x%.2x ",
            (unsigned char)p_msg->header[i]);
        if (retval == -1) {
            return -1;
        } else if (retval == buff_size) {
            return -2;
        }
        buff_bytes = buff_bytes + retval;
    }

    retval = snprintf(buff + buff_bytes, buff_size - buff_bytes, "\n");
    if (retval == -1) {
        return -1;
    } else if (retval == buff_size) {
        return -2;
    }
    buff_bytes = buff_bytes + retval;
    retval = _zdtm_write_log(cur_env, buff, buff_bytes);
    if (retval != 0) {
        return -3;
    }
    buff_bytes = 0;

    /* Dump the message type in ASCII and Hex. */
    retval = snprintf(buff, buff_size,
        "Type \t\t(ASCII): %c%c%c \t\t(Hex): 0x%x 0x%x 0x%x\n",
        p_msg->body.type[0], p_msg->body.type[1], p_msg->body.type[2],
        p_msg->body.type[0], p_msg->body.type[1], p_msg->body.type[2]);
    if (retval == -1) {
        return -1;
    } else if (retval == buff_size) {
        return -2;
    }
    retval = _zdtm_write_log(cur_env, buff, retval);
    if (retval != 0) {
        return -3;
    }
 
    /* Dump the raw message content in hex. */
    retval = snprintf(buff, buff_size, "Content (Hex):\n"); 
    if (retval == -1) {
        return -1;
    } else if (retval == buff_size) {
        return -2;
    }
    retval = _zdtm_write_log(cur_env, buff, retval);
    if (retval != 0) {
        return -3;
    }

    i = 0;
    while (i < p_msg->cont_size) {
        if ((p_msg->cont_size - i) >= 15) {
            retval = snprintf(buff + buff_bytes, buff_size - buff_bytes,
                "%.3d: ", i);
            if(retval == -1) {
                return -1;
            } else if (retval == buff_size) {
                return -2;
            }
            buff_bytes = buff_bytes + retval;
            for (j = i; j < (i + 15); j++) {
                /* print each byte */
                retval = snprintf(buff + buff_bytes, buff_size - buff_bytes,
                    "0x%.2x ",
                    ((unsigned char *)(p_msg->body.p_raw_content))[j]);
                if (retval == -1) {
                    return -1;
                } else if (retval == buff_size) {
                    buff[buff_size - 1] = '\0';
                    return -2;
                }
                buff_bytes = buff_bytes + retval;
            }

            retval = snprintf(buff + buff_bytes, buff_size - buff_bytes, "\n");
            if (retval == -1) {
                return -1;
            } else if (retval == buff_size) {
                return -2;
            }
            buff_bytes = buff_bytes + retval;
            i = j;
        } else {
            retval = snprintf(buff + buff_bytes, buff_size - buff_bytes,
                "%.3d: ", i);
            if(retval == -1) {
                return -1;
            } else if (retval == buff_size) {
                return -2;
            }
            buff_bytes = buff_bytes + retval;
            for (j = i; j < p_msg->cont_size; j++) {
                /* print each byte */
                retval = snprintf(buff + buff_bytes, buff_size - buff_bytes,
                    "0x%.2x ",
                    ((unsigned char *)(p_msg->body.p_raw_content))[j]);
                if (retval == -1) {
                    return -1;
                } else if (retval == buff_size) {
                    buff[buff_size - 1] = '\0';
                    return -2;
                }
                buff_bytes = buff_bytes + retval;
            }
            
            retval = snprintf(buff + buff_bytes, buff_size - buff_bytes, "\n");
            if (retval == -1) {
                return -1;
            } else if (retval == buff_size) {
                return -2;
            }
            buff_bytes = buff_bytes + retval;
            i = j;
        }

        retval = _zdtm_write_log(cur_env, buff, buff_bytes);
        if (retval != 0) {
            return -3;
        }
        buff_bytes = 0;
    }   

    retval = snprintf(buff, buff_size,
        "Content Size \t(Base 10): %.5u \t(Hex): 0x%.4x\n",
        p_msg->cont_size, p_msg->cont_size);
    if (retval == -1) {
        return -1;
    } else if (retval == buff_size) {
        return -2;
    }
    buff_bytes = buff_bytes + retval;
    retval = snprintf(buff + buff_bytes, buff_size - buff_bytes,
        "Checksum \t(Base 10): %.5u \t(Hex): 0x%.4x\n",
        p_msg->check_sum, p_msg->check_sum);
    if (retval == -1) {
        return -1;
    } else if (retval == buff_size) {
        return -2;
    }
    buff_bytes = buff_bytes + retval;
    retval = snprintf(buff + buff_bytes, buff_size - buff_bytes,
        "Body Size \t(Base 10): %.5u \t(Hex): 0x%.4x\n\n",
        p_msg->body_size, p_msg->body_size);
    if (retval == -1) {
        return -1;
    } else if (retval == buff_size) {
        return -2;
    }
    buff_bytes = buff_bytes + retval;
    retval = _zdtm_write_log(cur_env, buff, buff_bytes);
    if (retval != 0) {
        return -3;
    }

    return 0;
}
