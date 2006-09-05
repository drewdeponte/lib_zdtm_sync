#ifndef ZDTM_AMG_MSG_H
#define ZDTM_AMG_MSG_H

#include "zdtm_common.h"

#define AMG_TODO_MASK 0x01
#define AMG_CAL_MASK 0x02
#define AMG_ADDR_MASK 0x04

/**
 * Zaurus AMG message content.
 *
 * The zdtm_amg_msg_content is a structure which represents an AMG
 * Zaurus message content after being parsed from the raw message
 * content.
 */
struct zdtm_amg_msg_content {
    unsigned char empty_sync_type;
};
extern const char *AMG_MSG_TYPE;
#define IS_AMG(x) (memcmp(x->body.type, AMG_MSG_TYPE, MSG_TYPE_SIZE) == 0)

int zdtm_parse_raw_amg_msg(void *buf, struct zdtm_amg_msg_content *amg);

#endif
