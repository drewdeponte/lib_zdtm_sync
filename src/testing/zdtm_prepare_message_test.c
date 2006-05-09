#include "zdtm_sync.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    int i;

    /* Set up a fake message */
    zdtm_lib_env cur_env;
    zdtm_msg msg;
    
    memset(&cur_env, 0, sizeof(zdtm_lib_env));
    memset(&msg, 0, sizeof(zdtm_msg));

    /* Make a simply RAY msg. */
    memcpy(msg.body.type, RAY_MSG_TYPE, MSG_TYPE_SIZE);

    zdtm_prepare_message(&cur_env, &msg);

    /* Dump the header: */
    printf("Header: ");
    for(i = 0; i < MSG_HDR_SIZE; ++i)
        printf("0x%x ", (unsigned char)msg.header[i]);
    printf("\n");

    /* Dump the raw body in hex. */
    printf("Body (Hex): ");
    for(i = 0; i < msg.body_size; ++i)
        printf("0x%x ", ((unsigned char*)(msg.body.p_raw_content))[i]);
    printf("\n");

    printf("Content Size: %u\n", msg.cont_size);
    printf("Checksum: 0x%x\n", msg.check_sum);
    
    return 0;
}
