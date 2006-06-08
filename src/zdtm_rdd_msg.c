#include "zdtm_rdd_msg.h"

const char *RDD_MSG_TYPE = "RDD";

int zdtm_rdd_length(struct zdtm_rdd_msg_content *rdd){
    return sizeof(rdd->sync_type) +
           sizeof(rdd->num_sync_ids) +
           sizeof(rdd->sync_id);
}

void *zdtm_rdd_write(void *buf, struct zdtm_rdd_msg_content *rdd){
    *((unsigned char*)buf++) = rdd->sync_type;

#ifdef WORDS_BIGENDIAN
    *((uint16_t*)buf) = zdtm_liltobigs(rdd->num_sync_ids);
    buf += sizeof(uint16_t);

    *((uint32_t*)buf) = zdtm_liltobigs(rdd->sync_id);
    buf += sizeof(uint32_t);

#else
    *((uint16_t*)buf) = rdd->num_sync_ids;
    buf += sizeof(uint16_t);

    *((uint32_t*)buf) = rdd->sync_id;
    buf += sizeof(uint32_t);

#endif

    return buf;
}

