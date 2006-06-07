#include "zdtm_common.h"

/**
 * Convert little-endian short to a big-endian short.
 *
 * The zdtm_liltobigs function converts a little-endian uint16_t
 * to a big-endian uint16_t.
 * @param lilshort The lil-end uint16_t to convert to big-end uint16_t.
 * @return The big-endian version of given little-endian uint16_t.
 */
uint16_t zdtm_liltobigs(uint16_t lilshort) {
    int size, i;
    unsigned char buff[sizeof(uint16_t)];

    size = sizeof(uint16_t);

    for (i = 0; i < size; i++) {
        buff[i] = ((unsigned char *)&lilshort)[(size - i - 1)];
    }

    return *(uint16_t *)buff;
}

/**
 * Convert little-endian long to a big-endian long.
 *
 * The zdtm_liltobigl function converts a little-endian uint32_t
 * to a big-endian uint32_t.
 * @param lillong The lil-end uint32_t to convert to big-end uint32_t.
 * @return The big-endian version of given little-endian uint32_t.
 */
uint32_t zdtm_liltobigl(uint32_t lillong) {
    int size, i;
    unsigned char buff[sizeof(uint32_t)];

    size = sizeof(uint32_t);
    
    for (i = 0; i < size; i++) {
        buff[i] = ((unsigned char *)&lillong)[(size - i - 1)];
    }

    return *(uint32_t *)buff;
}

/** 
 * Convert big-endian short to a little-endian short.
 *
 * The zdtm_bigtolils function converts a big-endian uint16_t to a
 * little-endian uint16_t.
 * @param bigshort The big-end uint16_t to convert to lil-end uint16_t.
 * @return The little-endian version of given big-endian uint16_t.
 */ 
uint16_t zdtm_bigtolils(uint16_t bigshort) {
    return zdtm_liltobigs(bigshort);
}

/**
 * Convert big-endian long to a little-endian long.
 *
 * The zdtm_bigtolils function converts a big-endian uint32_t to a
 * little-endian uint32_t.
 * @param biglong The big-end uint32_t to convert to lil-end uint32_t.
 * @return The little-endian version of given big-endian uint32_t.
 */
uint32_t zdtm_bigtolill(uint32_t biglong) {
    return zdtm_liltobigl(biglong);
}
