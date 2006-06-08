#ifndef _ZDTM_RGE_MSG_H_
#define _ZDTM_RGE_MSG_H_

#include "zdtm_common.h"

/**
 * Desktop RGE message content.
 *
 * The zdtm_rge_msg_content represents an RGE Desktop to Zaurus message
 * requests the contents of a DTM index file or a DTM box file.
 *
 */

struct zdtm_rge_msg_content {
    uint16_t path_len;
    char *path;
};

extern const char *RGE_MSG_TYPE;
#define IS_RGE(x) (memcmp(x->body.type, RGE_MSG_TYPE, MSG_TYPE_SIZE) == 0)

inline int zdtm_rge_length(struct zdtm_rge_msg_content *rge);
inline void *zdtm_rge_write(void *buf, struct zdtm_rge_msg_content *rge);

#endif
