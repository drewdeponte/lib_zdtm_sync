#include "zdtm_types.h"

/* This is a static message header to be used for messages that
 * originate from the Zaurus side of the synchronization. */
const unsigned char ZMSG_HDR[MSG_HDR_SIZE] =
{0x00, 0x00, 0x00, 0x00, 0x00, 0x96, 0x01, 0x01, 0x00, 0xff, 0xff, 0xff, 0xff};

/* This is a static message header to be used for messages that
 * originate from the Desktop side of the synchronization. The two 0xff
 * bytes are to be replaced by the message content size later. */
const unsigned char DMSG_HDR[MSG_HDR_SIZE] =
{0x00, 0x00, 0x00, 0x00, 0x00, 0x96, 0x01, 0x01, 0x0c, 0xff, 0xff, 0x00, 0x00};
