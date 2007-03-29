#include "zdtm_adr_msg.h"

const char *ADR_MSG_TYPE = "ADR";

int zdtm_parse_raw_adr_msg(void *buf, struct zdtm_adr_msg_content *adr) {
    int i, j;

    memcpy(adr->uk, buf, 2);
    buf += 2;

#ifdef WORDS_BIGENDIAN
    adr->num_params = zdtm_liltobigl(*((uint32_t *)buf));
    buf += sizeof(uint32_t);
#else
    adr->num_params = *((uint32_t *)buf);
    buf += sizeof(uint32_t);
#endif

    adr->params = (struct zdtm_adr_msg_param *)malloc(
        (adr->num_params * sizeof(struct zdtm_adr_msg_param)));
    if (adr->params == NULL)
        return -1;

    for (i = 0; i < adr->num_params; i++) {
#ifdef WORDS_BIGENDIAN
        adr->params[i].param_len = zdtm_liltobigl(*((uint32_t *)buf));
        buf += sizeof(uint32_t);
#else
        adr->params[i].param_len = *((uint32_t *)buf);
        buf += sizeof(uint32_t);
#endif
        
        adr->params[i].param_data = malloc(adr->params[i].param_len);
        if (adr->params[i].param_data == NULL) {
            for (j = 0; j < i; j++) {
                /* free each previously allocated params data */
                free(adr->params[j].param_data);
            }
            /* free the entire param array that was allocated */
            free(adr->params);
            return -2;
        }
        memcpy(adr->params[i].param_data, buf, adr->params[i].param_len);
        buf += adr->params[i].param_len;
    }

    return 0;
}
