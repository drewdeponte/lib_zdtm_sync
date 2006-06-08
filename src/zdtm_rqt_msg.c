#include "zdtm_rqt_msg.h"

const char *RQT_MSG_TYPE = "RQT";

int zdtm_rqt_length(struct zdtm_rqt_msg_content *rqt){
    return sizeof(rqt->null_bytes);
}

void *zdtm_rqt_write(void *buf, struct zdtm_rqt_msg_content *rqt){
    memcpy(buf, rqt->null_bytes, sizeof(rqt->null_bytes));
    buf += sizeof(rqt->null_bytes);
    return buf;
}
