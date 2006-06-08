#include "zdtm_rms_msg.h"
#include <string.h>

const char *RMS_MSG_TYPE = "RMS";

int zdtm_rms_length(struct zdtm_rms_msg_content *rms){
    return sizeof(struct zdtm_rms_msg_content);
}

void *zdtm_rms_write(void *buf, struct zdtm_rms_msg_content *rms){
        if(rms->log_size > sizeof(rms->log))
            return NULL;
        
#ifdef WORDS_BIGENDIAN            
        *((uint16_t*)buf) = zdtm_liltobigs(rms->log_size);
        buf += sizeof(uint16_t);
        
#else
        *((uint16_t*)buf) = rms->log_size;
        buf += sizeof(uint16_t);

#endif

        memcpy(buf, rms->log, rms->log_size);

        buf += rms->log_size;

        memset(buf, 0x00, sizeof(rms->log) - rms->log_size);

        buf += sizeof(rms->log) - rms->log_size;

        return buf;
}

