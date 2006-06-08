#ifndef _ZDTM_RAY_MSG_H_
#define _ZDTM_RAY_MSG_H_ 1

#include "zdtm_common.h"

/**
 * Desktop RAY message content.
 *
 * The zdtm_ray_msg_content represents an RAY Desktop to Zaurus message
 * in response to the AAY message.  It contains no specific content.
 */
struct zdtm_ray_msg_content {

};

extern const char *RAY_MSG_TYPE;
#define IS_RAY(x) (memcmp(x->body.type, RAY_MSG_TYPE, MSG_TYPE_SIZE) == 0)

#endif
