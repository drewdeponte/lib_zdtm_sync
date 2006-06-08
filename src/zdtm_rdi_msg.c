#include "zdtm_rdi_msg.h"

const char *RDI_MSG_TYPE = "RDI";


int zdtm_rdi_length(struct zdtm_rdi_msg_content *rdi){
    return sizeof(struct zdtm_rdi_msg_content);
}

void *zdtm_rdi_write(void *buf, struct zdtm_rdi_msg_content *rdi){
    *((unsigned char*)buf++) = rdi->sync_type;
    *((unsigned char*)buf++) = rdi->uk;
    return buf;
}
