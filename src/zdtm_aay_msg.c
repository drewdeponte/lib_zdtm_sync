#include "zdtm_aay_msg.h"

const char *AAY_MSG_TYPE = "AAY";

/**
 * Parse a raw AAY message.
 *
 * The zdtm_parse_raw_aay_msg function takes a raw AAY message and
 * parses it into it's appropriate components and fills in the aay
 * content fields so that the data can be easily obtained at a later
 * point in time.
 * @param buf Pointer to AAY message raw content.
 * @param aay Pointer to struct to store parsed AAY message content in.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully parsed the aay message.
 */
int zdtm_parse_raw_aay_msg(void *buf, struct zdtm_aay_msg_content *aay) {
    memcpy((void *)aay->uk_data_0, buf, 3);
    return 0;
}
