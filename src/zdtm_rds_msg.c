#include "zdtm_rds_msg.h"

const char *RDS_MSG_TYPE = "RDS";

int zdtm_rds_length(struct zdtm_rds_msg_content *rds){
    return sizeof(struct zdtm_rds_msg_content);
}

void *zdtm_rds_write(void *buf, struct zdtm_rds_msg_content *rds){
    *((unsigned char*)buf++) = rds->sync_type;
    *((unsigned char*)buf++) = rds->status;
    memcpy(buf, rds->null_bytes, sizeof(rds->null_bytes));
    buf += sizeof(rds->null_bytes);

    return buf;
}

