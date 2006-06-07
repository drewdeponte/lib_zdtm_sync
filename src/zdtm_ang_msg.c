#include "zdtm_ang_msg.h"

/**
 * Parse a raw ANG message.
 *
 * The zdtm_parse_raw_ang_msg function takes a raw ANG message and
 * parses it into it's appropriate components and fills in the ang
 * content fields so that the data con be easily obtained at a later
 * point in time.
 * @param buf Pointer to ANG message raw content.
 * @param ang Pointer to struct to store parsed ANG message content in.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully parsed the ang message.
 */
int zdtm_parse_raw_ang_msg(void *buf, struct zdtm_ang_msg_content *ang) {
    ang->uk_data_0 = *(unsigned char *)buf;
    return 0;
}
