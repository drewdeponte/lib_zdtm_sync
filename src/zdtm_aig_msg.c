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

#include "zdtm_aig_msg.h"

const char *AIG_MSG_TYPE = "AIG";

/**
 * Parse a raw AIG message.
 *
 * The zdtm_parse_raw_aig_msg function takes a raw AIG message and
 * parses it into it's appropriate components and fills in the aig
 * content fields so that the data can be easily obtained at a later
 * point in time.
 * @param buf Pointer to AIG message raw content.
 * @param aig Pointer to struct to store parsed AIG message content in.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully parsed the aig message.
 * @retval -1 Failed to allocate memory for the model string.
 */
int zdtm_parse_raw_aig_msg(void *buf, struct zdtm_aig_msg_content *aig) {
#ifdef WORDS_BIGENDIAN
    aig->model_str_len = zdtm_liltobigs(*((uint16_t *)buf));
#else
    aig->model_str_len = *((uint16_t *)buf);
#endif
    buf += sizeof(uint16_t);

    aig->model_str = (unsigned char *)malloc(aig->model_str_len);
    if (aig->model_str == NULL) {
        return -1;
    }
    memcpy((void *)aig->model_str, buf, aig->model_str_len);
    buf += aig->model_str_len;

    memcpy((void *)aig->uk_data_0, buf, 5);
    buf += 5;

    memcpy((void *)aig->language, buf, 2);
    buf += 2;

    aig->auth_state = *((unsigned char *)buf);
    buf += 1;

    memcpy((void *)aig->uk_data_1, buf, 6);

    return 0;
}
