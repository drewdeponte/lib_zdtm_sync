#ifndef ZDTM_AMG_MSG_H
#define ZDTM_AMG_MSG_H

#include "zdtm_common.h"

/**
 * Zaurus AMG message content.
 *
 * The zdtm_amg_msg_content is a structure which represents an AMG
 * Zaurus message content after being parsed from the raw message
 * content.
 */
struct zdtm_amg_msg_content {
};
const char *AMG_MSG_TYPE = "AMG";
#define IS_AMG(x) (memcmp(x->body.type, AMG_MSG_TYPE, MSG_TYPE_SIZE) == 0)

#endif
