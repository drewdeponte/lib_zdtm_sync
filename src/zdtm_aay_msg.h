#ifndef ZDTM_AAY_MSG_H
#define ZDTM_AAY_MSG_H

#include "zdtm_common.h"

/**
 * Zaurus AAY message content.
 *
 * The zdtm_aay_msg_content is a structure which represents an AAY
 * Zaurus message content after being parsed from the raw message
 * content.
 */
struct zdtm_aay_msg_content {
    unsigned char uk_data_0[3]; // general unknown data
};
extern const char *AAY_MSG_TYPE;
#define IS_AAY(x) (memcmp(x->body.type, AAY_MSG_TYPE, MSG_TYPE_SIZE) == 0)

int zdtm_parse_raw_aay_msg(void *buf, struct zdtm_aay_msg_content *aay);

#endif
