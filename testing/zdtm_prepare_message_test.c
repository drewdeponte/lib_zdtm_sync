#include "zdtm_sync.h"
#include <stdio.h>

void dump_msg(zdtm_msg *msg)
{
    int i;

    /* Dump the header: */
    printf("Header: ");
    for(i = 0; i < MSG_HDR_SIZE; ++i)
        printf("0x%x ", (unsigned char)msg->header[i]);
    printf("\n");

    /* Dump the raw body in hex. */
    printf("Body (Hex): ");
    for(i = 0; i < msg->body_size; ++i)
        printf("0x%x ", ((unsigned char*)(msg->body.p_raw_content))[i]);
    printf("\n");

    printf("Content Size: %u\n", msg->cont_size);
    printf("Checksum: 0x%x\n", msg->check_sum);
}

int main(int argc, char *argv[])
{
    int r;

    /* Set up a fake message */
    zdtm_lib_env cur_env;
    zdtm_msg msg;
    
    memset(&cur_env, 0, sizeof(zdtm_lib_env));

    /* Make a simply RAY msg. */
    printf("RAY\n");
    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RAY_MSG_TYPE, MSG_TYPE_SIZE);
    r = zdtm_prepare_message(&cur_env, &msg);
    if(r < 0){ printf("Failed: %d\n", r); return 1; }
    dump_msg(&msg);

    /* Make a simply RIG msg. */
    printf("RIG\n");
    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RIG_MSG_TYPE, MSG_TYPE_SIZE);
    r = zdtm_prepare_message(&cur_env, &msg);
    if(r < 0){ printf("Failed: %d\n", r); return 1; }
    dump_msg(&msg);

    /* Make a simply RRL msg. */
    printf("RRL pw abcd\n");
    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RRL_MSG_TYPE, MSG_TYPE_SIZE);
    msg.body.cont.rrl.pw_size = 4;
    msg.body.cont.rrl.pw = "abcd";
    r = zdtm_prepare_message(&cur_env, &msg);
    if(r < 0){ printf("Failed: %d\n", r); return 1; }
    dump_msg(&msg);
    
    /* Make a simply RMG msg. */
    printf("RMG\n");
    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RMG_MSG_TYPE, MSG_TYPE_SIZE);
    msg.body.cont.rmg.uk = 0x01;
    msg.body.cont.rmg.sync_type = SYNC_TYPE_TODO;
    r = zdtm_prepare_message(&cur_env, &msg);
    if(r < 0){ printf("Failed: %d\n", r); return 1; }
    dump_msg(&msg);
    
    /* Make a simply RMS msg. */
    printf("RMS\n");
    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RMS_MSG_TYPE, MSG_TYPE_SIZE);
    msg.body.cont.rms.log_size = 12;
    memset(msg.body.cont.rms.log, 0xfe, 12);
    r = zdtm_prepare_message(&cur_env, &msg);
    if(r < 0){ printf("Failed: %d\n", r); return 1; }
    dump_msg(&msg);

    /* Make a simply RTG msg. */
    printf("RTG\n");
    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RTG_MSG_TYPE, MSG_TYPE_SIZE);
    r = zdtm_prepare_message(&cur_env, &msg);
    if(r < 0){ printf("Failed: %d\n", r); return 1; }
    dump_msg(&msg);
    
    /* Make a simply RTS msg. */
    printf("RTS\n");
    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RTS_MSG_TYPE, MSG_TYPE_SIZE);
    memcpy(msg.body.cont.rts.date, "20060521010000", RTS_DATE_LEN);
    r = zdtm_prepare_message(&cur_env, &msg);
    if(r < 0){ printf("Failed: %d\n", r); return 1; }
    dump_msg(&msg);
    
    /* Make a simply RDI msg. */
    printf("RDI\n");
    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RDI_MSG_TYPE, MSG_TYPE_SIZE);
    msg.body.cont.rdi.sync_type = SYNC_TYPE_TODO;
    msg.body.cont.rdi.uk = 0x07;
    r = zdtm_prepare_message(&cur_env, &msg);
    if(r < 0){ printf("Failed: %d\n", r); return 1; }
    dump_msg(&msg);

    /* Make a simply RSY msg. */
    printf("RSY\n");
    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RSY_MSG_TYPE, MSG_TYPE_SIZE);
    msg.body.cont.rsy.sync_type = SYNC_TYPE_TODO;
    msg.body.cont.rsy.uk = 0x07;
    r = zdtm_prepare_message(&cur_env, &msg);
    if(r < 0){ printf("Failed: %d\n", r); return 1; }
    dump_msg(&msg);

    /* Make a simply RDR msg. */
    printf("RDR\n");
    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RDR_MSG_TYPE, MSG_TYPE_SIZE);
    msg.body.cont.rdr.sync_type = SYNC_TYPE_TODO;
    msg.body.cont.rdr.num_sync_ids = 1;
    msg.body.cont.rdr.sync_id = 0xdeadbeef;
    r = zdtm_prepare_message(&cur_env, &msg);
    if(r < 0){ printf("Failed: %d\n", r); return 1; }
    dump_msg(&msg);

    /* Make a simply RDW msg var 1. */
    printf("RDW - 1\n");
    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RDW_MSG_TYPE, MSG_TYPE_SIZE);
    msg.body.cont.rdw.sync_type = SYNC_TYPE_TODO;
    msg.body.cont.rdw.num_sync_ids = 1;
    msg.body.cont.rdw.sync_id = 0xdeadbeef;
    msg.body.cont.rdw.variation = 1;
    memset(msg.body.cont.rdw.vars.one.padding, 0xff, sizeof(msg.body.cont.rdw.vars.one.padding));
    msg.body.cont.rdw.cont.todo.category_len = 5;
    msg.body.cont.rdw.cont.todo.category = "CRUD";
    memcpy(msg.body.cont.rdw.cont.todo.start_date, "abcde", 5);
    memcpy(msg.body.cont.rdw.cont.todo.due_date, "abcde", 5);
    memcpy(msg.body.cont.rdw.cont.todo.completed_date, "abcde", 5);
    msg.body.cont.rdw.cont.todo.progress = 0xa5;
    msg.body.cont.rdw.cont.todo.priority = 0xff;
    msg.body.cont.rdw.cont.todo.description_len = 5;
    msg.body.cont.rdw.cont.todo.description = "CRUD";
    msg.body.cont.rdw.cont.todo.notes_len = 5;
    msg.body.cont.rdw.cont.todo.notes = "CRUD";

    r = zdtm_prepare_message(&cur_env, &msg);
    if(r < 0){ printf("Failed: %d\n", r); return 1; }
    dump_msg(&msg);

    /* Make a simply RDW msg var 2. */
    printf("RDW - 2\n");
    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RDW_MSG_TYPE, MSG_TYPE_SIZE);
    msg.body.cont.rdw.sync_type = SYNC_TYPE_TODO;
    msg.body.cont.rdw.num_sync_ids = 1;
    msg.body.cont.rdw.sync_id = 0xdeadbeef;
    msg.body.cont.rdw.variation = 2;

    msg.body.cont.rdw.vars.two.attribute = 0xfe;

    r = zdtm_prepare_message(&cur_env, &msg);
    if(r < 0){ printf("Failed: %d\n", r); return 1; }
    dump_msg(&msg);


    /* Make a simply RDW msg var 3. */
    printf("RDW - 3\n");
    memset(&msg, 0, sizeof(zdtm_msg));
    memcpy(msg.body.type, RDW_MSG_TYPE, MSG_TYPE_SIZE);
    msg.body.cont.rdw.sync_type = SYNC_TYPE_TODO;
    msg.body.cont.rdw.num_sync_ids = 1;
    msg.body.cont.rdw.sync_id = 0xdeadbeef;
    msg.body.cont.rdw.variation = 3;
    msg.body.cont.rdw.vars.three.attribute = 0xfe;
    memcpy(msg.body.cont.rdw.vars.three.card_created_date_time, "abcde", 5);
    msg.body.cont.rdw.cont.todo.category_len = 5;
    msg.body.cont.rdw.cont.todo.category = "CRUD";
    memcpy(msg.body.cont.rdw.cont.todo.start_date, "abcde", 5);
    memcpy(msg.body.cont.rdw.cont.todo.due_date, "abcde", 5);
    memcpy(msg.body.cont.rdw.cont.todo.completed_date, "abcde", 5);
    msg.body.cont.rdw.cont.todo.progress = 0xa5;
    msg.body.cont.rdw.cont.todo.priority = 0xff;
    msg.body.cont.rdw.cont.todo.description_len = 5;
    msg.body.cont.rdw.cont.todo.description = "CRUD";
    msg.body.cont.rdw.cont.todo.notes_len = 5;
    msg.body.cont.rdw.cont.todo.notes = "CRUD";

    r = zdtm_prepare_message(&cur_env, &msg);
    if(r < 0){ printf("Failed: %d\n", r); return 1; }
    dump_msg(&msg);

    return 0;
}
