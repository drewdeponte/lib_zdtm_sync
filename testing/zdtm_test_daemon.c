#include "zdtm_sync.h"
#include <stdio.h>

int test_send_message(zdtm_lib_env *cur_env, zdtm_msg *msg) {
    int r;
    zdtm_msg rmsg;    

    printf("<--- rqst");
    memset(&rmsg, 0, sizeof(zdtm_msg));
    r = zdtm_recv_message(cur_env, &rmsg);
    printf(" - (%d).\n", r);
    if(r != 2){ return 1; }
    
    printf("---> %c%c%c", msg->body.type[0], msg->body.type[1],
        msg->body.type[2]);
    r = zdtm_send_message(cur_env, msg);
    printf(" - (%d).\n", r);
    if(r != 0){ return 1; }

    printf("<--- ack");
    memset(&rmsg, 0, sizeof(zdtm_msg));
    r = zdtm_recv_message(cur_env, &rmsg);
    printf(" - (%d).\n", r);
    if(r != 1){ return 1; }

    return 0;
}

int test_recv_message(zdtm_lib_env *cur_env, zdtm_msg *rmsg) {
    int r;

    printf("---> rqst");
    r = zdtm_send_rqst_message(cur_env);
    printf(" - (%d).\n", r);
    if(r != 0){ return 1; }
    
    memset(rmsg, 0, sizeof(zdtm_msg));
    r = zdtm_recv_message(cur_env, rmsg);
    printf("<--- %c%c%c", rmsg->body.type[0], rmsg->body.type[1],
        rmsg->body.type[2]);
    printf(" - (%d).\n", r);
    if(r != 0) { return 1; }
    
    printf("---> ack");
    r = zdtm_send_ack_message(cur_env);
    printf(" - (%d).\n", r);
    if(r != 0){ return 1; }

    return 0;
}

int test_initialize(zdtm_lib_env *cur_env) {
    int r;

    r = zdtm_open_log(cur_env);
    printf("zdtm_open_log - (%d).\n", r);
    if(r != 0){ return 1; }
    
    r = zdtm_listen_for_zaurus(cur_env);
    printf("zdtm_listen_for_zaurus - (%d).\n", r);
    if(r != 0){ return 1; }

    return 0;
}

int test_finalize(zdtm_lib_env *cur_env) {
    int r;

    r = zdtm_close_log(cur_env);
    printf("zdtm_close_log - (%d).\n", r);
    if(r != 0){ return 1; }
    
    return 0;
}

int test_connect(zdtm_lib_env *cur_env) {
    int r;
    zdtm_msg msg;

    r = zdtm_conn_to_zaurus(cur_env, "192.168.129.201");
    printf("zdtm_conn_to_zaurus - (%d).\n", r);
    if(r != 0){ return 1; }

    printf("making a simple RAY message.\n");
    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RAY_MSG_TYPE, MSG_TYPE_SIZE);

    r = zdtm_send_message_to(cur_env, &msg, cur_env->reqfd);
    printf("zdtm_send_message_to - (%d).\n", r);
    if(r != 0){ return 1; }

    /* Handle the connection coming back from the Zaurus */
    r = zdtm_handle_zaurus_conn(cur_env);
    printf("zdtm_handle_zaurus_conn - (%d).\n", r);
    if(r != 0){ return 1; }

    return 0;
}

