#ifndef ZDTM_AIG_MSG_H
#define ZDTM_AIG_MSG_H

#include "zdtm_common.h"

/**
 * Zaurus AIG message content.
 *
 * The zdtm_aig_msg_content is a structure which represents an AIG
 * Zaurus message content after being parsed from the raw message
 * content.
 */
struct zdtm_aig_msg_content {
    uint16_t model_str_len;     // length of the model string
    unsigned char *model_str;   // string of the zaurus model info
    unsigned char uk_data_0[5]; // general unknown data
    unsigned char language[2];  // an identifier of the zaurus lang
    unsigned char auth_state;   // zaurus authentication state
    unsigned char uk_data_1[6]; // general unknown data
};
const char *AIG_MSG_TYPE = "AIG";
#define IS_AIG(x) (memcmp(x->body.type, AIG_MSG_TYPE, MSG_TYPE_SIZE) == 0)

int zdtm_parse_raw_aig_msg(void *buf, struct zdtm_aig_msg_content *aig);

#endif
