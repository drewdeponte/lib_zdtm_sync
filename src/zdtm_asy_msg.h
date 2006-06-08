#ifndef ZDTM_ASY_MSG_H
#define ZDTM_ASY_MSG_H

#include "zdtm_common.h"

/**
 * Zaurus ASY message content.
 *
 * The zdtm_asy_msg_content is a structure which represents an ASY
 * Zaurus message content after being parsed from the raw message
 * content.
 */
struct zdtm_asy_msg_content {
    unsigned char new_list_id;
    uint16_t num_new_sync_ids;  // Number of new sync ids.
    uint32_t *new_sync_ids;     // Array of new sync ids.
    unsigned char mod_list_id;
    uint16_t num_mod_sync_ids;  // Number of mod sync ids.
    uint32_t *mod_sync_ids;     // Array of mod sync ids.
    unsigned char del_list_id;
    uint16_t num_del_sync_ids;  // Number of del sync ids.
    uint32_t *del_sync_ids;     // Array of ndel sync ids.
};
extern const char *ASY_MSG_TYPE;
#define IS_ASY(x) (memcmp(x->body.type, ASY_MSG_TYPE, MSG_TYPE_SIZE) == 0)

int zdtm_parse_raw_asy_msg(void *buf, struct zdtm_asy_msg_content *asy);

#endif
