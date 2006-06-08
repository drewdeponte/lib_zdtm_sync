#ifndef _ZDTM_RQT_MSG_H_
#define _ZDTM_RQT_MSG_H_ 1

#include "zdtm_common.h"

/**
 * Desktop RQT message content.
 *
 * The zdtm_rqt_msg_content represents an RQT Desktop to Zaurus message
 * terminates the zaurus connection.
 */

struct zdtm_rqt_msg_content {
    char null_bytes[3];
};

extern const char *RQT_MSG_TYPE;
#define IS_RQT(x) (memcmp(x->body.type, RQT_MSG_TYPE, MSG_TYPE_SIZE) == 0)


inline int zdtm_rqt_length(struct zdtm_rqt_msg_content *rqt);
inline void *zdtm_rqt_write(void *buf, struct zdtm_rqt_msg_content *rqt);


#endif