int test_disconnect(zdtm_lib_env *cur_env) {
    int r;
    zdtm_msg msg, rmsg;

    /* Make a simple RQT message, send it and recv AEX */
    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RQT_MSG_TYPE, MSG_TYPE_SIZE);
    memset(msg.body.cont.rqt.null_bytes, 0,
        sizeof(msg.body.cont.rqt.null_bytes));

    r = test_send_message(cur_env, &msg);
    if(r != 0){ return 1; }

    r = test_recv_message(cur_env, &rmsg);
    if(r != 0){ return 1; }

    /* receive a request and send a RAY message */
    printf("<--- rqst");
    memset(&rmsg, 0, sizeof(zdtm_msg));
    r = zdtm_recv_message(cur_env, &rmsg);
    printf(" - (%d).\n", r);
    if(r != 2){ return 1; }

    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RAY_MSG_TYPE, MSG_TYPE_SIZE);

    printf("---> RAY");
    r = zdtm_send_message(cur_env, &msg);
    printf(" - (%d).\n", r);
    if(r != 0){ return 1; }

    r = zdtm_close_zaurus_conn(cur_env);
    printf("zdtm_close_zaurus_conn - (%d).\n", r);
    if(r != 0){ return 1; }

    r = zdtm_close_conn_to_zaurus(cur_env);
    printf("zdtm_close_conn_to_zaurus - (%d).\n", r);
    if(r != 0){ return 1; }

    return 0;
}

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
     * test_send_message() function. */
    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RAY_MSG_TYPE, MSG_TYPE_SIZE);

    printf("---> RAY");
    r = zdtm_send_message(cur_env, &msg);
    printf(" - (%d).\n", r);
    if(r != 0){ return 1; }

    printf("<--- ack");
    memset(&rmsg, 0, sizeof(zdtm_msg));
    r = zdtm_recv_message(cur_env, &rmsg);
    printf(" - (%d).\n", r);
    if(r != 1){ return 1; }

    r = test_recv_message(cur_env, &rmsg);
    if(r != 0){ return 1; }
   
    /* make a simple RIG message, send it and recv AIG */
    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RIG_MSG_TYPE, MSG_TYPE_SIZE);

    r = test_send_message(cur_env, &msg);
    if(r != 0){ return 1; }

    r = test_recv_message(cur_env, &rmsg);
    if(r != 0){ return 1; }
    
    memcpy(buff, rmsg.body.cont.aig.model_str,
        rmsg.body.cont.aig.model_str_len);
    buff[rmsg.body.cont.aig.model_str_len] = '\0';
    printf("Model String: %s\n", buff);
    printf("Language: %c%c\n",
        rmsg.body.cont.aig.language[0],
        rmsg.body.cont.aig.language[1]);
    printf("AuthState: 0x%.2x\n", rmsg.body.cont.aig.auth_state);
 
    /* make a simple RIG message, send it and recv AIG */
    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RIG_MSG_TYPE, MSG_TYPE_SIZE);

    r = test_send_message(cur_env, &msg);
    if(r != 0){ return 1; }

    r = test_recv_message(cur_env, &rmsg);
    if(r != 0){ return 1; }
    
    memcpy(buff, rmsg.body.cont.aig.model_str,
        rmsg.body.cont.aig.model_str_len);
    buff[rmsg.body.cont.aig.model_str_len] = '\0';
    printf("Model String: %s\n", buff);
    printf("Language: %c%c\n",
        rmsg.body.cont.aig.language[0],
        rmsg.body.cont.aig.language[1]);
    printf("AuthState: 0x%.2x\n", rmsg.body.cont.aig.auth_state);
 
    /* make a simple RMG message, send it and recv AMG */
    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RMG_MSG_TYPE, MSG_TYPE_SIZE);
    msg.body.cont.rmg.uk = 0x01;
    msg.body.cont.rmg.sync_type = SYNC_TYPE_TODO;

    r = test_send_message(cur_env, &msg);
    if(r != 0){ return 1; }

    r = test_recv_message(cur_env, &rmsg);
    if(r != 0){ return 1; }

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

    /* make a  RTG message, send it and recv an ATG */
    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RTG_MSG_TYPE, MSG_TYPE_SIZE);
    
    r = test_send_message(cur_env, &msg);
    if(r != 0){ return 1; }

    r = test_recv_message(cur_env, &rmsg);
    if(r != 0){ return 1; }

    /* make a  RTS message, send it and recv an AEX */
    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RTS_MSG_TYPE, MSG_TYPE_SIZE);
    memcpy(msg.body.cont.rts.date, "20060920011020", RTS_DATE_LEN);
    
    r = test_send_message(cur_env, &msg);
    if(r != 0){ return 1; }

    r = test_recv_message(cur_env, &rmsg);
    if(r != 0){ return 1; }

    /* BEFORE I GO ANY FURTHER I MUST BE SURE THAT A FULL SYNC IS NOT
     * REQUIRED. IF A FULL SYNC IS REQUIRED THEN I SHOULD HANDLE THE
     * FULL SYNC. */
    if (slow_sync_required) {
        printf("SLOW SYNC REQUIRED!\n");
        printf("This test_daemon currently does not support slow syncs\n");
        printf("Hence, you must sync your Zaurus with the Windows sync\n");
        printf("software and come back and sync with the test_daemon\n");
        printf("to get beyond this point in the synchronization\n");
        return 1;
    }

    /* make a  RSY message, send it and recv an ASY */
    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RSY_MSG_TYPE, MSG_TYPE_SIZE);
    msg.body.cont.rsy.sync_type = SYNC_TYPE_TODO;
    msg.body.cont.rsy.uk = 0x07;
    
    r = test_send_message(cur_env, &msg);
    if(r != 0){ return 1; }

    r = test_recv_message(cur_env, &rmsg);
    if(r != 0){ return 1; }

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

    return 0;
}

int main(int argc, char *argv[]) {
    int r;
    
    zdtm_lib_env cur_env;
    
    memset(&cur_env, 0, sizeof(zdtm_lib_env));

    r = test_initialize(&cur_env);
    printf("initialize - (%d).\n", r);
    if(r != 0){ return 1; }

    r = test_connect(&cur_env);
    printf("connect - (%d).\n", r);
    if(r != 0){ return 1; }

    r = test_get_changeinfo(&cur_env);
    printf("get_changeinfo - (%d).\n", r);
    if(r != 0){ return 1; }

    r = test_disconnect(&cur_env);
    printf("disconnect - (%d).\n", r);
    if(r != 0){ return 1; }

    r = test_finalize(&cur_env);
    printf("finalize - (%d).\n", r);
    if(r != 0){ return 1; }

    return 0;
}
