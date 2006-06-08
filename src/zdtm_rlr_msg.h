#ifndef _ZDTM_RLR_MSG_H_
#define _ZDTM_RLR_MSG_H_ 1

#include "zdtm_common.h"


/**
 * Desktop RLR message content.
 *
 * The zdtm_rlr_msg_content represents an RLR Desktop to Zaurus message
 * requests all synchronization ids of a given type.
 *
 */

struct zdtm_rlr_msg_content {
    unsigned char sync_type;
};

extern const char *RLR_MSG_TYPE;
#define IS_RLR(x) (memcmp(x->body.type, RLR_MSG_TYPE, MSG_TYPE_SIZE) == 0)

inline int zdtm_rlr_length(struct zdtm_rlr_msg_content *rlr);
inline void *zdtm_rlr_write(void *buf, struct zdtm_rlr_msg_content *rlr);


#endif 
