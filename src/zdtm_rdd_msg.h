#ifndef _ZDTM_RDD_MSG_H_
#define _ZDTM_RDD_MSG_H_ 1

#include "zdtm_common.h"


/**
 * Desktop RDD message content.
 *
 * The zdtm_rdd_msg_content represents an RDD Desktop to Zaurus message
 * indicates that an item is to be deleted during synchronizaion.
 *
 *      - num_sync_ids is always 1, like a happy vestigial organ.
 */

struct zdtm_rdd_msg_content {
    unsigned char sync_type;
    uint16_t num_sync_ids;
    uint32_t sync_id;
};

extern const char *RDD_MSG_TYPE;
#define IS_RDD(x) (memcmp(x->body.type, RDD_MSG_TYPE, MSG_TYPE_SIZE) == 0)

inline int zdtm_rdd_length(struct zdtm_rdd_msg_content *rdd);
inline void *zdtm_rdd_write(void *buf, struct zdtm_rdd_msg_content *rdd);


#endif
