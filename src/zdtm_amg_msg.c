#include "zdtm_amg_msg.h"

const char *AMG_MSG_TYPE = "AMG";

int zdtm_parse_raw_amg_msg(void *buf, struct zdtm_amg_msg_content *amg) {
    buf = buf + 2;
    amg->empty_sync_type = (*(unsigned char *)buf);

    return 0;
}
