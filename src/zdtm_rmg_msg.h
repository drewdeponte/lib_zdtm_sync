#ifndef _ZDTM_RMG_MSG_H_
#define _ZDTM_RMG_MSG_H_ 1

#include "zdtm_common.h"

/**
 * Desktop RMG message content.
 *
 * The zdtm_rmg_msg_content represents an RMG Desktop to Zaurus message
 * to indicate the synchronization type.
 *      - uk is typically 0x01
 *      - sync_type
 *          - todo 0x06
 *          - calendar 0x01
 *          - address book 0x07
 * Synchronization log.
 */

struct zdtm_rmg_msg_content {
    unsigned char uk;
    unsigned char sync_type;
};

extern const char *RMG_MSG_TYPE;
#define IS_RMG(x) (memcmp(x->body.type, RMG_MSG_TYPE, MSG_TYPE_SIZE) == 0)

inline int zdtm_rmg_length(struct zdtm_rmg_msg_content *rmg);
inline void *zdtm_rmg_write(void *buf, struct zdtm_rmg_msg_content *rmg);


#endif 
