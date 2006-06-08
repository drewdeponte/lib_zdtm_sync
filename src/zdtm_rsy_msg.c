#include "zdtm_rsy_msg.h"

const char *RSY_MSG_TYPE = "RSY";

int zdtm_rsy_length(struct zdtm_rsy_msg_content *rsy){
    return sizeof(struct zdtm_rsy_msg_content);
}

void *zdtm_rsy_write(void *buf, struct zdtm_rsy_msg_content *rsy){
    *((unsigned char*)buf++) = rsy->sync_type;
    *((unsigned char*)buf++) = rsy->uk;
    return buf;
}
