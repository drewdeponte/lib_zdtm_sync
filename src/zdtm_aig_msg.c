#include "zdtm_aig_msg.h"

/**
 * Parse a raw AIG message.
 *
 * The zdtm_parse_raw_aig_msg function takes a raw AIG message and
 * parses it into it's appropriate components and fills in the aig
 * content fields so that the data can be easily obtained at a later
 * point in time.
 * @param buf Pointer to AIG message raw content.
 * @param aig Pointer to struct to store parsed AIG message content in.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully parsed the aig message.
 * @retval -1 Failed to allocate memory for the model string.
 */
int zdtm_parse_raw_aig_msg(void *buf, struct zdtm_aig_msg_content *aig) {
#ifdef WORDS_BIGENDIAN
    aig->model_str_len = zdtm_liltobigs(*((uint16_t *)buf));
#else
    aig->model_str_len = *((uint16_t *)buf);
#endif
    buf += sizeof(uint16_t);

    aig->model_str = (unsigned char *)malloc(aig->model_str_len);
    if (aig->model_str == NULL) {
        return -1;
    }
    memcpy((void *)aig->model_str, buf, aig->model_str_len);
    buf += aig->model_str_len;

    memcpy((void *)aig->uk_data_0, buf, 5);
    buf += 5;

    memcpy((void *)aig->language, buf, 2);
    buf += 2;

    aig->auth_state = *((unsigned char *)buf);
    buf += 1;

    memcpy((void *)aig->uk_data_1, buf, 6);

    return 0;
}
