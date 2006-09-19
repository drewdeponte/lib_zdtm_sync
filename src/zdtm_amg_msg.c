#include "zdtm_amg_msg.h"

const char *AMG_MSG_TYPE = "AMG";

int zdtm_parse_raw_amg_msg(void *p_cont, struct zdtm_amg_msg_content *amg) {
    memcpy((void *)amg->sl, p_cont, 2);
    p_cont = p_cont + 2;
    amg->fullsync_flags = *((unsigned char *)p_cont);
    p_cont = p_cont + 1;
    memcpy((void *)amg->uk, p_cont, 46);

    return 0;
}
