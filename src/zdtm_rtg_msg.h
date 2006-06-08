#ifndef _ZDTM_RTG_MSG_H_
#define _ZDTM_RTG_MSG_H_ 1

#include "zdtm_common.h"

/**
 * Desktop RTG message content.
 *
 * The zdtm_rtg_msg_content represents an RTG Desktop to Zaurus message
 * Request for time stamp.
 */

struct zdtm_rtg_msg_content {
};

extern const char *RTG_MSG_TYPE;
#define IS_RTG(x) (memcmp(x->body.type, RTG_MSG_TYPE, MSG_TYPE_SIZE) == 0)

#endif
