#include "zdtm_adi_msg.h"

const char *ADI_MSG_TYPE = "ADI";

/**
 * Parse a raw ADI message.
 *
 * The zdtm_parse_raw_adi_msg function takes a raw ADI message and
 * parses it into it's appropriate components and fills in the adi
 * content fields so that the data can be easily obtained at a later
 * point in time.
 * @param buf Pointer to ADI message raw content.
 * @param adi Pointer to struct to store parsed ADI message content in.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully parsed the adi message.
 * @retval -1 Failed to allocate memory for adi message params.
 * @retval -2 Failed to allocate memory for a adi msg param description.
 */
int zdtm_parse_raw_adi_msg(void *buf, struct zdtm_adi_msg_content *adi) {
    int i, j;

#ifdef WORDS_BIGENDIAN
    adi->num_cards = zdtm_liltobigl(*((uint32_t *)buf));
    buf += sizeof(uint32_t);

    adi->num_params = zdtm_liltobigs(*((uint16_t *)buf));
    buf += sizeof(uint16_t);
#else
    adi->num_cards = *((uint32_t *)buf);
    buf += sizeof(uint32_t);

    adi->num_params = *((uint16_t *)buf);
    buf += sizeof(uint16_t);
#endif

    adi->uk_data_0 = *((unsigned char *)buf);
    buf += 1;

    adi->params = (struct zdtm_adi_msg_param *)malloc(
        (adi->num_params * sizeof(struct zdtm_adi_msg_param)));
    if (adi->params == NULL)
        return -1;

    for (i = 0; i < adi->num_params; i++) {
        memcpy((void *)adi->params[i].abrev, buf, 4);
        buf += 4;
    }

    for (i = 0; i < adi->num_params; i++) {
        memcpy((void *)&adi->params[i].type_id, buf, 1);
        buf += 1;
    }

    for (i = 0; i < adi->num_params; i++) {
#ifdef WORDS_BIGENDIAN
        adi->params[i].desc_len = zdtm_liltobigs(*((uint16_t *)buf));
#else
        adi->params[i].desc_len = *((uint16_t *)buf);
#endif
        buf += sizeof(uint16_t);

        adi->params[i].desc = \
            (unsigned char *)malloc(adi->params[i].desc_len);
        if (adi->params[i].desc == NULL) {
            for (j = 0; j < i; j++) {
                free((void *)adi->params[j].desc);
            }
            free((void *)adi->params);
            return -2;
        }

        memcpy((void *)adi->params[i].desc, buf, adi->params[i].desc_len);
        buf += adi->params[i].desc_len;
    }

    return 0;
}
