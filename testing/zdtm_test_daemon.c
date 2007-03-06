#include "zdtm_sync.h"
#include <stdio.h>

int test_get_changeinfo(zdtm_lib_env *cur_env) {
    time_t last_time_synced, time_synced;
    //zdtm_msg msg, rmsg;
    int i;
    int r, retval;
    char buff[256];
    uint32_t *p_new_sync_ids;
    uint32_t *p_mod_sync_ids;
    uint32_t *p_del_sync_ids;
    uint16_t num_new_sync_ids;
    uint16_t num_mod_sync_ids;
    uint16_t num_del_sync_ids;

    /* Obtain Device Info from Zaurus */
    r = zdtm_check_cur_auth_state(cur_env);
    if (r < 0) {
        fprintf(stderr, "ERR(%d): zdtm_check_cur_auth_state() failed.\n", r);
        return 5;
    }
    printf("- Obtained Device Info\n");
    printf("\tModel String: %s\n", cur_env->model);
    printf("\tLanguage: %c%c\n", cur_env->language[0], cur_env->language[1]);
    printf("\tCurrent Auth State: 0x%.2x\n", cur_env->cur_auth_state);
    if (r == 1) { /* does require passcode */
        retval = _zdtm_authenticate_passcode(cur_env, "1234");
        if (retval == 0) {
            printf("Successfully authenticated passcode \"1234\".\n");
        } else if (retval == 1) {
            printf("Passcode \"1234\"'s authentication was denied.\n");
            return 5;
        } else {
            fprintf(stderr,
                "ERR(%d): _zdtm_authenticate_passcode() failed.\n", r);
            return 5;
        }
    }
    /* doesn't require passcode */

    /* Obtain Zaurus Sync State */
    r = _zdtm_obtain_sync_state(cur_env);
    if (r != 0) {
        fprintf(stderr, "ERR(%d): _zdtm_obtain_sync_state() failed.\n", r);
        return 5;
    }

    /* Here I check if ToDo Slow Sync is Required */
    if (cur_env->todo_slow_sync_required) {
        /* todo slow sync required */
        printf("ToDo Slow Sync Required!\n");
    }

    if (cur_env->calendar_slow_sync_required) {
        /* cal slow sync required */
        printf("Calendar Slow Sync Required!\n");
    }
    
    if (cur_env->address_book_slow_sync_required) {
        /* address book slow sync required */
        printf("Address Book Slow Sync Required!\n");
    }

    /* Here I get the last time it was synced */
    r = _zdtm_obtain_last_time_synced(cur_env, &last_time_synced);
    if (r != 0) {
        fprintf(stderr, "ERR(%d): _zdtm_obtain_last_time_synced() failed.\n",
            r);
        return 5;
    }
    ctime_r(&last_time_synced, buff);
    printf("Obtained Last Time Synced: %s", buff);

    /* Get the current time of the system and set the last time synced
     * to it. */
    time_synced = time(NULL);
    r = _zdtm_set_last_time_synced(cur_env, time_synced);
    if (r != 0) {
        fprintf(stderr, "ERR(%d): _zdtm_set_last_time_synced() failed.\n",
            r);
        return 6;
    }
    ctime_r(&time_synced, buff);
    printf("Set Last Time Synced: %s", buff);

    /* BEFORE I GO ANY FURTHER I MUST BE SURE THAT A FULL SYNC IS NOT
     * REQUIRED. IF A FULL SYNC IS REQUIRED THEN I SHOULD HANDLE THE
     * FULL SYNC. */
    r = zdtm_requires_slow_sync(cur_env);
    if (r < 0) {
        fprintf(stderr, "ERR(%d): _zdtm_requires_slow_sync() failed.\n",
            r);
        return 7;
    }
    if (r == 1) {
        printf("SLOW SYNC REQUIRED!\n");
        printf("This test_daemon currently does not support slow syncs\n");
        printf("Hence, you must sync your Zaurus with the Windows sync\n");
        printf("software and come back and sync with the test_daemon\n");
        printf("to get beyond this point in the synchronization\n");
        return -2;
    }

    r = _zdtm_obtain_sync_id_lists(cur_env, &p_new_sync_ids, &num_new_sync_ids,
                                   &p_mod_sync_ids, &num_mod_sync_ids,
                                   &p_del_sync_ids, &num_del_sync_ids);
    if (r != 0) {
        fprintf(stderr, "ERR(%d): _zdtm_obtain_sync_id_lists() failed.\n",
            r);
        return 8;
    }
    
    printf("num new sync ids = %u\n", num_new_sync_ids);
    for (i = 0; i < num_new_sync_ids; i++) {
        printf(" -- new sync id [%d] = (hex) 0x%.8x, (dec) %u\n", i,
            (int)p_new_sync_ids[i],
            (int)p_new_sync_ids[i]);
    }
    printf("num mod sync ids = %u\n", num_mod_sync_ids);
    for (i = 0; i < num_mod_sync_ids; i++) {
        printf(" -- mod sync id [%d] = (hex) 0x%.8x, (dec) %u\n", i,
            (int)p_mod_sync_ids[i],
            (int)p_mod_sync_ids[i]);
    }
    printf("num del sync ids = %u\n", num_del_sync_ids);
    for (i = 0; i < num_del_sync_ids; i++) {
        printf(" -- del sync id [%d] = (hex) 0x%.8x, (dec) %u\n", i,
            (int)p_del_sync_ids[i],
            (int)p_del_sync_ids[i]);
    }

    free(p_new_sync_ids);
    free(p_mod_sync_ids);
    free(p_del_sync_ids);

    return 0;
}

