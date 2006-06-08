#include "zdtm_rge_msg.h"

const char *RGE_MSG_TYPE = "RGE";

int zdtm_rge_length(struct zdtm_rge_msg_content *rge){
    return sizeof(rge->path_len) + rge->path_len;
}

void *zdtm_rge_write(void *buf, struct zdtm_rge_msg_content *rge){ 
    *((uint16_t*)buf) = rge->path_len;
    buf += sizeof(rge->path_len);

    memcpy(buf, rge->path, rge->path_len);
    buf += rge->path_len;

    return buf;
}

