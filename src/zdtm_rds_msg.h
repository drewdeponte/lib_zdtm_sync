#ifndef _ZDTM_RDS_MSG_H_
#define _ZDTM_RDS_MSG_H_ 1

#include "zdtm_common.h"

/**
 * Desktop RDS message content.
 *
 * The zdtm_rds_msg_content represents an RDS Desktop to Zaurus message
 * indicates that synchronization was successful. 
 *
 * status is always 0x7.
 */

struct zdtm_rds_msg_content {
    unsigned char sync_type;
    char status;
    char null_bytes[2];
};

extern const char *RDS_MSG_TYPE;
#define IS_RDS(x) (memcmp(x->body.type, RDS_MSG_TYPE, MSG_TYPE_SIZE) == 0)

inline int zdtm_rds_length(struct zdtm_rds_msg_content *rds);
inline void *zdtm_rds_write(void *buf, struct zdtm_rds_msg_content *rds);


#endif
