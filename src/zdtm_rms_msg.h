#ifndef _ZDTM_RMS_MSG_H_
#define _ZDTM_RMS_MSG_H_ 1

#include "zdtm_common.h"

/**
 * Desktop RMS message content.
 *
 * The zdtm_rms_msg_content represents an RMS Desktop to Zaurus message
 * in the process of doing a full synchronization.
 * Not really implemented at the moment.
 * First two bytes are size, the remaining 38 are log message.
 * The message is padded with 0x00 if it is not the full size.
 */

#define RMS_LOG_SIZE 38
 
struct zdtm_rms_msg_content {
    uint16_t log_size;
    unsigned char log[RMS_LOG_SIZE];
};

extern const char *RMS_MSG_TYPE;
#define IS_RMS(x) (memcmp(x->body.type, RMS_MSG_TYPE, MSG_TYPE_SIZE) == 0)

inline int zdtm_rms_length(struct zdtm_rms_msg_content *rms);
inline void *zdtm_rms_write(void *buf, struct zdtm_rms_msg_content *rms);


#endif
