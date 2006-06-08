#ifndef _ZDTM_RIG_MSG_H_
#define _ZDTM_RIG_MSG_H_ 1

#include "zdtm_common.h"

/**
 * Desktop RIG message content.
 *
 * The zdtm_rig_msg_content represents an RIG Desktop to Zaurus message
 * to request an AIG information packet.
 */
struct zdtm_rig_msg_content {
};

extern const char *RIG_MSG_TYPE;
#define IS_RIG(x) (memcmp(x->body.type, RIG_MSG_TYPE, MSG_TYPE_SIZE) == 0)


#endif 
