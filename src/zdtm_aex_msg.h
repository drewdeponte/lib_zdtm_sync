#ifndef ZDTM_AEX_MSG_H
#define ZDTM_AEX_MSG_H

#include "zdtm_common.h"

/**
 * Zaurus AEX message content.
 *
 * The zdtm_aex_msg_content is a structure which represents an AEX
 * Zaurus message content after being parsed from the raw message
 * content.
 */
struct zdtm_aex_msg_content {
};
const char *AEX_MSG_TYPE = "AEX";
#define IS_AEX(x) (memcmp(x->body.type, AEX_MSG_TYPE, MSG_TYPE_SIZE) == 0)

#endif
