#include "zdtm_rmg_msg.h"

const char *RMG_MSG_TYPE = "RMG";


int zdtm_rmg_length(struct zdtm_rmg_msg_content *rmg){
    return sizeof(struct zdtm_rmg_msg_content);
}

void *zdtm_rmg_write(void *buf, struct zdtm_rmg_msg_content *rmg){
    *((unsigned char*)buf++) = rmg->uk;
    *((unsigned char*)buf++) = rmg->sync_type;
    return buf;
}

