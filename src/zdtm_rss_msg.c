#include "zdtm_rss_msg.h"

const char *RSS_MSG_TYPE = "RSS";


int zdtm_rss_length(struct zdtm_rss_msg_content *rss){
    return sizeof(struct zdtm_rss_msg_content);
}

void *zdtm_rss_write(void *buf, struct zdtm_rss_msg_content *rss){
    *((unsigned char*)buf++) = rss->uk_1;
    *((unsigned char*)buf++) = rss->sync_type;
    *((unsigned char*)buf++) = rss->uk_2;
    return buf;
}

