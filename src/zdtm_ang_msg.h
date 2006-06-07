#ifndef ZDTM_ANG_MSG_H
#define ZDTM_ANG_MSG_H

#include "zdtm_common.h"

/**
 * Zaurus ANG message content.
 *
 * The zdtm_ang_msg_content is a structure which represents an ANG
 * Zaurus message content after being parsed from the raw message
 * content.
 */
struct zdtm_ang_msg_content {
    unsigned char uk_data_0;    // general unknown data
};
const char *ANG_MSG_TYPE = "ANG";
#define IS_ANG(x) (memcmp(x->body.type, ANG_MSG_TYPE, MSG_TYPE_SIZE) == 0)

int zdtm_parse_raw_ang_msg(void *buf, struct zdtm_ang_msg_content *ang);

#endif
