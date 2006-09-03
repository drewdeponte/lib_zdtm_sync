#include "zdtm_sync.h"
#include <stdio.h>

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
    zdtm_msg msg, rmsg;
    char buff[256];

    /* make simple RAY message */    
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




    printf("---> rqst");
    r = zdtm_send_rqst_message(cur_env);
    printf(" - (%d).\n", r);
    if(r != 0){ return 1; }

    printf("<--- AAY");
    memset(&rmsg, 0, sizeof(zdtm_msg));
    r = zdtm_recv_message(cur_env, &rmsg);
    printf(" - (%d).\n", r);
    if(r != 0) { return 1; }
    r = zdtm_dump_msg_log(cur_env, &rmsg);
    printf("zdtm_dump_msg_log - (%d).\n", r);

    printf("---> ack");
    r = zdtm_send_ack_message(cur_env);
    printf(" - (%d).\n", r);
    if(r != 0){ return 1; }
    


    printf("<--- rqst");
    memset(&rmsg, 0, sizeof(zdtm_msg));
    r = zdtm_recv_message(cur_env, &rmsg);
    printf(" - (%d).\n", r);
    if(r != 2){ return 1; }
    
    /* make a simple RIG message */
    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RIG_MSG_TYPE, MSG_TYPE_SIZE);

    printf("---> RIG");
    r = zdtm_send_message(cur_env, &msg);
    printf(" - (%d).\n", r);
    if(r != 0){ return 1; }

    printf("<--- ack");
    memset(&rmsg, 0, sizeof(zdtm_msg));
    r = zdtm_recv_message(cur_env, &rmsg);
    printf(" - (%d).\n", r);
    if(r != 1){ return 1; }



    
    printf("---> rqst");
    r = zdtm_send_rqst_message(cur_env);
    printf(" - (%d).\n", r);
    if(r != 0){ return 1; }
    
    printf("<--- AIG");
    memset(&rmsg, 0, sizeof(zdtm_msg));
    r = zdtm_recv_message(cur_env, &rmsg);
    printf(" - (%d).\n", r);
    if(r != 0) { return 1; }
    r = zdtm_dump_msg_log(cur_env, &rmsg);
    printf("zdtm_dump_msg_log - (%d).\n", r);
    
    memcpy(buff, rmsg.body.cont.aig.model_str,
        rmsg.body.cont.aig.model_str_len);
    buff[rmsg.body.cont.aig.model_str_len] = '\0';
    printf("Model String: %s\n", buff);
    printf("Language: %c%c\n",
        rmsg.body.cont.aig.language[0],
        rmsg.body.cont.aig.language[1]);
    printf("AuthState: 0x%.2x\n", rmsg.body.cont.aig.auth_state);

    printf("---> ack");
    r = zdtm_send_ack_message(cur_env);
    printf(" - (%d).\n", r);
    if(r != 0){ return 1; }

    


    printf("<--- rqst");
    memset(&rmsg, 0, sizeof(zdtm_msg));
    r = zdtm_recv_message(cur_env, &rmsg);
    printf(" - (%d).\n", r);
    if(r != 2){ return 1; }
    
    /* make a simple RIG message */
    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RIG_MSG_TYPE, MSG_TYPE_SIZE);

    printf("---> RIG");
    r = zdtm_send_message(cur_env, &msg);
    printf(" - (%d).\n", r);
    if(r != 0){ return 1; }

    printf("<--- ack");
    memset(&rmsg, 0, sizeof(zdtm_msg));
    r = zdtm_recv_message(cur_env, &rmsg);
    printf(" - (%d).\n", r);
    if(r != 1){ return 1; }



    
    printf("---> rqst");
    r = zdtm_send_rqst_message(cur_env);
    printf(" - (%d).\n", r);
    if(r != 0){ return 1; }
    
    printf("<--- AIG");
    memset(&rmsg, 0, sizeof(zdtm_msg));
    r = zdtm_recv_message(cur_env, &rmsg);
    printf(" - (%d).\n", r);
    if(r != 0) { return 1; }
    r = zdtm_dump_msg_log(cur_env, &rmsg);
    printf("zdtm_dump_msg_log - (%d).\n", r);
    
    memcpy(buff, rmsg.body.cont.aig.model_str,
        rmsg.body.cont.aig.model_str_len);
    buff[rmsg.body.cont.aig.model_str_len] = '\0';
    printf("Model String: %s\n", buff);
    printf("Language: %c%c\n",
        rmsg.body.cont.aig.language[0],
        rmsg.body.cont.aig.language[1]);
    printf("AuthState: 0x%.2x\n", rmsg.body.cont.aig.auth_state);

    printf("---> ack");
    r = zdtm_send_ack_message(cur_env);
    printf(" - (%d).\n", r);
    if(r != 0){ return 1; }

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
