#ifndef _ZDTM_RRL_MSG_H_
#define _ZDTM_RRL_MSG_H_ 1

#include "zdtm_common.h"

/**
 * Desktop RRL message content.
 *
 * The zdtm_rrl_msg_content represents an RRL Desktop to Zaurus message
 * for authentication.
 */
struct zdtm_rrl_msg_content {
    unsigned char pw_size;
    char *pw;
};

extern const char *RRL_MSG_TYPE;
#define IS_RRL(x) (memcmp(x->body.type, RRL_MSG_TYPE, MSG_TYPE_SIZE) == 0)

inline int zdtm_rrl_length(struct zdtm_rrl_msg_content *rrl);
inline void *zdtm_rrl_write(void *buf, struct zdtm_rrl_msg_content *rrl);

#endif
