#ifndef _ZDTM_RDI_MSG_H_
#define _ZDTM_RDI_MSG_H_ 1

#include "zdtm_common.h"
/**
 * Desktop RDI message content.
 *
 * The zdtm_rdi_msg_content represents an RDI Desktop to Zaurus message
 * solicits ADI message.
 *      - sync_type
 *          - todo 0x06
 *          - calendar 0x01
 *          - address book 0x07
 *      - uk is typically 0x07
 */

struct zdtm_rdi_msg_content {
    unsigned char sync_type;
    unsigned char uk;
};

extern const char *RDI_MSG_TYPE;
#define IS_RDI(x) (memcmp(x->body.type, RDI_MSG_TYPE, MSG_TYPE_SIZE) == 0)

inline int zdtm_rdi_length(struct zdtm_rdi_msg_content *rdi);
inline void *zdtm_rdi_write(void *buf, struct zdtm_rdi_msg_content *rdi);


#endif
