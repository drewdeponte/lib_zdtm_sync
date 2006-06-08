#include "zdtm_rlr_msg.h"

const char *RLR_MSG_TYPE = "RLR";

int zdtm_rlr_length(struct zdtm_rlr_msg_content *rlr){
    return sizeof(rlr->sync_type);
}

void *zdtm_rlr_write(void *buf, struct zdtm_rlr_msg_content *rlr){
    *((unsigned char*)buf++) = rlr->sync_type;

    return buf;
}

