#include "zdtm_rrl_msg.h"
#include <string.h>

const char *RRL_MSG_TYPE = "RRL";

int zdtm_rrl_length(struct zdtm_rrl_msg_content *rrl){
    return sizeof(rrl->pw_size) + rrl->pw_size;
}

void *zdtm_rrl_write(void *buf, struct zdtm_rrl_msg_content *rrl){
    *((unsigned char*)buf++) = rrl->pw_size;
    memcpy(buf, rrl->pw, rrl->pw_size);
    buf += rrl->pw_size;
    return buf;
}
