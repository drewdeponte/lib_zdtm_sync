#ifndef _ZDTM_RTS_MSG_H_
#define _ZDTM_RTS_MSG_H_ 1

#include "zdtm_common.h"

/**
 * Desktop RTS message content.
 *
 * The zdtm_rts_msg_content represents an RTS Desktop to Zaurus message
 * notify Zaurus of the time.
 *
 * String of YYYYMMDDhhmmss
 */

#define RTS_DATE_LEN 14

struct zdtm_rts_msg_content {
    char date[RTS_DATE_LEN];
};

extern const char *RTS_MSG_TYPE;
#define IS_RTS(x) (memcmp(x->body.type, RTS_MSG_TYPE, MSG_TYPE_SIZE) == 0)

inline int zdtm_rts_length(struct zdtm_rts_msg_content *rts);
inline void *zdtm_rts_write(void *buf, struct zdtm_rts_msg_content *rts);


#endif
