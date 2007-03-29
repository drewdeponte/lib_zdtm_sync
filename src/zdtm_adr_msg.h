#ifndef _ZDTM_ADR_MSG_H_
#define _ZDTM_ADR_MSG_H_ 1

#include "zdtm_common.h"

struct zdtm_adr_msg_param {
    uint32_t param_len;
    unsigned char *param_data;
};

struct zdtm_adr_msg_content {
    unsigned char uk[2];
    uint16_t num_params;
    struct zdtm_adr_msg_param *params;
};

extern const char *ADR_MSG_TYPE;
#define IS_ADR(x) (memcmp(x->body.type, ADR_MSG_TYPE, MSG_TYPE_SIZE) == 0)

int zdtm_parse_raw_adr_msg(void *buf, struct zdtm_adr_msg_content *adr);

#endif
