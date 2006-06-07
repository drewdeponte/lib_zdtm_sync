#ifndef ZDTM_ATG_MSG_H
#define ZDTM_ATG_MSG_H

#include "zdtm_common.h"

/**
 * Zaurus ATG message content.
 *
 * The zdtm_atg_msg_content is a structure which represents an ATG
 * Zaurus message content after being parsed from the raw message
 * content.
 */
struct zdtm_atg_msg_content {
    unsigned char year[4];
    unsigned char month[2];
    unsigned char day[2];
    unsigned char hour[2];
    unsigned char minutes[2];
    unsigned char seconds[2];
};  
const char *ATG_MSG_TYPE = "ATG";
#define IS_ATG(x) (memcmp(x->body.type, ATG_MSG_TYPE, MSG_TYPE_SIZE) == 0)

int zdtm_parse_raw_atg_msg(void *buf, struct zdtm_atg_msg_content *atg);

#endif
