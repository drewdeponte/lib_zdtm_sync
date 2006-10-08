#include "zdtm_sync.h"
#include <stdio.h>

int test_get_changeinfo(zdtm_lib_env *cur_env) {
    int r;
    int i;
    int slow_sync_required;
    zdtm_msg msg, rmsg;
    char buff[256];

    slow_sync_required = 0;

    /* make simple RAY message, send it and recv AAY */

    /* This initial RAY message send does not wait for a rqst message as
     * all the rest of the send messages do hence, it does not use the
     * zdtm_send_message() function. */
    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RAY_MSG_TYPE, MSG_TYPE_SIZE);
    r = _zdtm_send_message(cur_env, &msg);
    if (r != 0) {
        printf("Error(%d): Failed to send RAY message to Zaurus.\n", r);
        return 1;
    }
    printf("- Sent RAY message to Zaurus.\n");

    memset(&rmsg, 0, sizeof(zdtm_msg));
    r = _zdtm_recv_message(cur_env, &rmsg);
    if (r != 1) {
        printf("Error(%d): Failed to receive ack from Zaurus.\n", r);
        return 1;
    }
    printf("- Received ack message from Zaurus.\n");
    _zdtm_clean_message(&rmsg);

    memset(&rmsg, 0, sizeof(zdtm_msg));
    r = zdtm_recv_message(cur_env, &rmsg);
    if (r != 0) {
        printf("Error(%d): Failed to received AAY message from Zaurus.\n", r);
        return 1;
    }
    printf("- Received AAY message from the Zaurus.\n");
    _zdtm_clean_message(&rmsg);
   
    /* make a simple RIG message, send it and recv AIG */
    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RIG_MSG_TYPE, MSG_TYPE_SIZE);
    r = zdtm_send_message(cur_env, &msg);
    if (r != 0) {
        printf("Error(%d): Failed to send RIG message to Zaurus.\n", r);
        return 1;
    }
    printf("- Sent RIG message to Zaurus.\n");

    memset(&msg, 0, sizeof(zdtm_msg));
    r = zdtm_recv_message(cur_env, &rmsg);
    if (r != 0) {
        _zdtm_clean_message(&rmsg);
        printf("Error(%d): Failed to receive AIG message from Zaurus.\n", r);
        return 1;
    }
    printf("- Received AIG message from Zaurus.\n");
    
    memcpy(buff, rmsg.body.cont.aig.model_str,
        rmsg.body.cont.aig.model_str_len);
    buff[rmsg.body.cont.aig.model_str_len] = '\0';
    printf("Model String: %s\n", buff);
    printf("Language: %c%c\n",
        rmsg.body.cont.aig.language[0],
        rmsg.body.cont.aig.language[1]);
    printf("AuthState: 0x%.2x\n", rmsg.body.cont.aig.auth_state);

    if (rmsg.body.cont.aig.auth_state == 0x0b ||
        rmsg.body.cont.aig.auth_state == 0x07) {
        printf("The authentication state requires a passcode to sync.\n");
        printf("The test_daemon currently does not support passcode\n");
        printf("authentication. Please disable the passcode on your\n");
        printf("Zaurus and try again.\n");
        _zdtm_clean_message(&rmsg);
        return 0;
    }
    _zdtm_clean_message(&rmsg);
 
    /* make a simple RIG message, send it and recv AIG */
    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RIG_MSG_TYPE, MSG_TYPE_SIZE);

    r = zdtm_send_message(cur_env, &msg);
    if(r != 0){ return 1; }

    memset(&rmsg, 0, sizeof(zdtm_msg));
    r = zdtm_recv_message(cur_env, &rmsg);
    if(r != 0){ _zdtm_clean_message(&rmsg); return 1; }
    
    memcpy(buff, rmsg.body.cont.aig.model_str,
        rmsg.body.cont.aig.model_str_len);
    buff[rmsg.body.cont.aig.model_str_len] = '\0';
    printf("Model String: %s\n", buff);
    printf("Language: %c%c\n",
        rmsg.body.cont.aig.language[0],
        rmsg.body.cont.aig.language[1]);
    printf("AuthState: 0x%.2x\n", rmsg.body.cont.aig.auth_state);

    _zdtm_clean_message(&rmsg);
 
    /* make a simple RMG message, send it and recv AMG */
    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RMG_MSG_TYPE, MSG_TYPE_SIZE);
    msg.body.cont.rmg.uk = 0x01;
    msg.body.cont.rmg.sync_type = SYNC_TYPE_TODO;

    r = zdtm_send_message(cur_env, &msg);
    if(r != 0){ return 1; }

    memset(&rmsg, 0, sizeof(zdtm_msg));
    r = zdtm_recv_message(cur_env, &rmsg);
    if(r != 0){ _zdtm_clean_message(&rmsg); return 1; }

    /* Here I check if ToDo Slow Sync is Required */
    if ((rmsg.body.cont.amg.fullsync_flags & 0x01) == 0) {
        /* todo slow sync required */
        printf("ToDo Slow Sync Required!\n");
        slow_sync_required = 1;
    }

    if ((rmsg.body.cont.amg.fullsync_flags & 0x02) == 0) {
        /* cal slow sync required */
        printf("Calendar Slow Sync Required!\n");
        slow_sync_required = 1;
    }
    
    if ((rmsg.body.cont.amg.fullsync_flags & 0x04) == 0) {
        /* address book slow sync required */
        printf("Address Book Slow Sync Required!\n");
        slow_sync_required = 1;
    }

    _zdtm_clean_message(&rmsg);

    /* make a  RTG message, send it and recv an ATG */
    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RTG_MSG_TYPE, MSG_TYPE_SIZE);
    
    r = zdtm_send_message(cur_env, &msg);
    if(r != 0){ return 1; }

    memset(&rmsg, 0, sizeof(zdtm_msg));
    r = zdtm_recv_message(cur_env, &rmsg);
    if(r != 0){ _zdtm_clean_message(&rmsg); return 1; }
    _zdtm_clean_message(&rmsg);

    /* make a  RTS message, send it and recv an AEX */
    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RTS_MSG_TYPE, MSG_TYPE_SIZE);
    memcpy(msg.body.cont.rts.date, "20060920011020", RTS_DATE_LEN);
    
    r = zdtm_send_message(cur_env, &msg);
    if(r != 0){ return 1; }

    memset(&rmsg, 0, sizeof(zdtm_msg));
    r = zdtm_recv_message(cur_env, &rmsg);
    if(r != 0){ _zdtm_clean_message(&rmsg); return 1; }
    _zdtm_clean_message(&rmsg);

    /* BEFORE I GO ANY FURTHER I MUST BE SURE THAT A FULL SYNC IS NOT
     * REQUIRED. IF A FULL SYNC IS REQUIRED THEN I SHOULD HANDLE THE
     * FULL SYNC. */
    if (slow_sync_required) {
        printf("SLOW SYNC REQUIRED!\n");
        printf("This test_daemon currently does not support slow syncs\n");
        printf("Hence, you must sync your Zaurus with the Windows sync\n");
        printf("software and come back and sync with the test_daemon\n");
        printf("to get beyond this point in the synchronization\n");
        return 0;
    }

    /* make a  RSY message, send it and recv an ASY */
    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RSY_MSG_TYPE, MSG_TYPE_SIZE);
    msg.body.cont.rsy.sync_type = SYNC_TYPE_TODO;
    msg.body.cont.rsy.uk = 0x07;
    
    r = zdtm_send_message(cur_env, &msg);
    if(r != 0){ return 1; }
    
    memset(&rmsg, 0, sizeof(zdtm_msg));
    r = zdtm_recv_message(cur_env, &rmsg);
    if(r != 0){ _zdtm_clean_message(&rmsg); return 1; }

    printf("new list id = 0x%.2x\n", rmsg.body.cont.asy.new_list_id);
    printf("num new sync ids = %u\n", rmsg.body.cont.asy.num_new_sync_ids);
    for (i = 0; i < rmsg.body.cont.asy.num_new_sync_ids; i++) {
        printf(" -- new sync id [%d] = (hex) 0x%.8x, (dec) %u\n", i,
            rmsg.body.cont.asy.new_sync_ids[i],
            rmsg.body.cont.asy.new_sync_ids[i]);
    }
    printf("mod list id = 0x%.2x\n", rmsg.body.cont.asy.mod_list_id);
    printf("num mod sync ids = %u\n", rmsg.body.cont.asy.num_mod_sync_ids);
    for (i = 0; i < rmsg.body.cont.asy.num_mod_sync_ids; i++) {
        printf(" -- mod sync id [%d] = (hex) 0x%.8x, (dec) %u\n", i,
            rmsg.body.cont.asy.mod_sync_ids[i],
            rmsg.body.cont.asy.mod_sync_ids[i]);
    }
    printf("del list id = 0x%.2x\n", rmsg.body.cont.asy.del_list_id);
    printf("num del sync ids = %u\n", rmsg.body.cont.asy.num_del_sync_ids);
    for (i = 0; i < rmsg.body.cont.asy.num_del_sync_ids; i++) {
        printf(" -- del sync id [%d] = (hex) 0x%.8x, (dec) %u\n", i,
            rmsg.body.cont.asy.del_sync_ids[i],
            rmsg.body.cont.asy.del_sync_ids[i]);
    }

    _zdtm_clean_message(&rmsg);

    return 0;
}

int main(int argc, char *argv[]) {
    int r;
    
    zdtm_lib_env cur_env;
    
    memset(&cur_env, 0, sizeof(zdtm_lib_env));

    r = zdtm_initialize(&cur_env);
    if(r != 0) {
        printf("Error(%d): Failed to initialized zdtm library.\n", r);
        return 1;
    }

    printf("Succsessfully initialized zdtm sync library.\n");

    r = zdtm_connect(&cur_env, "192.168.129.201");
    if(r != 0) {
        printf("Error(%d): Failed to connect to Zaurus.\n", r);
        return 1;
    }

    printf("Successfully connected to the Zaurus.\n");

    r = test_get_changeinfo(&cur_env);
    printf("get_changeinfo - (%d).\n", r);
    if(r != 0) { return 1; }

    r = zdtm_disconnect(&cur_env);
    if(r != 0) {
        printf("Error(%d): Failed to disconnect from the Zaurus.\n", r);
        return 1;
    }

    printf("Successfully disconnected from the Zaurus.\n");

    r = zdtm_finalize(&cur_env);
    if(r != 0) {
        printf("Error(%d): Failed to finalize the zdtm library.\n", r);
        return 1;
    }

    printf("Successfully finalized the zdtm sync library.\n");

    return 0;
}
