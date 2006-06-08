#ifndef _ZDTM_RSY_MSG_H_
#define _ZDTM_RSY_MSG_H_ 1

#include "zdtm_common.h"

/**
 * Desktop RSY message content.
 *
 * The zdtm_rsy_msg_content represents an RSY Desktop to Zaurus message
 * solicits ASY message.
 *      - sync_type
 *          - todo 0x06
 *          - calendar 0x01
 *          - address book 0x07
 *      - uk is typically 0x07
 */

struct zdtm_rsy_msg_content {
    unsigned char sync_type;
    unsigned char uk;
};

extern const char *RSY_MSG_TYPE;
#define IS_RSY(x) (memcmp(x->body.type, RSY_MSG_TYPE, MSG_TYPE_SIZE) == 0)

inline int zdtm_rsy_length(struct zdtm_rsy_msg_content *rsy);
inline void *zdtm_rsy_write(void *buf, struct zdtm_rsy_msg_content *rsy);


#endif
