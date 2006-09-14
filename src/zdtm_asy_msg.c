#include "zdtm_asy_msg.h"

const char *ASY_MSG_TYPE = "ASY";

/**
 * Parse a raw ASY message.
 *
 * The zdtm_parse_raw_asy_msg function takes a raw ASY message and
 * parses it into it's appropriate components and fills in the asy
 * content fields so that the data can be easily obtained at a later
 * point in time.
 * @param buf Pointer to ASY message raw content.
 * @param asy Pointer to struct to stare parsed ASY message content in.
 * @return An integer representing success (zero) or failure (non-zero).
 * @retval 0 Successfully parsed the adi message.
 * @retval -1 Failed to allocate memory for new sync ids.
 * @retval -2 Failed to allocate memory for mod sync ids.
 * @retval -3 Failed to allocate memory for del sync ids.
 */
int zdtm_parse_raw_asy_msg(void *buf, struct zdtm_asy_msg_content *asy) {
    int i;

    // New List
    asy->new_list_id = *((unsigned char *)buf);
    buf += 1;

#ifdef WORDS_BIGENDIAN
    asy->num_new_sync_ids = zdtm_liltobigs(*((uint16_t *)buf));
    buf += sizeof(uint16_t);
#else
    asy->num_new_sync_ids = *((uint16_t *)buf);
    buf += sizeof(uint16_t);
#endif

    asy->new_sync_ids = malloc(sizeof(uint32_t) * asy->num_new_sync_ids);
    if (asy->new_sync_ids == NULL)
        return -1;
    
    for (i = 0; i < asy->num_new_sync_ids; i++) {
#ifdef WORDS_BIGENDIAN
        asy->new_sync_ids[i] = zdtm_liltobigl(*((uint32_t *)buf));
        buf += sizeof(uint32_t);
#else
        asy->new_sync_ids[i] = *((uint32_t *)buf);
        buf += sizeof(uint32_t);
#endif
    }

    // Mod List
    asy->mod_list_id = *((unsigned char *)buf);
    buf += 1;

#ifdef WORDS_BIGENDIAN
    asy->num_mod_sync_ids = zdtm_liltobigs(*((uint16_t *)buf));
    buf += sizeof(uint16_t);
#else
    asy->num_mod_sync_ids = *((uint16_t *)buf);
    buf += sizeof(uint16_t);
#endif

    asy->mod_sync_ids = malloc(sizeof(uint32_t) * asy->num_mod_sync_ids);
    if (asy->mod_sync_ids == NULL) {
        free((void *)asy->new_sync_ids);
        return -2;
    }

    for (i = 0; i < asy->num_mod_sync_ids; i++) {
#ifdef WORDS_BIGENDIAN
        asy->mod_sync_ids[i] = zdtm_liltobigl(*((uint32_t *)buf));
        buf += sizeof(uint32_t);
#else
        asy->mod_sync_ids[i] = *((uint32_t *)buf);
        buf += sizeof(uint32_t);
#endif
    }

    // Del List
    asy->del_list_id = *((unsigned char *)buf);
    buf += 1;

#ifdef WORDS_BIGENDIAN
    asy->num_del_sync_ids = zdtm_liltobigs(*((uint16_t *)buf));
    buf += sizeof(uint16_t);
#else
    asy->num_del_sync_ids = *((uint16_t *)buf);
    buf += sizeof(uint16_t);
#endif

    asy->del_sync_ids = malloc(sizeof(uint32_t) * asy->num_del_sync_ids);
    if (asy->del_sync_ids == NULL) {
        free((void *)asy->new_sync_ids);
        free((void *)asy->mod_sync_ids);
        return -3;
    }

    for (i = 0; i < asy->num_del_sync_ids; i++) {
#ifdef WORDS_BIGENDIAN
        asy->del_sync_ids[i] = zdtm_liltobigl(*((uint32_t *)buf));
        buf += sizeof(uint326_t);
#else
        asy->del_sync_ids[i] = *((uint32_t *)buf);
        buf += sizeof(uint32_t);
#endif
    }

    return 0;
}