int main(int argc, char *argv[]) {
    zdtm_lib_env cur_env;
    int r;
    
    memset(&cur_env, 0, sizeof(zdtm_lib_env));
    
    r = zdtm_initialize(&cur_env);
    if (r != 0) {
        fprintf(stderr, "ERR(%d): zdtm_initialize() failed.\n", r);
        return 1;
    }

    printf("Succsessfully initialized zdtm sync library.\n");
    
    r = zdtm_set_zaurus_ip(&cur_env, "192.168.129.201");
    if (r != 0) {
        fprintf(stderr, "ERR(%d): zdtm_set_zaurus_ip() failed.\n", r);
        return 2;
    }

    printf("Successfully set the Zaurus IP address.\n");

    r = zdtm_set_sync_type(&cur_env, 0);
    if (r != 0) {
        fprintf(stderr, "ERR(%d): zdtm_set_sync_type() failed.\n", r);
        return 3;
    }
    
    printf("Successfully set the Synchronization Type.\n");
    
    r = zdtm_initiate_sync(&cur_env);
    if (r != 0) {
        fprintf(stderr, "ERR(%d): zdtm_initiate_sync() failed.\n", r);
        return 4;
    }
    printf("- Initiated Synchronization\n");

    r = test_get_changeinfo(&cur_env);
    printf("get_changeinfo - (%d).\n", r);
    if(r == 0) {
        r = _zdtm_state_sync_done(&cur_env);
        if (r != 0) {
            fprintf(stderr, "ERR(%d): _zdtm_state_sync_done() failed.\n", r);
            return 6;
        }
    }

    r = zdtm_terminate_sync(&cur_env);
    if (r != 0) {
        fprintf(stderr, "ERR(%d): zdtm_terminate_sync() failed.\n", r);
        return 7;
    }

    printf("Successfully terminated synchronization.\n");

    r = zdtm_finalize(&cur_env);
    if(r != 0) {
        fprintf(stderr, "Error(%d): Failed to finalize the zdtm library.\n",
                r);
        return 8;
    }

    printf("Successfully finalized the zdtm sync library.\n");

    return 0;
}
