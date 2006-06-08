#include "zdtm_rts_msg.h"
#include <string.h>

const char *RTS_MSG_TYPE = "RTS";


int zdtm_rts_length(struct zdtm_rts_msg_content *rts){
    return sizeof(struct zdtm_rts_msg_content);
}

void *zdtm_rts_write(void *buf, struct zdtm_rts_msg_content *rts){
    memcpy(buf, rts->date, RTS_DATE_LEN);
    buf += RTS_DATE_LEN;
    return buf; 
}

