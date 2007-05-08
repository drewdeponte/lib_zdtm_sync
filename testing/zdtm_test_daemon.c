/*
 * Copyright 2005-2007 Andrew De Ponte
 * 
 * This file is part of lib_zdtm_sync.
 * 
 * lib_zdtm_sync is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or any
 * later version.
 * 
 * lib_zdtm_sync is distributed in the hopes that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with lib_zdtm_sync; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

#include "zdtm_sync.h"
#include <stdio.h>
#include <string.h>

void print_todo_item(struct zdtm_todo_item *p_todo_item) {
    char buff[256];

    printf("--== Todo Item ==--\n");
    printf("Sync ID: %u\n", p_todo_item->sync_id);
    memset(buff, 0, 256);
    memcpy(buff, p_todo_item->category, p_todo_item->category_len);
    printf("Category: %s\n", buff);
    printf("Progress: 0x%.2x\n", p_todo_item->progress);
    printf("Priority: 0x%.2x\n", p_todo_item->priority);
    memset(buff, 0, 256);
    memcpy(buff, p_todo_item->description, p_todo_item->description_len);
    printf("Description: %s\n", buff);
    memset(buff, 0, 256);
    memcpy(buff, p_todo_item->notes, p_todo_item->notes_len);
    printf("Notes: %s\n", buff);
}

int test_get_changeinfo(zdtm_lib_env *cur_env) {
    int i, r;
    uint32_t *p_new_sync_ids;
    uint32_t *p_mod_sync_ids;
    uint32_t *p_del_sync_ids;
    uint16_t num_new_sync_ids;
    uint16_t num_mod_sync_ids;
    uint16_t num_del_sync_ids;
    struct zdtm_todo_item todo_item;

    r = zdtm_obtain_sync_id_lists(cur_env, &p_new_sync_ids, &num_new_sync_ids,
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
        printf("Before the call to zdtm_obtain_todo_item.\n");
        r = zdtm_obtain_todo_item(cur_env, p_new_sync_ids[i], &todo_item);
        printf("After the call to zdtm_obtain_todo_item.\n");
        if (r != 0) {
            fprintf(stderr, "ERR(%d): zdtm_obtain_todo_item() failed.\n",
                r);
            return 22;
        }
        print_todo_item(&todo_item);
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

    if (argc > 2) {
        printf("Usage: %s [passcode]\n", argv[0]);
        return 0;
    }
    
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

    if (argc == 2) {
        r = zdtm_set_passcode(&cur_env, argv[1]);
        if (r != 0) {
            fprintf(stderr, "ERR(%d): zdtm_set_passcode() failed.\n", r);
            return 5;
        }
        printf("Successfully set the passcode.\n");
    } else {
        printf("No passcode given, trying without one.\n");
    }
    
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
