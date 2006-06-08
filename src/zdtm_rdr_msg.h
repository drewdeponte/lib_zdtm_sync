#ifndef _ZDTM_RDR_MSG_H_
#define _ZDTM_RDR_MSG_H_ 1

#include "zdtm_common.h"

/**
 * Desktop RDR message content.
 *
 * The zdtm_rdr_msg_content represents an RDR Desktop to Zaurus message
 * solicits ADR messages, one message for each specific item.
 *      - sync_type
 *          - todo 0x06
 *          - calendar 0x01
 *          - address book 0x07
 *      - num_sync_ids Usually 1
 *      - sync_id
 */

struct zdtm_rdr_msg_content {
    unsigned char sync_type;
    uint16_t num_sync_ids;
    uint32_t sync_id;
};

extern const char *RDR_MSG_TYPE;
#define IS_RDR(x) (memcmp(x->body.type, RDR_MSG_TYPE, MSG_TYPE_SIZE) == 0)

inline int zdtm_rdr_length(struct zdtm_rdr_msg_content *rdr);
inline void *zdtm_rdr_write(void *buf, struct zdtm_rdr_msg_content *rdr);


#endif
