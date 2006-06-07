#include "zdtm_atg_msg.h"

const char *ATG_MSG_TYPE = "ATG";

/**
 * Parse a raw ATG message.
 *
 * The zdtm_parse_raw_atg_msg function takes a raw ATG message and
 * parses it into it's appropriate components and fills in the atg
 * content fields so that the data can be easily obtained at a later
 * point in time.
 * @param buf Pointer to ATG message raw content.
 * @param atg Poniter to struct to store parsed ATG message content in.
 * @return An integener representing success (zero) or failure (non-zero).
 * @retval 0 Successfully parsed the atg message.
 */
int zdtm_parse_raw_atg_msg(void *buf, struct zdtm_atg_msg_content *atg) {
    memcpy((void *)atg->year, buf, 4);
    buf += 4;
    memcpy((void *)atg->month, buf, 2);
    buf += 2;
    memcpy((void *)atg->day, buf, 2);
    buf += 2;
    memcpy((void *)atg->hour, buf, 2);
    buf += 2;
    memcpy((void *)atg->minutes, buf, 2);
    buf += 2;
    memcpy((void *)atg->seconds, buf, 2);

    return 0;
}
